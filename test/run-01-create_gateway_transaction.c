#include "../create_tx.c"
#include "../check_tx.c"
#include "../shadouble.c"
#include "../signature.c"
#include "../txhash.c"
#include "../inputhash.c"
#include "../minimal_log.c"
#include "../marshal.c"
#include "../proof.c"
#include "../shard.c"
#include "../merkle_txs.c"
#include "../merkle_recurse.c"
#include "../hash_tx.c"
#include "../tx.c"
#include "../block_shard.c"
#include <assert.h>
#include "helper_gateway_key.h"
#include "helper_key.h"
#include "helper_fakenewstate.h"

/* AUTOGENERATED MOCKS START */
/* Generated stub for block_get_tx */
union protocol_tx *block_get_tx(const struct block *block, u16 shardnum,
				u8 txoff)
{ fprintf(stderr, "block_get_tx called!\n"); abort(); }
/* Could not find declaration for helper_addr */
/* Could not find declaration for helper_gateway_addr */
/* Could not find declaration for helper_gateway_key */
/* Could not find declaration for helper_gateway_public_key */
/* Could not find declaration for helper_private_key */
/* Could not find declaration for helper_public_key */
/* Generated stub for reward_amount */
u32 reward_amount(const struct block *reward_block,
		  const union protocol_tx *tx)
{ fprintf(stderr, "reward_amount called!\n"); abort(); }
/* Generated stub for reward_get_tx */
bool reward_get_tx(struct state *state,
		   const struct block *reward_block,
		   const struct block *claim_block,
		   u16 *shardnum, u8 *txoff)
{ fprintf(stderr, "reward_get_tx called!\n"); abort(); }
/* Generated stub for txhash_gettx_ancestor */
struct txhash_elem *txhash_gettx_ancestor(struct state *state,
					  const struct protocol_tx_id *sha,
					  const struct block *block)
{ fprintf(stderr, "txhash_gettx_ancestor called!\n"); abort(); }
/* AUTOGENERATED MOCKS END */

bool accept_gateway(const struct state *state,
		    const struct protocol_address *addr)
{
	return (memcmp(addr, helper_gateway_addr(), sizeof(*addr)) == 0);
}

int main(int argc, char *argv[])
{
	union protocol_tx *t;
	struct state *s = fake_new_state();
	struct protocol_gateway_payment *payment;
	struct protocol_gateway_payment *out;

	/* Single payment. */
	payment = tal_arr(s, struct protocol_gateway_payment, 1);
	payment[0].send_amount = cpu_to_le32(1000);
	payment[0].output_addr = *helper_addr(0);
	t = create_from_gateway_tx(s, helper_gateway_public_key(),
				   1, payment, true, helper_gateway_key(s));
	assert(t);
	out = get_from_gateway_outputs(&t->from_gateway);
	assert(t->from_gateway.version == current_version());
	assert(version_ok(t->from_gateway.version));
	assert(t->from_gateway.features == 0);
	assert(memcmp(&t->from_gateway.gateway_key, helper_gateway_public_key(),
		      sizeof(t->from_gateway.gateway_key)) == 0);
	assert(le16_to_cpu(t->from_gateway.num_outputs) == 1);
	assert(le16_to_cpu(t->from_gateway.unused) == 0);
	assert(le32_to_cpu(out[0].send_amount) == 1000);
	assert(memcmp(&out[0].output_addr, helper_addr(0),
		      sizeof(out[0].output_addr)) == 0);

	assert(check_tx(s, t, NULL) == PROTOCOL_ECODE_NONE);
	assert(check_tx_inputs(s, NULL, NULL, t, NULL) == ECODE_INPUT_OK);

	/* Two payments (must be same shard!) */
	payment = tal_arr(s, struct protocol_gateway_payment, 2);
	payment[0].send_amount = cpu_to_le32(1000);
	payment[0].output_addr = *helper_addr(0);
	payment[1].send_amount = cpu_to_le32(2000);
	payment[1].output_addr = *helper_addr(1);
	t = create_from_gateway_tx(s, helper_gateway_public_key(),
				   2, payment, false, helper_gateway_key(s));
	assert(t);
	out = get_from_gateway_outputs(&t->from_gateway);
	assert(t->from_gateway.version == current_version());
	assert(version_ok(t->from_gateway.version));
	assert(t->from_gateway.features == 0);
	assert(le16_to_cpu(t->from_gateway.unused) == 0);
	assert(memcmp(&t->from_gateway.gateway_key, helper_gateway_public_key(),
		      sizeof(t->from_gateway.gateway_key)) == 0);
	assert(le16_to_cpu(t->from_gateway.num_outputs) == 2);
	assert(le32_to_cpu(out[0].send_amount) == 1000);
	assert(memcmp(&out[0].output_addr, helper_addr(0),
		      sizeof(out[0].output_addr)) == 0);
	assert(le32_to_cpu(out[1].send_amount) == 2000);
	assert(memcmp(&out[1].output_addr, helper_addr(1),
		      sizeof(out[1].output_addr)) == 0);

	assert(check_tx(s, t, NULL) == PROTOCOL_ECODE_NONE);
	assert(check_tx_inputs(s, NULL, NULL, t, NULL) == ECODE_INPUT_OK);

	/* Now try changing it. */
	t->from_gateway.version++;
	assert(check_tx(s, t, NULL) == PROTOCOL_ECODE_TX_HIGH_VERSION);
	t->from_gateway.version--;

	t->from_gateway.features++;
	assert(check_tx(s, t, NULL) == PROTOCOL_ECODE_TX_BAD_SIG);
	t->from_gateway.features--;

	t->from_gateway.type ^= 0xFF;
	assert(check_tx(s, t, NULL) == PROTOCOL_ECODE_TX_TYPE_UNKNOWN);
	t->from_gateway.type ^= 0xFF;

	t->from_gateway.num_outputs = cpu_to_le16(le16_to_cpu(t->from_gateway.num_outputs)
					     - 1);
	assert(check_tx(s, t, NULL) == PROTOCOL_ECODE_TX_BAD_SIG);
	t->from_gateway.num_outputs = cpu_to_le16(le16_to_cpu(t->from_gateway.num_outputs)
					     + 1);

	out[0].send_amount ^= cpu_to_le32(1);
	assert(check_tx(s, t, NULL) == PROTOCOL_ECODE_TX_BAD_SIG);
	out[0].send_amount ^= cpu_to_le32(1);

	out[0].output_addr.addr[0]++;
	assert(check_tx(s, t, NULL) == PROTOCOL_ECODE_TX_BAD_SIG);
	out[0].output_addr.addr[0]--;

	out[1].send_amount ^= cpu_to_le32(1);
	assert(check_tx(s, t, NULL) == PROTOCOL_ECODE_TX_BAD_SIG);
	out[1].send_amount ^= cpu_to_le32(1);

	out[1].output_addr.addr[0]++;
	assert(check_tx(s, t, NULL) == PROTOCOL_ECODE_TX_BAD_SIG);
	out[1].output_addr.addr[0]--;

	/* We restored it ok? */
	assert(check_tx(s, t, NULL) == PROTOCOL_ECODE_NONE);
	assert(check_tx_inputs(s, NULL, NULL, t, NULL) == ECODE_INPUT_OK);

	/* Try signing it with non-gateway key. */
	t = create_from_gateway_tx(s, helper_public_key(0),
				   2, payment, false,
				   helper_private_key(s, 0));
	assert(check_tx(s, t, NULL) == PROTOCOL_ECODE_TX_BAD_GATEWAY);

	tal_free(s);
	return 0;
}
