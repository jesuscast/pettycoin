#ifndef PETTYCOIN_PENDING_H
#define PETTYCOIN_PENDING_H
#include <ccan/short_types/short_types.h>
#include "protocol.h"

struct pending_trans {
	const union protocol_transaction *t;
	struct protocol_input_ref *refs; /* num_inputs(t) array */
};

/* aka state->pending */
struct pending_block {
	u8 *prev_merkles;
	/* Available for the next block. */
	struct pending_trans **pend;
};

struct state;
struct peer;
struct block;
struct protocol_transaction_gateway;

/* This block is no longer on favoured chain.  Get transactions out. */
void steal_pending_transactions(struct state *state,
				const struct block *old,
				const struct block *new);

/* Add a new transaction from peer to the current block. */
void add_pending_transaction(struct peer *peer,
			     const union protocol_transaction *t);

/* Get a new working block. */
struct pending_block *new_pending_block(struct state *state);

#endif /* PETTYCOIN_PENDING_H */
