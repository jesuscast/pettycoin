/* Test we correctly switch to longer chains. */
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

#include "../pettycoin-generate.c"
#include "../timestamp.c"
#undef main
#undef time_now
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
#include "../prev_txhashes.c"
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
#include "../block_shard.c"
#include "../proof.c"
#include "../prev_blocks.c"
#include "easy_genesis.c"

/* AUTOGENERATED MOCKS START */
/* Generated stub for add_txhash_to_hashes */
void add_txhash_to_hashes(struct state *state,
			  const tal_t *ctx,
			  struct block *block, u16 shard, u8 txoff,
			  const struct protocol_tx_id *txhash)
{ fprintf(stderr, "add_txhash_to_hashes called!\n"); abort(); }
/* Generated stub for add_tx_to_hashes */
void add_tx_to_hashes(struct state *state,
		      const tal_t *ctx,
		      struct block *block, u16 shard, u8 txoff,
		      const union protocol_tx *tx)
{ fprintf(stderr, "add_tx_to_hashes called!\n"); abort(); }
/* Generated stub for block_expired_by */
bool block_expired_by(u32 expires, u32 now)
{ fprintf(stderr, "block_expired_by called!\n"); abort(); }
/* Generated stub for block_expiry */
u32 block_expiry(struct state *state, const struct block_info *bi)
{ fprintf(stderr, "block_expiry called!\n"); abort(); }
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
/* Generated stub for save_tx */
void save_tx(struct state *state, struct block *block, u16 shard, u8 txoff)
{ fprintf(stderr, "save_tx called!\n"); abort(); }
/* Generated stub for send_tx_in_block_to_peers */
void send_tx_in_block_to_peers(struct state *state, const struct peer *exclude,
			       struct block *block, u16 shard, u8 txoff)
{ fprintf(stderr, "send_tx_in_block_to_peers called!\n"); abort(); }
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
/* Generated stub for txhash_gettx_ancestor */
struct txhash_elem *txhash_gettx_ancestor(struct state *state,
					  const struct protocol_tx_id *sha,
					  const struct block *block)
{ fprintf(stderr, "txhash_gettx_ancestor called!\n"); abort(); }
/* Generated stub for upgrade_tx_in_hashes */
void upgrade_tx_in_hashes(struct state *state,
			  const struct protocol_tx_id *sha,
			  const union protocol_tx *tx)
{ fprintf(stderr, "upgrade_tx_in_hashes called!\n"); abort(); }
/* Generated stub for wake_peers */
void wake_peers(struct state *state)
{ fprintf(stderr, "wake_peers called!\n"); abort(); }
/* AUTOGENERATED MOCKS END */

void restart_generating(struct state *state)
{
}

void todo_add_get_shard(struct state *state,
			const struct protocol_block_id *block,
			u16 shardnum)
{
}

void save_block(struct state *state, struct block *new)
{
}

struct pending_block *new_pending_block(struct state *state)
{
	return talz(state, struct pending_block);

}

void logv(struct log *log, enum log_level level, const char *fmt, va_list ap)
{
}

void log_to_file(int fd, const struct log_record *lr)
{
}

void seek_detached_blocks(struct state *state, 
			  const struct block *block)
{
}

