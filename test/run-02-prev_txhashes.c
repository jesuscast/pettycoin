#include "../chain.c"
#include "../state.c"
#include "../block.c"
#include "../pseudorand.c"
#include "../minimal_log.c"
#include "../difficulty.c"
#include "../block_shard.c"
#include "../prev_txhashes.c"
#include "../shadouble.c"
#include "easy_genesis.c"
#include <ccan/strmap/strmap.h>
#include <ccan/tal/str/str.h>

/* AUTOGENERATED MOCKS START */
/* Generated stub for block_to_pending */
void block_to_pending(struct state *state, const struct block *block)
{ fprintf(stderr, "block_to_pending called!\n"); abort(); }
/* Generated stub for check_prev_txhashes */
bool check_prev_txhashes(struct state *state, const struct block *block,
			 const struct block **bad_prev,
			 u16 *bad_shard)
{ fprintf(stderr, "check_prev_txhashes called!\n"); abort(); }
/* Generated stub for check_proof */
bool check_proof(const struct protocol_proof *proof,
		 const struct block *b,
		 const union protocol_tx *tx,
		 const struct protocol_input_ref *refs)
{ fprintf(stderr, "check_proof called!\n"); abort(); }
/* Generated stub for check_tx */
enum protocol_ecode check_tx(struct state *state, const union protocol_tx *tx,
			     const struct block *inside_block)
{ fprintf(stderr, "check_tx called!\n"); abort(); }
/* Generated stub for check_tx_inputs */
enum input_ecode check_tx_inputs(struct state *state,
				 const struct block *block,
				 const struct txhash_elem *me,
				 const union protocol_tx *tx,
				 unsigned int *bad_input_num)
{ fprintf(stderr, "check_tx_inputs called!\n"); abort(); }
/* Generated stub for complain_bad_prev_txhashes */
void complain_bad_prev_txhashes(struct state *state,
				struct block *block,
				const struct block *bad_prev,
				u16 bad_prev_shard)
{ fprintf(stderr, "complain_bad_prev_txhashes called!\n"); abort(); }
/* Generated stub for from_hex */
bool from_hex(const char *str, size_t slen, void *buf, size_t bufsize)
{ fprintf(stderr, "from_hex called!\n"); abort(); }
/* Generated stub for hash_tx */
void hash_tx(const union protocol_tx *tx, struct protocol_tx_id *txid)
{ fprintf(stderr, "hash_tx called!\n"); abort(); }
/* Generated stub for hash_tx_and_refs */
void hash_tx_and_refs(const union protocol_tx *tx,
		      const struct protocol_input_ref *refs,
		      struct protocol_txrefhash *txrefhash)
{ fprintf(stderr, "hash_tx_and_refs called!\n"); abort(); }
/* Generated stub for inputhash_hashfn */
size_t inputhash_hashfn(const struct inputhash_key *key)
{ fprintf(stderr, "inputhash_hashfn called!\n"); abort(); }
/* Generated stub for inputhash_keyof */
const struct inputhash_key *inputhash_keyof(const struct inputhash_elem *ie)
{ fprintf(stderr, "inputhash_keyof called!\n"); abort(); }
/* Generated stub for json_add_address */
void json_add_address(char **result, const char *fieldname, bool test_net,
		      const struct protocol_address *addr)
{ fprintf(stderr, "json_add_address called!\n"); abort(); }
/* Generated stub for json_add_block_id */
void json_add_block_id(char **result, const char *fieldname,
		       const struct protocol_block_id *id)
{ fprintf(stderr, "json_add_block_id called!\n"); abort(); }
/* Generated stub for json_add_double_sha */
void json_add_double_sha(char **result, const char *fieldname,
			 const struct protocol_double_sha *sha)
{ fprintf(stderr, "json_add_double_sha called!\n"); abort(); }
/* Generated stub for json_add_hex */
void json_add_hex(char **result, const char *fieldname, const void *data,
		  size_t len)
{ fprintf(stderr, "json_add_hex called!\n"); abort(); }
/* Generated stub for json_add_num */
void json_add_num(char **result, const char *fieldname, unsigned int value)
{ fprintf(stderr, "json_add_num called!\n"); abort(); }
/* Generated stub for json_add_tx_id */
void json_add_tx_id(char **result, const char *fieldname,
		    const struct protocol_tx_id *id)
{ fprintf(stderr, "json_add_tx_id called!\n"); abort(); }
/* Generated stub for json_array_end */
void json_array_end(char **ptr)
{ fprintf(stderr, "json_array_end called!\n"); abort(); }
/* Generated stub for json_array_start */
void json_array_start(char **ptr, const char *fieldname)
{ fprintf(stderr, "json_array_start called!\n"); abort(); }
/* Generated stub for json_get_params */
void json_get_params(const char *buffer, const jsmntok_t param[], ...)
{ fprintf(stderr, "json_get_params called!\n"); abort(); }
/* Generated stub for json_object_end */
void json_object_end(char **ptr)
{ fprintf(stderr, "json_object_end called!\n"); abort(); }
/* Generated stub for json_object_start */
void json_object_start(char **ptr, const char *fieldname)
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
/* Generated stub for make_prev_blocks */
void make_prev_blocks(const struct block *prev,
		      struct protocol_block_id prevs[PROTOCOL_NUM_PREV_IDS])
{ fprintf(stderr, "make_prev_blocks called!\n"); abort(); }
/* Generated stub for merkle_txs */
void merkle_txs(const struct block_shard *shard,
		struct protocol_double_sha *merkle)
{ fprintf(stderr, "merkle_txs called!\n"); abort(); }
/* Generated stub for todo_forget_about_block */
void todo_forget_about_block(struct state *state,
			     const struct protocol_block_id *block)
{ fprintf(stderr, "todo_forget_about_block called!\n"); abort(); }
/* Generated stub for wake_peers */
void wake_peers(struct state *state)
{ fprintf(stderr, "wake_peers called!\n"); abort(); }
/* AUTOGENERATED MOCKS END */

