#include <ccan/asort/asort.h>
#include <ccan/time/time.h>
#include <assert.h>

/* Override time_now in timestamp.h */
static time_t fake_time;
static struct timeabs fake_time_now(void)
{
	struct timeabs now;

	now.ts.tv_sec = fake_time;
	now.ts.tv_nsec = 0;

	return now;
}
#undef time_now
#define time_now fake_time_now

int generate_main(int argc, char *argv[]);
#define main generate_main

#include "../timestamp.c"
#include "../pettycoin-generate.c"
#undef main
#include "helper_key.h"
#include "helper_gateway_key.h"
#include "../hash_block.c"
#include "../shadouble.c"
#include "../difficulty.c"
#include "../merkle_txs.c"
#include "../merkle_recurse.c"
#include "../merkle_hashes.c"
#include "../tx_cmp.c"
#include "../marshal.c"
#include "../hash_tx.c"
#include "../create_tx.c"
#include "../check_block.c"
#include "../block.c"
#include "../block_shard.c"
#include "../prev_txhashes.c"
#include "../prev_blocks.c"
#include "../minimal_log.c"
#include "../signature.c"
#include "../txhash.c"
#include "../inputhash.c"
#include "../shard.c"
#include "../chain.c"
#include "../check_tx.c"
#include "../features.c"
#include "../tal_packet.c"
#include "../gateways.c"
#include "../state.c"
#include "../pseudorand.c"
#include "../create_refs.c"
#include "../tx.c"
#include "../proof.c"
#include "../tx_in_hashes.c"
#include "../horizon.c"
#include "easy_genesis.c"

