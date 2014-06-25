/*
 # Example 1 (a gateway injection)
 $ bitcoind -testnet getnewaddress
 mwATgTqtQmAP4obu4tvc7i8Z9Q9qNhxqsN
 $ bitcoind -testnet dumpprivkey mwATgTqtQmAP4obu4tvc7i8Z9Q9qNhxqsN
 cTQSBNmMkbCUdFetsnSfzdAiJcdngQsKLyYWVTKgm6fE9GLN74qR
 $ ./inject gateway cTQSBNmMkbCUdFetsnSfzdAiJcdngQsKLyYWVTKgm6fE9GLN74qR localhost 56344 P-mwATgTqtQmAP4obu4tvc7i8Z9Q9qNhxqsN 100
 6ac5ce095cb096d16ab81cf276486615df8714105bc0672639bbc31bfd8071c1

 # Example 2 (using that gateway injection to pay someone else)
 $ bitcoind -testnet getnewaddress
 mv5fpRMAhaPV9LBrAk3MaBH8FG13TpqTxD
 $ bitcoind -testnet dumpprivkey mv5fpRMAhaPV9LBrAk3MaBH8FG13TpqTxD
 cUjJCgPjWAdsJBm85zwCg7ekLYkeeRRoUmkNk3wYydrhbHYKxnwt
 $ ./inject tx cTQSBNmMkbCUdFetsnSfzdAiJcdngQsKLyYWVTKgm6fE9GLN74qR localhost 56344 50 50 mv5fpRMAhaPV9LBrAk3MaBH8FG13TpqTxD 6ac5ce095cb096d16ab81cf276486615df8714105bc0672639bbc31bfd8071c1/0
*/
#include <ccan/err/err.h>
#include <ccan/net/net.h>
#include <ccan/read_write_all/read_write_all.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netdb.h>
#include "base58.h"
#include "create_transaction.h"
#include "protocol_net.h"
#include "marshall.h"
#include "netaddr.h"
#include "addr.h"
#include "hash_transaction.h"
#include "hash_block.h"
#include "log.h"
#include <string.h>
#include <assert.h>
#include <openssl/obj_mac.h>
#include <signal.h>
#include <unistd.h>

// Thus function based on bitcoin's key.cpp:
// Copyright (c) 2009-2012 The Bitcoin developers
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
static bool EC_KEY_regenerate_key(EC_KEY *eckey, BIGNUM *priv_key)
{
	BN_CTX *ctx = NULL;
	EC_POINT *pub_key = NULL;
	const EC_GROUP *group = EC_KEY_get0_group(eckey);

	if ((ctx = BN_CTX_new()) == NULL)
		return false;

	pub_key = EC_POINT_new(group);
	if (pub_key == NULL)
		return false;

	if (!EC_POINT_mul(group, pub_key, priv_key, NULL, NULL, ctx))
		return false;

	EC_KEY_set_private_key(eckey, priv_key);
	EC_KEY_set_public_key(eckey, pub_key);

	BN_CTX_free(ctx);
	EC_POINT_free(pub_key);
	return true;
}

static EC_KEY *get_privkey(const char *arg, struct protocol_pubkey *gkey)
{
	size_t keylen;
	u8 keybuf[1 + 32 + 1 + 4], *pubkey;
	u8 csum[4];
	EC_KEY *priv = EC_KEY_new_by_curve_name(NID_secp256k1);
	BIGNUM bn;

	if (!raw_decode_base58(&bn, arg))
		errx(1, "Could not decode privkey");

	keylen = BN_num_bytes(&bn);
	/* Pettycoin always uses compressed keys. */
	if (keylen == 1 + 32 + 4)
		errx(1, "Looks like privkey for uncompressed pubkey.");
	if (keylen != 1 + 32 + 1 + 4)
		errx(1, "Privkey length %zu wrong", keylen);
	BN_bn2bin(&bn, keybuf);
	BN_free(&bn);

	base58_get_checksum(csum, keybuf, keylen - sizeof(csum));
	if (memcmp(csum, keybuf + keylen - sizeof(csum), sizeof(csum)) != 0)
		errx(1, "Privkey csum incorrect");

	/* Byte after key should be 1 to represent a compressed key. */
	if (keybuf[1 + 32] != 1)
		errx(1, "Privkey compressed key marker %u incorrect",
		     keybuf[1 + 32]);

	if (keybuf[0] == 128)
		errx(1, "Private key for main network!");
	else if (keybuf[0] != 239)
		errx(1, "Version byte %u not a private key for test network",
			keybuf[0]);

	/* We *always* used compressed form keys. */
	EC_KEY_set_conv_form(priv, POINT_CONVERSION_COMPRESSED);

        BN_init(&bn);
        if (!BN_bin2bn(keybuf + 1, 32, &bn))
		err(1, "Creating bignum from key");
        if (!EC_KEY_regenerate_key(priv, &bn))
		err(1, "Regenerating key");

	/* Save public key */ 
	pubkey = gkey->key;
	keylen = i2o_ECPublicKey(priv, &pubkey);
	assert(keylen == sizeof(gkey->key));

	return priv;
}

