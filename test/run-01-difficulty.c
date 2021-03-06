#include "../difficulty.c"
#include "../minimal_log.c"
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include "easy_genesis.c"

static u32 exp_of(u32 diff)
{
	return diff >> 24;
}

static u32 mantissa_of(u32 diff)
{
	return diff & 0x00FFFFFF;
}

static struct block *add_fake_blocks(const tal_t *ctx,
				     struct block *prev,
				     u32 difficulty,
				     unsigned int num,
				     unsigned int spacing)
{
	unsigned int i;

	for (i = 0; i < num; i++) {
		struct block *b = tal(ctx, struct block);
		struct protocol_block_header *head;
		struct protocol_block_tailer *tail;

		b->bi.hdr = head = tal(b, struct protocol_block_header);
		head->height = cpu_to_le32(block_height(&prev->bi) + 1);

		b->bi.tailer = tail = tal(b, struct protocol_block_tailer);
		tail->timestamp
			= cpu_to_le32(block_timestamp(&prev->bi)
				      + spacing);
		tail->difficulty = cpu_to_le32(difficulty);
		b->prev = prev;
		prev = b;
	}
	return prev;
}

static void test_cmp(u32 diff1, u32 diff2)
{
	BIGNUM b1, b2;

	/* Make sure they're valid! */
	if (!(diff1 & 0x00FF0000))
		diff1 += 0x00010000;
	if (!(diff2 & 0x00FF0000))
		diff2 += 0x00010000;
	assert(valid_difficulty(diff1));
	assert(valid_difficulty(diff2));

	decode_difficulty(diff1, &b1);
	decode_difficulty(diff2, &b2);

	/* Note: smaller == harder, hence the inverse. */
	if (BN_cmp(&b1, &b2) != -difficulty_cmp(diff1, diff2)) {
		fprintf(stderr, "Incorrect difficulty_cmp(0x%08x, 0x%08x)",
			diff1, diff2);
		abort();
	}
}

