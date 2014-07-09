#include "block.h"
#include "block_shard.h"
#include "chain.h"
#include "check_block.h"
#include "check_tx.h"
#include "complain.h"
#include "difficulty.h"
#include "generating.h"
#include "hash_block.h"
#include "hash_tx.h"
#include "merkle_txs.h"
#include "overflows.h"
#include "prev_txhashes.h"
#include "protocol.h"
#include "protocol_net.h"
#include "shadouble.h"
#include "shard.h"
#include "state.h"
#include "timestamp.h"
#include "tx.h"
#include "tx_cmp.h"
#include "version.h"
#include <assert.h>
#include <ccan/array_size/array_size.h>
#include <ccan/structeq/structeq.h>
#include <ccan/tal/tal.h>
#include <stdlib.h>
#include <string.h>

static void destroy_block(struct block *b)
{
	BN_free(&b->total_work);
}

/* Returns error if bad.  Not sufficient by itself: see check_tx_order,
 * shard_validate_transactions and check_block_prev_txhashes! */
enum protocol_ecode
check_block_header(struct state *state,
		   const struct protocol_block_header *hdr,
		   const u8 *shard_nums,
		   const struct protocol_double_sha *merkles,
		   const u8 *prev_txhashes,
		   const struct protocol_block_tailer *tailer,
		   struct block **blockp,
		   struct protocol_double_sha *sha)
{
	struct block *block = (*blockp) = tal(state, struct block);
	enum protocol_ecode e;
	unsigned int i;

	/* Shouldn't happen, since we check in unmarshal. */
	if (!version_ok(hdr->version)) {
		e = PROTOCOL_ECODE_BLOCK_HIGH_VERSION;
		memset(sha, 0, sizeof(*sha));
		goto fail;
	}

	/* We check work *first*: if it meets its target we can spend
	 * resources on it, since it's not cheap to produce (eg. we could
	 * keep it around, or ask others about its predecessors, etc) */

	/* Get SHA: should have enough leading zeroes to beat target. */
	hash_block(hdr, shard_nums, merkles, prev_txhashes, tailer, &block->sha);
	if (sha)
		*sha = block->sha;

	if (!beats_target(&block->sha, le32_to_cpu(tailer->difficulty))) {
		e = PROTOCOL_ECODE_INSUFFICIENT_WORK;
		goto fail;
	}

	/* Don't just search on main chain! */
	block->prev = block_find_any(state, &hdr->prev_block);
	if (!block->prev) {
		e = PROTOCOL_ECODE_PRIV_UNKNOWN_PREV;
		goto fail;
	}

	if (hdr->shard_order != next_shard_order(block->prev)) {
		e = PROTOCOL_ECODE_BAD_SHARD_ORDER;
		goto fail;
	}

	if (le32_to_cpu(hdr->depth) != le32_to_cpu(block->prev->hdr->depth)+1) {
		e = PROTOCOL_ECODE_BAD_DEPTH;
		goto fail;
	}

	/* If there's something wrong with the previous block, us too. */
	block->complaint = block->prev->complaint;

	/* Can't go backwards, can't be more than 2 hours in future. */
	if (!check_timestamp(state, le32_to_cpu(tailer->timestamp),block->prev)){
		e = PROTOCOL_ECODE_BAD_TIMESTAMP;
		goto fail;
	}

	/* Based on previous blocks, how difficult should this be? */
	if (le32_to_cpu(tailer->difficulty)
	    != get_difficulty(state, block->prev)) {
		e = PROTOCOL_ECODE_BAD_DIFFICULTY;
		goto fail;
	}

	total_work_done(le32_to_cpu(tailer->difficulty),
			&block->prev->total_work,
			&block->total_work);

	block->shard = tal_arr(block, struct block_shard *, num_shards(hdr));
	for (i = 0; i < num_shards(hdr); i++)
		block->shard[i] = new_block_shard(block->shard, i,
						  shard_nums[i]);
	block->hdr = hdr;
	block->shard_nums = shard_nums;
	block->merkles = merkles;
	block->prev_txhashes = prev_txhashes;
	block->tailer = tailer;
	block->all_known = false;
	list_head_init(&block->children);

	tal_add_destructor(block, destroy_block);