static void discard_remainder(int fd, struct protocol_net_hdr *hdr)
{
	size_t len = le32_to_cpu(hdr->len) - sizeof(*hdr);

	while (len) {
		char buf[128];
		size_t rlen = sizeof(buf);

		if (rlen > len)
			rlen = len;
		if (!read_all(fd, buf, rlen))
			err(1, "Reading discarding packet");
		len -= rlen;
	}
}

static void discard_packet(int fd, enum protocol_pkt_type type)
{
	struct protocol_net_hdr hdr;

	if (!read_all(fd, &hdr, sizeof(hdr)))
		err(1, "Reading discarding header");

	if (le32_to_cpu(hdr.type) != type) {
		log_broken(NULL, "Unexpected response, len %u type ",
			   le32_to_cpu(hdr.len));
		log_add_enum(NULL, enum protocol_pkt_type, le32_to_cpu(hdr.type));
		log_broken(NULL, "(expecting ");
		log_add_enum(NULL, enum protocol_pkt_type, type);
		fprintf(stderr, "\n");
		errx(1, "Discarding packet");
	}

	discard_remainder(fd, &hdr);
}

static void welcome_and_init(int fd, const struct protocol_net_address *netaddr)
{
	struct protocol_net_hdr hdr;
	struct protocol_pkt_welcome *w;
	struct protocol_pkt_sync *sync;
	struct protocol_pkt_set_filter filter;

	if (!read_all(fd, &hdr, sizeof(hdr)))
		err(1, "Reading welcome header");

	w = (void *)tal_arr(NULL, char, le32_to_cpu(hdr.len));
	w->len = hdr.len;
	w->type = hdr.type;
	if (!read_all(fd, (char *)w + sizeof(hdr),
		      le32_to_cpu(hdr.len) - sizeof(hdr)))
		err(1, "Reading welcome");

	/* Fix it up, but pretend to be interested in the same. */
	w->random++;
	w->listen_port = 0;
	w->you = *netaddr;

	if (!write_all(fd, w, le32_to_cpu(hdr.len)))
		err(1, "Writing welcome");

	/* They should send us sync. */
	if (!read_all(fd, &hdr, sizeof(hdr)))
		err(1, "Reading sync header");

	sync = (void *)tal_arr(NULL, char, le32_to_cpu(hdr.len));
	sync->len = hdr.len;
	sync->type = hdr.type;
	if (!read_all(fd, (char *)sync + sizeof(hdr),
		      le32_to_cpu(hdr.len) - sizeof(hdr)))
		err(1, "Reading sync");

	/* Now send sync back to them. */
	if (!write_all(fd, sync, le32_to_cpu(sync->len)))
		err(1, "Writing sync");

	filter.len = cpu_to_le32(sizeof(filter));
	filter.type = cpu_to_le32(PROTOCOL_PKT_SET_FILTER);
	/* FIXME: Allow 0?  Then we won't get traffic. */
	filter.filter = cpu_to_le64(0xFFFFFFFFFFFFFFFFULL);
	filter.offset = cpu_to_le64(0);

	if (!write_all(fd, &filter, sizeof(filter)))
		err(1, "Writing filter");

	/* They should send their filter, ignore. */
	discard_packet(fd, PROTOCOL_PKT_SET_FILTER);
}