int main(int argc, char *argv[])
{
	u32 diff1, diff2;
	struct state *state;
	struct block *b1, *b2;
	u32 exp, mantissa, tmp, i;
	BN_CTX *c = BN_CTX_new();

	/* Test encode_difficulty and decode_difficulty */
	for (exp = 4; exp < 20; exp++) {
		for (mantissa = 0x00010123;
		     mantissa < 0x01000000;
		     mantissa += 0x00010000) {
			BIGNUM b, res;
			u32 exp2, mantissa2;
			assert(decode_difficulty((exp << 24) | mantissa, &res));

			BN_init(&b);
			assert(BN_set_word(&b, mantissa));
			BN_lshift(&b, &b, (exp - 3) * 8);
			assert(BN_cmp(&b, &res) == 0);
			BN_free(&res);

			assert(encode_difficulty(&exp2, &mantissa2, &b));
			assert(exp2 == exp);
			assert(mantissa2 == mantissa);
			BN_free(&b);
		}
	}

	/* Test difficulty_one_sixteenth */
	for (exp = 4; exp < 20; exp++) {
		BIGNUM sixteen;

		BN_init(&sixteen);
		assert(BN_set_word(&sixteen, 16));

		/* make sure it's evenly divisible by 16. */
		for (mantissa = 0x00010120;
		     mantissa < 0x01000000;
		     mantissa += 0x00010000) {
			u32 diff = (exp << 24) | mantissa;
			BIGNUM b, b_times_16;

			assert(decode_difficulty(diff, &b));
			BN_init(&b_times_16);
			assert(BN_mul(&b_times_16, &b, &sixteen, c));
			BN_free(&b);

			assert(decode_difficulty(difficulty_one_sixteenth(diff),
						 &b));
			assert(BN_cmp(&b_times_16, &b) == 0);
			BN_free(&b);
			BN_free(&b_times_16);
		}
		BN_free(&sixteen);
	}

	/* Test difficulty_div4 */
	for (exp = 4; exp < 20; exp++) {
		BIGNUM four;

		BN_init(&four);
		assert(BN_set_word(&four, 4));

		/* make sure it's evenly divisible by 4. */
		for (mantissa = 0x00010200;
		     mantissa < 0x01000000;
		     mantissa += 0x00001000) {
			u32 diff = (exp << 24) | mantissa;
			BIGNUM b, b_times_4;

			assert(decode_difficulty(diff, &b));
			BN_init(&b_times_4);
			assert(BN_mul(&b_times_4, &b, &four, c));
			BN_free(&b);

			assert(decode_difficulty(difficulty_div4(diff),
						 &b));
			assert(BN_cmp(&b_times_4, &b) == 0);
			BN_free(&b);
			BN_free(&b_times_4);
		}
		BN_free(&four);
	}

	/* Test difficulty_cmp */
	test_cmp(0x10FFFFFF, 0x10FFFFFE);
	test_cmp(0x10FFFFFE, 0x10FFFFFF);
	test_cmp(0x10FFFFFF, 0x10FFFFFF);
	test_cmp(0x11FFFFFF, 0x10FFFFFF);
	srandom(time(NULL));
	for (i = 0; i < 1000; i++) {
		u32 exp1 = (random() % 27)+3, exp2 = (random() % 27)+3;
		test_cmp((exp1 << 24) | (random() & 0x00FFFFFF),
			 (exp2 << 24) | (random() & 0x00FFFFFF));
	}

	/* Set up state. */
	state = tal(NULL, struct state);
	state->block_height = tal_arr(state, struct list_head *, 1);
	state->block_height[0] = tal(state->block_height, struct list_head);
	state->test_net = true;
	list_head_init(state->block_height[0]);

	/* get_difficulty() can ask about genesis block. */
	BN_init(&genesis.total_work);
	if (!BN_zero(&genesis.total_work))
		errx(1, "Failed to initialize genesis block");
	list_head_init(&genesis.children);
	list_add_tail(state->block_height[0], &genesis.list);

	/* Difficulty immediately after genesis is the same */
	diff1 = get_difficulty(state, &genesis);
	assert(diff1 == block_difficulty(&genesis.bi));

	/* Try making it easier. */
	b1 = add_fake_blocks(state, &genesis, diff1,
			     PROTOCOL_DIFFICULTY_UPDATE_BLOCKS-1,
			     PROTOCOL_BLOCK_TARGET_TIME(true) * 2);

	diff1 = get_difficulty(state, b1);
	/* Won't change. */
	assert(diff1 == block_difficulty(&genesis.bi));
      
	/* Try making it faster. */
	b1 = add_fake_blocks(state, &genesis, diff1,
			     PROTOCOL_DIFFICULTY_UPDATE_BLOCKS-1,
			     PROTOCOL_BLOCK_TARGET_TIME(true) / 2);

	diff2 = get_difficulty(state, b1);
	/* Target should halve (but not quite due to out-by-one) */
	assert(exp_of(diff2) == exp_of(diff1));
	tmp = (u64)mantissa_of(diff2) * PROTOCOL_DIFFICULTY_UPDATE_BLOCKS
		/ (PROTOCOL_DIFFICULTY_UPDATE_BLOCKS - 1);
	/* Within 1. */
	assert(tmp == mantissa_of(diff1) / 2
	       || tmp == mantissa_of(diff1) / 2 + 1
	       || tmp == mantissa_of(diff1) / 2 - 1);

	/* Now try after genesis block (not out-by-one!). */
	diff1 = diff2;
	b2 = add_fake_blocks(state, b1, diff1,
			     PROTOCOL_DIFFICULTY_UPDATE_BLOCKS,
			     PROTOCOL_BLOCK_TARGET_TIME(state->test_net) / 2);

	diff2 = get_difficulty(state, b2);
	/* Target should halve. */
	assert(exp_of(diff2) == exp_of(diff1));
	assert(mantissa_of(diff2) == mantissa_of(diff1) / 2);

	/* Limit to 1/4. */
	b2 = add_fake_blocks(state, b1, diff1,
			     PROTOCOL_DIFFICULTY_UPDATE_BLOCKS,
			     PROTOCOL_BLOCK_TARGET_TIME(state->test_net) / 16);

	diff2 = get_difficulty(state, b2);
	/* Target should quarter. */
	assert(exp_of(diff2) == exp_of(diff1));
	assert(mantissa_of(diff2) == mantissa_of(diff1) / 4);

	/* Do it again, so we don't hit genesis limit */
	b1 = b2;
	diff1 = diff2;
	b2 = add_fake_blocks(state, b1, diff1,
			     PROTOCOL_DIFFICULTY_UPDATE_BLOCKS,
			     PROTOCOL_BLOCK_TARGET_TIME(state->test_net) / 16);

	diff2 = get_difficulty(state, b2);
	/* Target should quarter. */
	assert(exp_of(diff2) == exp_of(diff1));
	assert(mantissa_of(diff2) == mantissa_of(diff1) / 4);

	diff1 = diff2;

	/* Limit to * 4. */
	b2 = add_fake_blocks(state, b2, diff1,
			     PROTOCOL_DIFFICULTY_UPDATE_BLOCKS,
			     PROTOCOL_BLOCK_TARGET_TIME(state->test_net) * 10);

	diff2 = get_difficulty(state, b2);
	/* Target should multiply by 4. */
	assert(exp_of(diff2) == exp_of(diff1));
	assert(mantissa_of(diff2) == mantissa_of(diff1) * 4);

	/* FIXME: test total_work_done() */

	tal_free(state);
	BN_CTX_free(c);
	BN_free(&genesis.total_work);

	return 0;
}