/* AUTOGENERATED MOCKS START */
/* Generated stub for block_to_pending */
void block_to_pending(struct state *state, const struct block *block)
{ fprintf(stderr, "block_to_pending called!\n"); abort(); }
/* Generated stub for check_tx_refs */
enum ref_ecode check_tx_refs(struct state *state,
			     const struct block *block,
			     const union protocol_tx *tx,
			     const struct protocol_input_ref *refs,
			     unsigned int *bad_ref,
			     struct block **block_referred_to)
{ fprintf(stderr, "check_tx_refs called!\n"); abort(); }
/* Generated stub for complain_bad_amount */
void complain_bad_amount(struct state *state,
			 struct block *block,
			 const struct protocol_proof *proof,
			 const union protocol_tx *tx,
			 const struct protocol_input_ref *refs,
			 const union protocol_tx *intx[])
{ fprintf(stderr, "complain_bad_amount called!\n"); abort(); }
/* Generated stub for complain_bad_claim */
void complain_bad_claim(struct state *state,
			struct block *claim_block,
			const struct protocol_proof *claim_proof,
			const union protocol_tx *claim_tx,
			const struct protocol_input_ref *claim_refs,
			const struct block *reward_block,
			u16 reward_shard, u8 reward_txoff)
{ fprintf(stderr, "complain_bad_claim called!\n"); abort(); }
/* Generated stub for complain_bad_input */
void complain_bad_input(struct state *state,
			struct block *block,
			const struct protocol_proof *proof,
			const union protocol_tx *tx,
			const struct protocol_input_ref *refs,
			unsigned int bad_input,
			const union protocol_tx *intx)
{ fprintf(stderr, "complain_bad_input called!\n"); abort(); }
/* Generated stub for complain_bad_input_ref */
void complain_bad_input_ref(struct state *state,
			    struct block *block,
			    const struct protocol_proof *proof,
			    const union protocol_tx *tx,
			    const struct protocol_input_ref *refs,
			    unsigned int bad_refnum,
			    const struct block *block_referred_to)
{ fprintf(stderr, "complain_bad_input_ref called!\n"); abort(); }
/* Generated stub for complain_bad_prev_txhashes */
void complain_bad_prev_txhashes(struct state *state,
				struct block *block,
				const struct block *bad_prev,
				u16 bad_prev_shard)
{ fprintf(stderr, "complain_bad_prev_txhashes called!\n"); abort(); }
/* Generated stub for complain_doublespend */
void complain_doublespend(struct state *state,
			  struct block *block1,
			  u32 input1,
			  const struct protocol_proof *proof1,
			  const union protocol_tx *tx1,
			  const struct protocol_input_ref *refs1,
			  struct block *block2,
			  u32 input2,
			  const struct protocol_proof *proof2,
			  const union protocol_tx *tx2,
			  const struct protocol_input_ref *refs2)
{ fprintf(stderr, "complain_doublespend called!\n"); abort(); }
/* Generated stub for from_hex */
bool from_hex(const char *str, size_t slen, void *buf, size_t bufsize)
{ fprintf(stderr, "from_hex called!\n"); abort(); }
/* Could not find declaration for helper_addr */
/* Could not find declaration for helper_gateway_key */
/* Could not find declaration for helper_gateway_public_key */
/* Could not find declaration for helper_private_key */
/* Generated stub for json_add_address */
void json_add_address(struct json_result *result, const char *fieldname,
		      bool test_net,  const struct protocol_address *addr)
{ fprintf(stderr, "json_add_address called!\n"); abort(); }
/* Generated stub for json_add_block_id */
void json_add_block_id(struct json_result *result, const char *fieldname,
		       const struct protocol_block_id *id)
{ fprintf(stderr, "json_add_block_id called!\n"); abort(); }
/* Generated stub for json_add_double_sha */
void json_add_double_sha(struct json_result *result, const char *fieldname,
			 const struct protocol_double_sha *sha)
{ fprintf(stderr, "json_add_double_sha called!\n"); abort(); }
/* Generated stub for json_add_hex */
void json_add_hex(struct json_result *result, const char *fieldname,
		  const void *data, size_t len)
{ fprintf(stderr, "json_add_hex called!\n"); abort(); }
/* Generated stub for json_add_num */
void json_add_num(struct json_result *result, const char *fieldname,
		  unsigned int value)
{ fprintf(stderr, "json_add_num called!\n"); abort(); }
/* Generated stub for json_add_tx_id */
void json_add_tx_id(struct json_result *result, const char *fieldname,
		    const struct protocol_tx_id *id)
{ fprintf(stderr, "json_add_tx_id called!\n"); abort(); }
/* Generated stub for json_array_end */
void json_array_end(struct json_result *ptr)
{ fprintf(stderr, "json_array_end called!\n"); abort(); }
/* Generated stub for json_array_start */
void json_array_start(struct json_result *ptr, const char *fieldname)
{ fprintf(stderr, "json_array_start called!\n"); abort(); }
/* Generated stub for json_get_params */
void json_get_params(const char *buffer, const jsmntok_t param[], ...)
{ fprintf(stderr, "json_get_params called!\n"); abort(); }
/* Generated stub for json_object_end */
void json_object_end(struct json_result *ptr)
{ fprintf(stderr, "json_object_end called!\n"); abort(); }
/* Generated stub for json_object_start */
void json_object_start(struct json_result *ptr, const char *fieldname)
{ fprintf(stderr, "json_object_start called!\n"); abort(); }
/* Generated stub for json_tok_contents */
const char *json_tok_contents(const char *buffer, const jsmntok_t *t)
{ fprintf(stderr, "json_tok_contents called!\n"); abort(); }
/* Generated stub for json_tok_len */
int json_tok_len(const jsmntok_t *t)
{ fprintf(stderr, "json_tok_len called!\n"); abort(); }
/* Generated stub for json_tok_number */
bool json_tok_number(const char *buffer, const jsmntok_t *tok,
		     unsigned int *num)
{ fprintf(stderr, "json_tok_number called!\n"); abort(); }
/* Generated stub for log_to_file */
void log_to_file(int fd, const struct log_record *lr)
{ fprintf(stderr, "log_to_file called!\n"); abort(); }
/* Generated stub for logv */
void logv(struct log *log, enum log_level level, const char *fmt, va_list ap)
{ fprintf(stderr, "logv called!\n"); abort(); }
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
/* Generated stub for todo_add_get_tx */
void todo_add_get_tx(struct state *state, const struct protocol_tx_id *tx)
{ fprintf(stderr, "todo_add_get_tx called!\n"); abort(); }
/* Generated stub for todo_add_get_tx_in_block */
void todo_add_get_tx_in_block(struct state *state,
			      const struct protocol_block_id *block,
			      u16 shardnum, u8 txoff)
{ fprintf(stderr, "todo_add_get_tx_in_block called!\n"); abort(); }
/* Generated stub for todo_forget_about_block */
void todo_forget_about_block(struct state *state,
			     const struct protocol_block_id *block)
{ fprintf(stderr, "todo_forget_about_block called!\n"); abort(); }
/* Generated stub for wake_peers */
void wake_peers(struct state *state)
{ fprintf(stderr, "wake_peers called!\n"); abort(); }
/* AUTOGENERATED MOCKS END */

void send_tx_in_block_to_peers(struct state *state, const struct peer *exclude,
			       struct block *block, u16 shard, u8 txoff)
{
}

void save_block(struct state *state, struct block *new)
{
}

void save_tx(struct state *state, struct block *block, u16 shard, u8 txoff)
{
}

void seek_detached_blocks(struct state *state, 
			  const struct block *block)
{
}

void todo_add_get_shard(struct state *state,
			const struct protocol_block_id *block,
			u16 shardnum)
{
}

struct pending_block *new_pending_block(struct state *state)
{
	return talz(state, struct pending_block);
}

