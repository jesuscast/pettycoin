#ifndef PETTYCOIN_INPUTHASH_H
#define PETTYCOIN_INPUTHASH_H
#include "config.h"
#include "protocol.h"
#include <ccan/htable/htable_type.h>
#include <ccan/tal/tal.h>

struct inputhash_key {
	struct protocol_tx_id tx;
	u16 output_num;
};

/* Every transaction input is in this hash, indexed by tx. */
struct inputhash_elem {
	struct inputhash_key output;

	/* ...was used by this transaction as an input. */
	struct protocol_tx_id used_by;
};

const struct inputhash_key *inputhash_keyof(const struct inputhash_elem *ie);
size_t inputhash_hashfn(const struct inputhash_key *key);
bool inputhash_eq(const struct inputhash_elem *elem,
		  const struct inputhash_key *output);

HTABLE_DEFINE_TYPE(struct inputhash_elem,
		   inputhash_keyof, inputhash_hashfn, inputhash_eq, inputhash);

/* Since an input can be used by multiple transactions (different chains)... */
struct inputhash_elem *inputhash_firstval(struct inputhash *inputhash,
					  const struct protocol_tx_id *tx,
					  u16 output_num,
					  struct inputhash_iter *i);
struct inputhash_elem *inputhash_nextval(struct inputhash *inputhash,
					 const struct protocol_tx_id *tx,
					 u16 output_num,
					 struct inputhash_iter *i);

struct state;
void inputhash_add_tx(struct state *state,
		      struct inputhash *inputhash, const union protocol_tx *tx);

void inputhash_del_tx(struct inputhash *inputhash, const union protocol_tx *tx);
#endif /* PETTYCOIN_INPUTHASH_H */