	return PROTOCOL_ECODE_NONE;

fail:
	*blockp = tal_free(block);
	return e;
}

bool shard_belongs_in_block(const struct block *block,
			    const struct block_shard *shard)
{
	struct protocol_double_sha merkle;

	/* merkle_txs is happy with just the hashes. */
	assert(shard->txcount + shard->hashcount
	       == block->shard_nums[shard->shardnum]);
	merkle_txs(shard, &merkle);
	return structeq(&block->merkles[shard->shardnum], &merkle);
}

static void add_tx_to_txhash(struct state *state,
			     struct block *block,
			     struct block_shard *shard,
			     u8 txoff)
{
	struct txhash_elem *te;
	struct protocol_double_sha sha;
	struct txhash_iter iter;

	hash_tx(tx_for(shard, txoff), &sha);

	/* It could already be there (alternate chain, or previous
	 * partial shard which we just overwrote). */
	for (te = txhash_firstval(&state->txhash, &sha, &iter);
	     te;
	     te = txhash_nextval(&state->txhash, &sha, &iter)) {
		/* Previous partial shard which we just overwrote? */
		if (te->block == block
		    && te->shardnum == shard->shardnum
		    && te->txoff == txoff)
			return;
	}

	/* Add a new one for this block. */
	te = tal(shard, struct txhash_elem);
	te->block = block;
	te->shardnum = shard->shardnum;
	te->txoff = txoff;
	te->sha = sha;
	txhash_add(&state->txhash, te);
	/* FIXME:
	   tal_add_destructor(te, delete_from_txhash);
	*/
}

static struct txptr_with_ref dup_txp(const tal_t *ctx,
				     const struct txptr_with_ref txp)
{
	struct txptr_with_ref ret;
	size_t len;

	len = marshal_tx_len(txp.tx)
		+ num_inputs(txp.tx) * sizeof(struct protocol_input_ref);

	ret.tx = (void *)tal_dup(ctx, char, (char *)txp.tx, len, 0);
	return ret;
}

static void copy_old_txs(struct state *state,
			 struct block *block,
			 struct block_shard *new, const struct block_shard *old)
{
	unsigned int i;

	for (i = 0; i < new->size; i++) {
		if (!shard_is_tx(old, i)) {
			/* Both hashes must be identical. */
			assert(structeq(&old->u[i].hash, &new->u[i].hash));
			continue;
		}
		if (!tx_for(old, i))
			continue;

		/* We don't need to check_tx_ordering, since they were already
		 * checked. */

		/* It's probably not a talloc pointer, so copy! */
		put_tx_in_shard(state, block, new, i,
				dup_txp(new, old->u[i].txp));

		/* We don't need to copy proofs, since we have full shard. */
	}
}

void put_shard_of_hashes_into_block(struct state *state,
				    struct block *block,
				    struct block_shard *shard)
{
	unsigned int num;

	assert(shard_belongs_in_block(block, shard));
	assert(shard->txcount == 0);
	assert(shard->hashcount == num);

	/* If we know some transactions already, perform a merge. */
	if (block->shard[shard->shardnum]) {
		copy_old_txs(state, block, shard,
			     block->shard[shard->shardnum]);
		tal_free(block->shard[shard->shardnum]);
	}

	block->shard[shard->shardnum] = tal_steal(block, shard);
}

/* If we were to insert tx in block->shard[shardnum] at txoff, would it be
 * in order? */
bool check_tx_ordering(struct state *state,
		       struct block *block,
		       struct block_shard *shard, u8 txoff,
		       const union protocol_tx *tx,
		       u8 *bad_txoff)
{
	const union protocol_tx *other_tx;
	int i;

	/* Don't bother on empty shard. */
	if (shard->txcount == 0)
		return true;

	/* Check ordering against previous. */
	for (i = (int)txoff-1; i >= 0; i--) {
		other_tx = tx_for(shard, i);
		if (other_tx) {
			if (tx_cmp(other_tx, tx) >= 0) {
				*bad_txoff = i;
				return false;
			}
			break;
		}
	}

	/* Check ordering against next. */
	for (i = (int)txoff+1; i < shard->size; i++) {
		other_tx = tx_for(shard, i);
		if (other_tx) {
			if (tx_cmp(tx, other_tx) >= 0) {
				*bad_txoff = i;
				return false;
			}
			break;
		}
	}
	return true;
}

