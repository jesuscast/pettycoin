#include "addr.h"
#include "tx.h"
#include <ccan/structeq/structeq.h>

bool find_output(const union protocol_tx *tx, u16 output_num,
		 struct protocol_address *addr, u32 *amount)
{
	const struct protocol_gateway_payment *out;

	switch (tx_type(tx)) {
	case TX_FROM_GATEWAY:
		if (output_num > le16_to_cpu(tx->gateway.num_outputs))
			return false;
		out = get_gateway_outputs(&tx->gateway);
		*addr = out[output_num].output_addr;
		*amount = le32_to_cpu(out[output_num].send_amount);
		return true;
	case TX_NORMAL:
		if (output_num == 0) {
			/* Spending the send_amount. */
			*addr = tx->normal.output_addr;
			*amount = le32_to_cpu(tx->normal.send_amount);
			return true;
		} else if (output_num == 1) {
			/* Spending the change. */
			pubkey_to_addr(&tx->normal.input_key, addr);
			*amount = le32_to_cpu(tx->normal.change_amount);
			return true;
		}
		return false;
	}
	abort();
}

u32 find_matching_input(const union protocol_tx *tx,
			const struct protocol_input *inp)
{
	unsigned int i;

	/* Figure out which input of other did the spend. */
	for (i = 0; i < num_inputs(tx); i++) {
		if (structeq(&tx_input(tx, i)->input, &inp->input)
		    && tx_input(tx, i)->output == inp->output)
			return i;
	}
	abort();
}
