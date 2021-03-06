#include "block.h"
#include "marshal.h"
#include "protocol.h"
#include "protocol_net.h"
#include "tal_packet.h"
#include <assert.h>

void *tal_packet_(const tal_t *ctx, size_t len, int type)
{
	struct protocol_net_hdr *hdr;

	assert(len >= sizeof(*hdr));
	assert(len < PROTOCOL_MAX_PACKET_LEN);

	/* Must be a char array so that tal_count() is in bytes */
	hdr = (void *)tal_arr(ctx, char, len);

	hdr->len = cpu_to_le32(len);
	hdr->type = cpu_to_le32(type);

	return hdr;
}

void *tal_packet_dup(const tal_t *ctx, const void *pkt)
{
	const struct protocol_net_hdr *hdr = pkt;
	size_t len = le32_to_cpu(hdr->len);

	assert(len >= sizeof(*hdr));
	return tal_dup(ctx, char, (char *)pkt, len, 0);
}

void tal_packet_append_(void *ppkt, const void *mem, size_t len)
{
	struct protocol_net_hdr **hdr = ppkt;
	u32 orig_len = le32_to_cpu((*hdr)->len);

	tal_resize((char **)ppkt, orig_len + len);
	hdr = ppkt;
	memcpy((char *)*hdr + orig_len, mem, len);
	(*hdr)->len = cpu_to_le32(orig_len + len);
}

void tal_packet_append_tx_(void *ppkt, const union protocol_tx *tx)
{
	tal_packet_append_(ppkt, tx, tx_len(tx));
}

void tal_packet_append_block_(void *ppkt, const struct block_info *bi)
{
	struct protocol_net_hdr **hdr = ppkt;
	u32 orig_len = le32_to_cpu((*hdr)->len);
	size_t len = marshal_block_len(bi->hdr);

	tal_resize((char **)ppkt, orig_len + len);
	hdr = ppkt;
	marshal_block_into((char *)*hdr + orig_len, bi);
	(*hdr)->len = cpu_to_le32(orig_len + len);
}

void tal_packet_append_sha_(void *ppkt, const struct protocol_double_sha *sha)
{
	tal_packet_append_(ppkt, sha, sizeof(*sha));
}

void tal_packet_append_block_id_(void *ppkt,
				 const struct protocol_block_id *id)
{
	tal_packet_append_sha_(ppkt, &id->sha);
}

void tal_packet_append_tx_id_(void *ppkt,
			      const struct protocol_tx_id *id)
{
	tal_packet_append_sha_(ppkt, &id->sha);
}

void tal_packet_append_txrefhash_(void *ppkt,
				  const struct protocol_txrefhash *hashes)
{
	tal_packet_append_(ppkt, hashes, sizeof(*hashes));
}

void tal_packet_append_pos_(void *ppkt, const struct protocol_block_id *block,
			    u16 shardnum, u8 txoff)
{
	struct protocol_position pos;

	pos.block = *block;
	pos.shard = cpu_to_le16(shardnum);
	pos.txoff = txoff;
	pos.unused = 0;

	tal_packet_append_(ppkt, &pos, sizeof(pos));
}

void tal_packet_append_proven_tx_(void *ppkt,
				  const struct protocol_proof *proof,
				  const union protocol_tx *tx,
				  const struct protocol_input_ref *refs)
{
	tal_packet_append_(ppkt, proof, sizeof(*proof));
	tal_packet_append_tx_(ppkt, tx);
	tal_packet_append_(ppkt, refs, marshal_input_ref_len(tx));
}

void tal_packet_append_net_address_(void *ppkt,
				    const struct protocol_net_address *addr)
{
	tal_packet_append_(ppkt, addr, sizeof(*addr));
}