void put_tx_in_shard(struct state *state,
		     struct block *block,
		     struct block_shard *shard, u8 txoff,
		     struct txptr_with_ref txp)
{
	/* All this work for assertion checking! */
	if (shard_is_tx(shard, txoff)) {
		if (tx_for(shard, txoff)) {
			assert(memcmp(txp.tx, tx_for(shard, txoff),
				      marshal_tx_len(txp.tx)
				      + marshal_input_ref_len(txp.tx)) == 0);
			shard->txcount--;
		}
	} else {
		/* Tx must match hash. */
		struct protocol_net_txrefhash hashes;
		hash_tx_and_refs(txp.tx, refs_for(txp), &hashes);
		assert(structeq(shard->u[txoff].hash, &hashes));
		shard->hashcount--;
	}

	/* Now it's a transaction. */
	bitmap_clear_bit(shard->txp_or_hash, txoff);
	shard->u[txoff].txp = txp;
	shard->txcount++;

	/* Record it in the hash. */
	add_tx_to_txhash(state, block, shard, txoff);

	/* If we've just filled it, we don't need proofs any more. */
	if (shard_all_hashes(shard))
		shard->proof = tal_free(shard->proof);

}

void put_proof_in_shard(struct state *state,
			struct block *block,
			struct block_shard *shard, u8 txoff,
			const struct protocol_proof *proof)
{
	/* If we have all hashes, we don't need to keep proof. */
	if (shard_all_hashes(shard))
		return;

	if (!shard->proof)
		shard->proof = tal_arrz(shard, struct protocol_proof *,
					block->shard_nums[shard->shardnum]);

	if (shard->proof[txoff])
		return;

	shard->proof[txoff] = tal_dup(shard, struct protocol_proof, proof, 1,0);
}

/* Check what we can, using block->prev->...'s shards. */
bool check_block_prev_txhashes(struct state *state, const struct block *block)
{
	unsigned int i;
	size_t off = 0;
	const struct block *prev;

	for (i = 0, prev = block->prev;
	     i < PROTOCOL_PREV_BLOCK_TXHASHES && prev;
	     i++, prev = prev->prev) {
		unsigned int j;

		/* It's bad if we don't have that many prev merkles. */
		if (off + num_shards(prev->hdr)
		    > le32_to_cpu(block->hdr->num_prev_txhashes))
			return false;

		for (j = 0; j < num_shards(prev->hdr); j++) {
			u8 prev_txh;

			/* We need to know everything in shard to check
			 * previous merkle. */
			if (!shard_all_known(prev->shard[j]))
				continue;

			prev_txh = prev_txhash(&block->hdr->fees_to, prev, j);

			/* We only check one byte; that's enough. */
			if (prev_txh != block->prev_txhashes[off+j]) {
				log_unusual(state->log,
					    "Incorrect merkle for block %u:"
					    " block %u shard %u was %u not %u",
					    le32_to_cpu(block->hdr->depth),
					    le32_to_cpu(block->hdr->depth) - i,
					    j,
					    prev_txh,
					    block->prev_txhashes[off+j]);
				return false;
			}
		}
		off += j;
	}

	/* Must have exactly the right number of previous merkle hashes. */
	return off == le32_to_cpu(block->hdr->num_prev_txhashes);
}

void check_block(struct state *state, const struct block *block)
{
	u32 diff = le32_to_cpu(block->tailer->difficulty);
	struct protocol_double_sha sha;
	unsigned int shard;

	if (block != genesis_block(state)) {
		assert(beats_target(&block->sha, diff));
		assert(tal_count(block->shard) == num_shards(block->hdr));
	}
	hash_block(block->hdr, block->shard_nums, block->merkles,
		   block->prev_txhashes, block->tailer, &sha);
	assert(structeq(&sha, &block->sha));

	if (block->prev) {
		if (block->all_known)
			assert(block->prev->all_known);

		if (block->prev->complaint)
			assert(block->complaint);

	}

	/* FIXME: check block->prev_txhashes! */

	for (shard = 0; shard < num_shards(block->hdr); shard++) {
		check_block_shard(state, block, block->shard[shard]);
	}
}