int main(int argc, char *argv[])
{
	struct state *s;
	struct working_block *w;
	u8 *prev_txhashes;
	unsigned int i, j;
	struct block *b[5], *b_alt[3], *prev, *prev2;
	enum protocol_ecode e;
	struct protocol_block_id sha;
	struct protocol_block_id prev_block_ids[PROTOCOL_NUM_PREV_IDS];

	pseudorand_init();
	s = new_state(true);
	fake_time = le32_to_cpu(genesis_tlr.timestamp) + 1;
	prev = &genesis;
		
	/* Generate chain of three blocks. */
	for (i = 0; i < 3; i++) {
		prev_txhashes = make_prev_txhashes(s, prev, helper_addr(1));
		make_prev_blocks(prev, prev_block_ids);
		w = new_working_block(s, 0x1ffffff0,
				      prev_txhashes, tal_count(prev_txhashes),
				      block_height(&prev->bi) + 1,
				      next_shard_order(prev),
				      prev_block_ids, helper_addr(1));
		for (j = 0; !solve_block(w); j++);
		fake_time++;
		e = check_block_header(s, &w->bi, &prev2, &sha.sha);
		assert(e == PROTOCOL_ECODE_NONE);
		assert(prev2 == prev);

		b[i] = block_add(s, prev, &sha, &w->bi);

		assert(tal_count(s->longest_chains) == 1);
		assert(s->longest_chains[0] == b[i]);
		assert(tal_count(s->longest_knowns) == 1);
		assert(s->longest_knowns[0] == b[i]);
		assert(s->preferred_chain == b[i]);
		prev = b[i];
	}

	/* Now generate an alternate chain of two blocks, from b[0]. */
	prev = b[0];
	for (i = 0; i < 2; i++) {
		prev_txhashes = make_prev_txhashes(s, prev, helper_addr(2));
		make_prev_blocks(prev, prev_block_ids);
		w = new_working_block(s, 0x1ffffff0,
				      prev_txhashes, tal_count(prev_txhashes),
				      block_height(&prev->bi) + 1,
				      next_shard_order(prev),
				      prev_block_ids, helper_addr(2));
		for (j = 0; !solve_block(w); j++);
		fake_time++;
		e = check_block_header(s, &w->bi, &prev2, &sha.sha);
		assert(e == PROTOCOL_ECODE_NONE);
		assert(prev2 == prev);
		b_alt[i] = block_add(s, prev, &sha, &w->bi);
		if (i == 0) {
			assert(tal_count(s->longest_chains) == 1);
			assert(tal_count(s->longest_knowns) == 1);
		} else {
			/* Second block brings us equal. */
			assert(tal_count(s->longest_chains) == 2);
			assert(tal_count(s->longest_knowns) == 2);
			assert(s->longest_chains[1] == b_alt[1]);
			assert(s->longest_knowns[1] == b_alt[1]);
		}
		assert(s->longest_chains[0] == b[2]);
		assert(s->longest_knowns[0] == b[2]);
		assert(s->preferred_chain == b[2]);
		prev = b_alt[i];
	}

	/* Now make alternate chain overtake first chain. */
	prev_txhashes = make_prev_txhashes(s, prev, helper_addr(2));
	make_prev_blocks(prev, prev_block_ids);
	w = new_working_block(s, 0x1ffffff0,
			      prev_txhashes, tal_count(prev_txhashes),
			      block_height(&prev->bi) + 1,
			      next_shard_order(prev),
			      prev_block_ids, helper_addr(2));
	for (j = 0; !solve_block(w); j++);
	fake_time++;
	e = check_block_header(s, &w->bi, &prev2, &sha.sha);
	assert(e == PROTOCOL_ECODE_NONE);
	assert(prev2 == prev);

	b_alt[2] = block_add(s, prev, &sha, &w->bi);

	assert(tal_count(s->longest_chains) == 1);
	assert(s->longest_chains[0] == b_alt[2]);
	assert(tal_count(s->longest_knowns) == 1);
	assert(s->longest_knowns[0] == b_alt[2]);
	assert(s->preferred_chain == b_alt[2]);

	/* Now make first chain equal again. */
	prev = b[2];
	prev_txhashes = make_prev_txhashes(s, prev, helper_addr(1));
	make_prev_blocks(prev, prev_block_ids);
	w = new_working_block(s, 0x1ffffff0,
			      prev_txhashes, tal_count(prev_txhashes),
			      block_height(&prev->bi) + 1,
			      next_shard_order(prev),
			      prev_block_ids, helper_addr(1));
	for (j = 0; !solve_block(w); j++);
	fake_time++;
	e = check_block_header(s, &w->bi, &prev2, &sha.sha);
	assert(e == PROTOCOL_ECODE_NONE);
	assert(prev2 == prev);

	b[3] = block_add(s, prev, &sha, &w->bi);

	assert(tal_count(s->longest_chains) == 2);
	assert(tal_count(s->longest_knowns) == 2);
	assert(s->longest_chains[0] == b_alt[2]);
	assert(s->longest_knowns[0] == b_alt[2]);
	assert(s->longest_chains[1] == b[3]);
	assert(s->longest_knowns[1] == b[3]);
	assert(s->preferred_chain == b_alt[2]);

	/* Now overtake. */
	prev = b[3];
	prev_txhashes = make_prev_txhashes(s, prev, helper_addr(1));
	make_prev_blocks(prev, prev_block_ids);
	w = new_working_block(s, 0x1ffffff0,
			      prev_txhashes, tal_count(prev_txhashes),
			      block_height(&prev->bi) + 1,
			      next_shard_order(prev),
			      prev_block_ids, helper_addr(1));
	for (j = 0; !solve_block(w); j++);
	fake_time++;
	e = check_block_header(s, &w->bi, &prev2, &sha.sha);
	assert(e == PROTOCOL_ECODE_NONE);
	assert(prev2 == prev);
	b[4] = block_add(s, prev, &sha, &w->bi);

	assert(tal_count(s->longest_chains) == 1);
	assert(s->longest_chains[0] == b[4]);
	assert(tal_count(s->longest_knowns) == 1);
	assert(s->longest_knowns[0] == b[4]);
	assert(s->preferred_chain == b[4]);

	tal_free(s);
	return 0;
}