size_t marshal_tx_len(const union protocol_tx *tx)
{
	return sizeof(tx);
}

size_t marshal_input_ref_len(const union protocol_tx *tx)
{
	return 0;
}

void check_block(struct state *state, const struct block *block, bool all)
{
}

struct pending_block *new_pending_block(struct state *state)
{
	return talz(state, struct pending_block);
}

void save_block(struct state *state, struct block *new)
{
}

u8 pending_features(const struct block *block)
{
	return 0;
}

void seek_detached_blocks(struct state *state, 
			  const struct block *block)
{
}

struct strmap_block {
	STRMAP_MEMBERS(struct block *);
};
static struct strmap_block blockmap;

static struct block *add_next_block(struct state *state,
				    struct block *prev, const char *name,
				    unsigned int tx_count,
				    u8 shard_order,
				    const struct protocol_address *addr)
{
	struct block *b;
	struct block_info bi;
	struct protocol_block_header *hdr;
	struct protocol_block_tailer *tailer;
	u8 *num_txs;
	struct protocol_block_id dummy = { { { 0 } } };

	hdr = tal(state, struct protocol_block_header);
	hdr->shard_order = shard_order;
	hdr->height = cpu_to_le32(block_height(&prev->bi) + 1);
	hdr->prevs[0] = prev->sha;
	hdr->fees_to = *addr;

	tailer = tal(state, struct protocol_block_tailer);
	tailer->difficulty = cpu_to_le32(block_difficulty(&prev->bi));

	num_txs = tal_arrz(state, u8, 1 << hdr->shard_order);
	num_txs[0] = tx_count;

	memcpy(&dummy, name,
	       strlen(name) < sizeof(dummy) ? strlen(name) : sizeof(dummy));

	bi.hdr = hdr;
	bi.tailer = tailer;
	bi.num_txs = num_txs;
	b = block_add(state, prev, &dummy, &bi);

	strmap_add(&blockmap, name, b);
	return b;
}

static void create_chain(struct state *state, struct block *base,
			 const char *prefix,
			 const struct protocol_address *addr,
			 u8 shard_order,
			 unsigned int num, bool known)
{
	unsigned int i;

	for (i = 0; i < num; i++) {
		char *name = tal_fmt(state, "%s-%u", prefix, i);
		base = add_next_block(state, base, name, known ? 0 : 1,
				      shard_order, addr);
		known = true;
	}
}