#if 0
static void ignore_new_block(int fd, struct protocol_net_hdr *hdr)
{
	size_t len = le32_to_cpu(hdr->len);
	struct protocol_req_new_block *nb;
	const struct protocol_double_sha *merkles;
	const u8 *prev_merkles;
	const struct protocol_block_tailer *tailer;
	struct protocol_block_header *bhdr;
	struct protocol_resp_new_block resp;

	nb = malloc(len);
	memcpy(nb, hdr, sizeof(*hdr));
	bhdr = (void *)nb->block;
	len -= sizeof(*hdr);
	if (!read_all(fd, bhdr, len))
		err(1, "Reading new block");
	if (unmarshall_block(NULL, len, bhdr, &merkles, &prev_merkles, &tailer))
		err(1, "Unmarshalling block");

	/* If we don't tell it that's our latest, it will keep sending. */
	resp.len = cpu_to_le32(sizeof(resp));
	resp.type = cpu_to_le32(PROTOCOL_RESP_NEW_BLOCK);
	hash_block(bhdr, merkles, prev_merkles, tailer, &resp.final);

	log_unusual(NULL, "Ignoring new block ");
	log_add_struct(NULL, struct protocol_double_sha, &resp.final);
	if (!write_all(fd, &resp, sizeof(resp)))
		err(1, "Writing new block response");
}

static void ignore_batch_request(int fd, struct protocol_net_hdr *hdr)
{
	struct protocol_req_batch req;
	struct protocol_resp_batch resp;

	assert(le32_to_cpu(hdr->len) == sizeof(req));
	if (!read_all(fd, (char *)&req + sizeof(*hdr), sizeof(req) - sizeof(*hdr)))
		err(1, "Reading req batch");

	resp.len = cpu_to_le32(sizeof(resp));
	resp.type = cpu_to_le32(PROTOCOL_RESP_BATCH);
	resp.error = cpu_to_le32(PROTOCOL_ECODE_UNKNOWN_BLOCK);
	resp.num = cpu_to_le32(0);

	log_unusual(NULL, "Ignoring batch request");
	if (!write_all(fd, &resp, sizeof(resp)))
		err(1, "Writing batch response");
}
#endif

static volatile int closing_fd;

/* This will stop read. */
static void close_fd(int signum)
{
	close(closing_fd);
	closing_fd = -1;
}

static void read_response(int fd)
{
	struct protocol_net_hdr hdr;

	/* We wait for up to a second, in case it sends courtesy error */
	closing_fd = fd;
	signal(SIGALRM, close_fd);
	alarm(1);

	if (!read_all(fd, &hdr, sizeof(hdr))) {
		if (closing_fd == -1)
			return;
		err(1, "Reading response");
	}

	/* FIXME: Handle non-error responses! */
	log_broken(NULL, "Unexpected response, len %u type ",
		   le32_to_cpu(hdr.len));
	log_add_enum(NULL, enum protocol_pkt_type, le32_to_cpu(hdr.type));
	fprintf(stderr, "\n");
	err(1, "Got response");
}

static void usage(void)
{
	errx(1, "Usage: inject gateway <privkey> <peer> <port> <dstaddr> <satoshi>\n"
		"   inject tx <privkey> <peer> <port> <dstaddr> <satout> <change> <tx>[/<out>]...");
}

static struct protocol_double_sha parse_sha(const char *shastr)
{
	unsigned int i;
	struct protocol_double_sha sha;

	for (i = 0; i < 32; i++) {
		unsigned int v;

		if (sscanf(shastr + i*2, "%02x", &v) != 1)
			errx(1, "Bad sha '%s'", shastr);
		sha.sha[i] = v;
	}
	return sha;
}