int main(int argc, char *argv[])
{
	struct state *s;
	struct working_block *w, *w2;
	unsigned int i;
	union protocol_tx *t;
	struct protocol_gateway_payment payment;
	struct block *b, *b2, *prev;
	struct block_shard *shard;
	struct protocol_input inputs[1];
	u8 *prev_txhashes;
	enum protocol_ecode e;
	struct gen_update update;
	struct protocol_input_ref *refs;
	struct protocol_block_id sha;
	struct protocol_block_id prevs[PROTOCOL_NUM_PREV_IDS];

	/* We need enough of state to use the real init function here. */
	pseudorand_init();
	s = new_state(true);

	fake_time = le32_to_cpu(genesis_tlr.timestamp) + 1;

	memset(prevs, 0, sizeof(prevs));
	prevs[0] = genesis.sha;

	/* Create a block with a gateway tx in it. */
	prev_txhashes = make_prev_txhashes(s, &genesis, helper_addr(1));
	w = new_working_block(s, 0x1ffffff0,
			      prev_txhashes, tal_count(prev_txhashes),
			      block_height(&genesis.bi) + 1,
			      next_shard_order(&genesis),
			      prevs, helper_addr(1));

	payment.send_amount = cpu_to_le32(1000);
	payment.output_addr = *helper_addr(0);
	t = create_from_gateway_tx(s, helper_gateway_public_key(),
				   1, &payment, false, helper_gateway_key(s));
	/* Gateway txs have empty refs, so this gives 0-len array. */
	refs = create_refs(s, &genesis, t, 1);

	update.shard = shard_of_tx(t, next_shard_order(&genesis));
	update.txoff = 0;
	update.features = 0;
	update.unused = 0;
	hash_tx_and_refs(t, refs, &update.hashes);
	assert(add_tx(w, &update));
	for (i = 0; !solve_block(w); i++);

	e = check_block_header(s, &w->bi, &prev, &sha.sha);
	assert(e == PROTOCOL_ECODE_NONE);
	assert(prev == &genesis);

	b = block_add(s, prev, &sha, &w->bi);

	/* This is a NOOP, so should succeed. */
	assert(check_prev_txhashes(s, b, NULL, NULL));

	/* Put the single tx into a shard. */
	shard = new_block_shard(s, update.shard, 1);
	b->shard[shard->shardnum] = shard;
	put_tx_in_shard(s, NULL, b, shard, 0, txptr_with_ref(shard, t, refs));

	/* This should all be correct. */
	check_block_shard(s, b, shard);
	assert(block_all_known(b));

	prev_txhashes = make_prev_txhashes(s, b, helper_addr(1));

	/* Solve third block, with a normal tx in it. */
	fake_time++;
	prevs[0] = b->sha;
	prevs[1] = genesis.sha;
	w2 = new_working_block(s, 0x1ffffff0,
			       prev_txhashes, num_prev_txhashes(b),
			       block_height(&b->bi) + 1,
			       next_shard_order(b),
			       prevs, helper_addr(1));

	/* We are going to spend half the gateway tx. */
	hash_tx(t, &inputs[0].input);
	inputs[0].output = 0;
	inputs[0].unused = 0;

	t = create_normal_tx(s, helper_addr(1),
			     500, 500 - PROTOCOL_FEE(500), 1, true, inputs,
			     helper_private_key(s, 0));
	assert(t->normal.change_amount == 500 - PROTOCOL_FEE(500));
	assert(num_inputs(t) == 1);

	/* This should create a reference back to the gateway tx */
	refs = create_refs(s, b, t, 1);
	assert(tal_count(refs) == num_inputs(t));
	assert(refs[0].blocks_ago == cpu_to_le32(1));
	assert(refs[0].shard == cpu_to_le16(update.shard));
	assert(refs[0].txoff == 0);
	assert(refs[0].unused == 0);

	update.shard = shard_of_tx(t, next_shard_order(b));
	update.txoff = 0;
	update.features = 0;
	update.unused = 0;
	hash_tx_and_refs(t, refs, &update.hashes);
	assert(add_tx(w2, &update));
	for (i = 0; !solve_block(w2); i++);

	e = check_block_header(s, &w2->bi, &prev, &sha.sha);
	assert(e == PROTOCOL_ECODE_NONE);
	assert(prev == b);

	b2 = block_add(s, prev, &sha, &w2->bi);

	/* This should be correct. */
	assert(check_prev_txhashes(s, b2, NULL, NULL));

	/* Put the single tx into a shard. */
	shard = new_block_shard(s, update.shard, 1);
	b2->shard[shard->shardnum] = shard;
	put_tx_in_shard(s, NULL, b2, shard, 0, txptr_with_ref(shard, t, refs));

	/* Should work */
	check_block_shard(s, b2, shard);

	b2->shard[shard->shardnum] = shard;
	assert(block_all_known(b2));

	tal_free(s);
	return 0;
}