int main(void)
{
	struct state *state;
	union protocol_tx *tx;
	struct block *b;
	u8 empty_prev_txhash, non_empty_prev_txhash, *prev_txhashes;
	struct protocol_address my_addr;
	size_t i;

	strmap_init(&blockmap);
	memset(&my_addr, 0, sizeof(my_addr));

	pseudorand_init();
	state = new_state(true);

	empty_prev_txhash = prev_txhash(&my_addr, &genesis, 0);

	/* genesis -> block1-0 ... block1-6. */
	create_chain(state, &genesis, "block1", &my_addr,
		     PROTOCOL_INITIAL_SHARD_ORDER, 7, true);

	prev_txhashes = make_prev_txhashes(state,
					   strmap_get(&blockmap, "block1-6"),
					   &my_addr);
	/* Should all be the same, empty_prev_txhash */
	for (i = 0; i < tal_count(prev_txhashes); i++)
		assert(prev_txhashes[i] == empty_prev_txhash);

	/* Now add one with a transaction
	   1   2   3   4   5   6   7   8  
	   1-0 1-1 1-2 1-3 1-4 1-5 1-6 T
	*/
	b = add_next_block(state, strmap_get(&blockmap, "block1-6"),
			   "blockT", 1, PROTOCOL_INITIAL_SHARD_ORDER,
			   &my_addr);

	/* Force a tx into the block. */
	tx = talz(state, union protocol_tx);

	b->shard[0]->u[0].txp.tx = tx;
	b->shard[0]->txcount++;

	non_empty_prev_txhash = prev_txhash(&my_addr, b, 0);

	prev_txhashes = make_prev_txhashes(state, b, &my_addr);
	/* This will be first byte of prev_txhashes */
	for (i = 0; i < tal_count(prev_txhashes); i++) {
		if (i == 0)
			assert(prev_txhashes[i] == non_empty_prev_txhash);
		else
			assert(prev_txhashes[i] == empty_prev_txhash);
	}
	assert(i > 0);
	/* Now, add another three blocks:
	   1   2   3   4   5   6   7   8   9   10  11
	   1-0 1-1 1-2 1-3 1-4 1-5 1-6 T   2-0 2-1 2-2
	*/
	create_chain(state, b, "block2", &my_addr, PROTOCOL_INITIAL_SHARD_ORDER,
		     3, true);
	b = strmap_get(&blockmap, "block2-2");

	prev_txhashes = make_prev_txhashes(state, b, &my_addr);
	for (i = 0; i < tal_count(prev_txhashes); i++) {
		/* First will be block2-2, then block2-1, then blockT */
		if (i == 2 << PROTOCOL_INITIAL_SHARD_ORDER)
			assert(prev_txhashes[i] == non_empty_prev_txhash);
		else
			assert(prev_txhashes[i] == empty_prev_txhash);
	}
	assert(i > (2 << PROTOCOL_INITIAL_SHARD_ORDER));

	/* Now, add another four blocks:
	   1   2   3   4   5   6   7   8   9   10  11  12  13  14  15
	   1-0 1-1 1-2 1-3 1-4 1-5 1-6 T   2-0 2-1 2-2 3-0 3-1 3-2 3-3
	*/
	create_chain(state, b, "block3", &my_addr,
		     PROTOCOL_INITIAL_SHARD_ORDER + 1,
		     4, true);
	b = strmap_get(&blockmap, "block3-3");
	prev_txhashes = make_prev_txhashes(state, b, &my_addr);
	for (i = 0; i < tal_count(prev_txhashes); i++) {
		/* First will be block3-3, then block3-2, then block 3-0,
		 * then block T. */
		if (i == (3 << b->bi.hdr->shard_order))
			assert(prev_txhashes[i] == non_empty_prev_txhash);
		else
			assert(prev_txhashes[i] == empty_prev_txhash);
	}
	assert(i > (3 << b->bi.hdr->shard_order));

	strmap_clear(&blockmap);
	tal_free(state);
	return 0;
}
/* Generated stub for main */