/* Simple test code to create a gateway transaction */
int main(int argc, char *argv[])
{
	union protocol_transaction *t;
	struct protocol_gateway_payment payment;
	struct protocol_net_address netaddr;
	bool test_net;
	struct addrinfo *a;
	int fd;
	size_t len;
	struct protocol_net_hdr hdr;
	bool gateway = false;
	bool tx = false;
	struct protocol_double_sha sha;

	if (argv[1] && streq(argv[1], "gateway"))
		gateway = true;
	else if (argv[1] && streq(argv[1], "tx"))
		tx = true;
	else
		usage();

	if (gateway) {
		struct protocol_pubkey gkey;
		EC_KEY *key;

		if (argc != 7)
			usage();
		key = get_privkey(argv[2], &gkey);

		payment.send_amount = cpu_to_le32(atoi(argv[6]));
		if (!pettycoin_from_base58(&test_net, &payment.output_addr,
					   argv[5]))
			errx(1, "Invalid dstaddr");
		if (!test_net)
			errx(1, "dstaddr is not on test net!");

		log_unusual(NULL, "Destination address is: ");
		log_add_struct(NULL, struct protocol_address,
			       &payment.output_addr);

		t = create_gateway_transaction(NULL, &gkey, 1, 0, &payment, key);
	} else if (tx) {
		struct protocol_pubkey destkey;
		EC_KEY *key;
		struct protocol_input input[argc - 8];
		unsigned int i;
		struct protocol_address destaddr;

		if (argc < 9)
			usage();

		if (!pettycoin_from_base58(&test_net, &destaddr, argv[5]))
			errx(1, "Invalid dstaddr %s", argv[5]);
		if (!test_net)
			errx(1, "dstaddr is not on test net!");

		key = get_privkey(argv[2], &destkey);
		for (i = 0; i < argc - 8; i++) {
			input[i].input = parse_sha(argv[8+i]);
			if (argv[8+i][64] == '/')
				input[i].output = cpu_to_le16(atoi(argv[8+i] + 65));
			else
				input[i].output = cpu_to_le16(0);
			input[i].unused = cpu_to_le16(0);
		}
		t = create_normal_transaction(NULL, &destaddr, atoi(argv[6]),
					      atoi(argv[7]), argc - 8, input,
					      key);
		{
			struct protocol_pubkey pubkey;
			unsigned char *p = (void *)&pubkey;
			struct protocol_address addr;

			if (i2o_ECPublicKey(key, &p) != sizeof(pubkey))
				abort();

			pubkey_to_addr(&pubkey, &addr);
			log_unusual(NULL, "Our address is: ");
			log_add_struct(NULL, struct protocol_address, &addr);
		}
	}

	len = marshall_transaction_len(t);
	if (!len)
		errx(1, "Marshalling transaction failed");

	a = net_client_lookup(argv[3], argv[4], AF_UNSPEC, SOCK_STREAM);
	if (!a)
		errx(1, "Failed to look up address %s:%s", argv[2], argv[3]);

	if (!addrinfo_to_netaddr(&netaddr, a))
		err(1, "Failed to convert net address");

	fd = net_connect(a);
	if (fd < 0)
		err(1, "Failed to connect to %s:%s", argv[3], argv[4]);
	freeaddrinfo(a);

	welcome_and_init(fd, &netaddr);

	hdr.len = cpu_to_le32(len + sizeof(struct protocol_net_hdr));
	hdr.type = cpu_to_le32(PROTOCOL_PKT_TX);
	if (!write_all(fd, &hdr, sizeof(hdr)))
		err(1, "Failed writing header");
	if (!write_all(fd, t, len))
		err(1, "Failed writing transaction");

	read_response(fd);

	hash_tx(t, &sha);
	printf("%02x%02x%02x%02x%02x%02x%02x%02x"
	       "%02x%02x%02x%02x%02x%02x%02x%02x"
	       "%02x%02x%02x%02x%02x%02x%02x%02x"
	       "%02x%02x%02x%02x%02x%02x%02x%02x\n",
	       sha.sha[0], sha.sha[1], sha.sha[2], sha.sha[3],
	       sha.sha[4], sha.sha[5], sha.sha[6], sha.sha[7],
	       sha.sha[8], sha.sha[9], sha.sha[10], sha.sha[11],
	       sha.sha[12], sha.sha[13], sha.sha[14], sha.sha[15],
	       sha.sha[16], sha.sha[17], sha.sha[18], sha.sha[19],
	       sha.sha[20], sha.sha[21], sha.sha[22], sha.sha[23],
	       sha.sha[24], sha.sha[25], sha.sha[26], sha.sha[27],
	       sha.sha[28], sha.sha[29], sha.sha[30], sha.sha[31]);
	return 0;
}
