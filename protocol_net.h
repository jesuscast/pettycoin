#ifndef PETTYCOIN_PROTOCOL_NET_H
#define PETTYCOIN_PROTOCOL_NET_H
#include "protocol.h"

#define PROTOCOL_MAX_PACKET_LEN (4 * 1024 * 1024)

/* Request-response protocol for pettycoin. */
enum protocol_req_type {
	/* Invalid. */
	PROTOCOL_REQ_NONE,
	/* Hi, my version is, and my hobbies are... */
	PROTOCOL_REQ_WELCOME,
	/* Your last response didn't make sense. */
	PROTOCOL_REQ_ERR,
	/* I have a new block! */
	PROTOCOL_REQ_NEW_BLOCK,
	/* I have a new transaction */
	PROTOCOL_REQ_NEW_TRANSACTION,
	/* Tell me about this batch in a block. */
	PROTOCOL_REQ_BATCH,
	/* This transaction has a bad input. */
	PROTOCOL_REQ_BAD_TRANS_INPUT,
	/* This transaction has bad inputs (total is wrong). */
	PROTOCOL_REQ_BAD_TRANS_AMOUNT,
	/* I have found a flaw in this transaction in block. */
	PROTOCOL_REQ_BLOCK_TRANS_INVALID,
	/* PROTOCOL_REQ_BAD_TRANS_INPUT (within block). */
	PROTOCOL_REQ_BLOCK_BAD_TRANS_INPUT,
	/* PROTOCOL_REQ_BAD_TRANS_AMOUNT (within block). */
	PROTOCOL_REQ_BLOCK_BAD_TRANS_AMOUNT,
	/* Two transactions in block are out of order. */
	PROTOCOL_REQ_BLOCK_TRANS_MISORDER,
	/* A reference is to the wrong transaction. */
	PROTOCOL_REQ_BLOCK_BAD_INPUT_REF_TRANS,

	/* Tell me about this block. */
	PROTOCOL_REQ_TRANSACTION_NUMS,
	/* Tell me about this transaction in a block. */
	PROTOCOL_REQ_TRANSACTION,

	/* >= this is invalid. */
	PROTOCOL_REQ_MAX
};

/* High bit indicates a response packet. */
enum protocol_resp_type {
	/* Invalid. */
	PROTOCOL_RESP_NONE = 0x80000000,
	PROTOCOL_RESP_WELCOME,
	PROTOCOL_RESP_ERR,
	PROTOCOL_RESP_NEW_BLOCK,
	PROTOCOL_RESP_NEW_TRANSACTION,
	PROTOCOL_RESP_BATCH,
	PROTOCOL_RESP_BAD_TRANS_INPUT,
	PROTOCOL_RESP_BAD_TRANS_AMOUNT,
	PROTOCOL_RESP_BLOCK_TRANS_INVALID,
	PROTOCOL_RESP_BLOCK_BAD_TRANS_INPUT,
	PROTOCOL_RESP_BLOCK_BAD_TRANS_AMOUNT,
	PROTOCOL_RESP_BLOCK_TRANS_MISORDER,

	PROTOCOL_RESP_TRANSACTION_NUMS,
	PROTOCOL_RESP_TRANSACTION,

	/* >= this is invalid. */
	PROTOCOL_RESP_MAX
};

enum protocol_error {
	PROTOCOL_ERROR_NONE, /* happy camper. */
	/* General errors: */
	PROTOCOL_UNKNOWN_COMMAND,
	PROTOCOL_INVALID_LEN,
	PROTOCOL_SHOULD_BE_WAITING,
	PROTOCOL_INVALID_RESPONSE,

	/* protocol_req_welcome: */
	PROTOCOL_ERROR_HIGH_VERSION, /* version is unknown. */
	PROTOCOL_ERROR_LOW_VERSION, /* version is old. */
	PROTOCOL_ERROR_NO_INTEREST, /* not enough interest bits. */
	PROTOCOL_ERROR_WRONG_GENESIS, /* disagree over genesis block. */

	/* protocol_req_new_block: */
	PROTOCOL_ERROR_BLOCK_HIGH_VERSION, /* block version unknown. */
	PROTOCOL_ERROR_BLOCK_LOW_VERSION, /* block version is old. */
	PROTOCOL_ERROR_UNKNOWN_PREV, /* I don't know previous block. */
	PROTOCOL_ERROR_BAD_TIMESTAMP, /* Too far in future or past. */
	PROTOCOL_ERROR_BAD_PREV_MERKLES, /* Wrong number of prev_merkles. */
	PROTOCOL_ERROR_BAD_DIFFICULTY, /* Wrong difficulty calculation. */
	PROTOCOL_ERROR_INSUFFICIENT_WORK, /* Didn't meet difficulty. */

	/* protocol_req_new_transaction: */
	PROTOCOL_ERROR_TRANS_HIGH_VERSION, /* transaction version unknown */
	PROTOCOL_ERROR_TRANS_LOW_VERSION, /* transaction version old */
	PROTOCOL_ERROR_TRANS_UNKNOWN, /* unknown transaction type */
	PROTOCOL_ERROR_TRANS_BAD_GATEWAY, /* unknown gateway */
	PROTOCOL_ERROR_TRANS_CROSS_SHARDS, /* to different shards. */
	PROTOCOL_ERROR_TOO_LARGE, /* too many satoshi in one transaction. */
	PROTOCOL_ERROR_TRANS_BAD_SIG, /* invalid signature */
	PROTOCOL_ERROR_TOO_MANY_INPUTS, /* > TRANSACTION_MAX_INPUTS. */
	PROTOCOL_ERROR_TRANS_BAD_INPUT, /* an input is bad. */
	PROTOCOL_ERROR_TRANS_BAD_AMOUNTS, /* total inputs != outputs  */
	/* These two only occur within a batch: */
	PROTOCOL_ERROR_BATCH_BAD_INPUT_REF, /* input_ref is bad */
	PROTOCOL_ERROR_BATCH_BAD_INPUT_REF_TRANS, /* input_ref points to bad trans */

	/* protocol_req_batch: */
	PROTOCOL_ERROR_UNKNOWN_BLOCK, /* I don't know that block? */
	PROTOCOL_ERROR_BAD_BATCHNUM, /* Exceeds transaction count. */
	PROTOCOL_ERROR_UNKNOWN_BATCH, /* It exists, but I don't know it. */

	/* protocol_resp_batch: */
	PROTOCOL_ERROR_DISAGREE_BATCHNUM, /* Disagree with BAD_BATCHNUM */
	PROTOCOL_ERROR_DISAGREE_BATCHSIZE, /* Disagree with num in batch. */

	/* >= this is invalid. */
	PROTOCOL_ERROR_MAX
};

/* Every packet starts with these two. */
struct protocol_net_hdr {
	le32 len; /* size including header */
	le32 type; /* PROTOCOL_REQ_* or PROTOCOL_RESP_* */
};

struct protocol_net_address {
	u8 addr[16];
	be16 port;
}  __attribute__((aligned(2)));

struct protocol_req_welcome {
	le32 len; /* sizeof(struct protocol_req_welcome) */
	le32 type; /* PROTOCOL_REQ_WELCOME */
	le32 version; /* Protocol version, currently 1. */
	/* Freeform software version. */
	char moniker[28];
	/* Self-detection */
	le64 random;
	le32 num_blocks;
	/* Address we see you at. */
	struct protocol_net_address you;
	/* Port you can connect to us at (if != 0) */
	be16 listen_port;
	/* We are interested in certain addresses, based on their
	 * lower bits.  We must be interested in more than 1. */
	u8 interests[(1 << PROTOCOL_SHARD_BITS) / 8];
	/* As per bitcoin: last 10 blocks, then power of 2 back. */
	struct protocol_double_sha block[ /* num_blocks */ ];
};

/* Usually followed by a hangup if error, since communication has failed. */
struct protocol_req_err {
	le32 len; /* sizeof(struct protocol_req_err) */
	le32 type; /* PROTOCOL_REQ_ERR */
	le32 error;
};

/* Usually followed by a hangup if error, since communication has failed. */
struct protocol_resp_err {
	le32 len; /* sizeof(struct protocol_resp_err) */
	le32 type; /* PROTOCOL_RESP_ERR */
	le32 error;
};

/* I have a new block for you! */
struct protocol_req_new_block {
	le32 len; /* sizeof(struct protocol_req_new_block) */
	le32 type; /* PROTOCOL_REQ_NEW_BLOCK */

	/* Marshalled block. */
	char block[];
};

struct protocol_resp_new_block {
	le32 len; /* sizeof(struct protocol_resp_new_block) */
	le32 type; /* PROTOCOL_RESP_NEW_BLOCK */

	/* last block we know. */
	struct protocol_double_sha final;
};

/* I have a new transaction! */
struct protocol_req_new_transaction {
	le32 len; /* ... */
	le32 type; /* PROTOCOL_REQ_NEW_TRANSACTION */

	/* union protocol_transaction trans ...; */
};

struct protocol_resp_new_transaction {
	le32 len; /* sizeof(struct protocol_resp_new_transaction) */
	le32 type; /* PROTOCOL_RESP_NEW_TRANSACTION */
	le32 error; /* Expect PROTOCOL_ERROR_NONE. */
};

/* Give me this batch. */
struct protocol_req_batch {
	le32 len; /* sizeof(struct protocol_req_batch) */
	le32 type; /* PROTOCOL_REQ_BATCH */
	/* Which block do I want to know about. */
	struct protocol_double_sha block;
	/* Which batch number. */
	le32 batchnum;
};

/* Here's a batch for you. */
struct protocol_resp_batch {
	le32 len; /* sizeof(struct protocol_resp_batch + ...) */
	le32 type; /* PROTOCOL_RESP_WELCOME */
	le32 error; /* Expect PROTOCOL_ERROR_NONE. */
	le32 num; /* Number of transactions in batch. */

	/* {
		union protocol_transaction trans;
		struct protocol_input_ref backref[num_inputs(trans)];
	   }[num]
	*/
};

/* Which transactions are interesting to me? */
struct protocol_req_batch_nums {
	le32 len; /* sizeof(struct protocol_req_batchnums) */
	le32 type; /* PROTOCOL_REQ_TRANSACTION_NUMS */
	/* Which block do I want to know about. */
	struct protocol_double_sha block;
};

/* Here's some transaction nums for you. */
struct protocol_resp_transaction_nums {
	le32 len; /* sizeof(struct protocol_resp_welcome) */
	le32 type; /* PROTOCOL_RESP_TRANSACTION_NUMS */
	le32 error; /* Expect PROTOCOL_ERROR_NONE. */
	le32 num_transactions; /* Number of individual transactions. */
	le32 idx[ /* num_transactions */ ];
};

/* Give me this transaction. */
struct protocol_req_transaction {
	le32 len; /* sizeof(struct protocol_req_transaction) */
	le32 type; /* PROTOCOL_REQ_TRANSACTION */
	/* Which block do I want to know about. */
	struct protocol_double_sha block;
	/* Which transaction */
	le32 transnum;
};

/* Here's a transaction for you. */
struct protocol_resp_transaction {
	le32 len; /* sizeof(struct protocol_resp_batch + ...) */
	le32 type; /* PROTOCOL_RESP_WELCOME */
	le32 error; /* Expect PROTOCOL_ERROR_NONE. */

	/* This is the tree of double shas which proves it. */
	struct protocol_double_sha proof[PETTYCOIN_BATCH_ORDER];

	/* union protocol_transaction trans ...; */
};

struct protocol_req_bad_trans_input {
	le32 len; /* sizeof(struct protocol_req_bad_trans_input) */
	le32 type; /* PROTOCOL_REQ_BAD_TRANS_INPUT */

	/* The input we're complaining about. */
	le32 inputnum;

	/* The transaction whose input was bad:
	     union protocol_transaction trans ...; 
	   The bad input:
	     union protocol_transaction input ...;
	*/
};

struct protocol_resp_bad_trans_input {
	le32 len; /* sizeof(struct protocol_resp_bad_trans_input) */
	le32 type; /* PROTOCOL_RESP_BAD_TRANS_INPUT */
	le32 error; /* Expect PROTOCOL_ERROR_NONE. */
};

struct protocol_req_bad_trans_amount {
	le32 len; /* sizeof(struct protocol_req_bad_trans_amount) */
	le32 type; /* PROTOCOL_REQ_BAD_TRANS_AMOUNT */

	/* The transaction whose inputs were bad:
	     union protocol_transaction t ...;
	   The inputs:
	     union protocol_transaction input[t->normal.num_inputs] ...; */
};

struct protocol_resp_bad_trans_amount {
	le32 len; /* sizeof(struct protocol_resp_bad_trans_amount) */
	le32 type; /* PROTOCOL_RESP_BAD_TRANS_AMOUNT */
	le32 error; /* Expect PROTOCOL_ERROR_NONE. */
};

/* IPv4 addresses are represented as per rfc4291#section-2.5.5.2 */
struct protocol_net_addr {
	u8 addr[16];
};

/* This block contains an invalid transaction. */
struct protocol_req_block_trans_invalid {
	le32 len; /* sizeof(struct protocol_req_block_trans_invalid) */
	le32 type; /* PROTOCOL_REQ_BLOCK_TRANS_INVALID */

	/* Which block I am referring to. */
	struct protocol_double_sha block;

	/* What is wrong with it (as per protocol_req_new_transaction).
	 * One of:
	 *  PROTOCOL_ERROR_TRANS_HIGH_VERSION
	 *  PROTOCOL_ERROR_TRANS_LOW_VERSION
	 *  PROTOCOL_ERROR_TRANS_UNKNOWN
	 *  PROTOCOL_ERROR_TRANS_BAD_GATEWAY
	 *  PROTOCOL_ERROR_TRANS_CROSS_SHARDS
	 *  PROTOCOL_ERROR_TOO_LARGE
	 *  PROTOCOL_ERROR_TRANS_BAD_SIG
	 *  PROTOCOL_ERROR_TOO_MANY_INPUTS
	 *  PROTOCOL_ERROR_BATCH_BAD_INPUT_REF
	 */
	le32 error;

	struct protocol_proof proof;

	/* union protocol_transaction trans;
	   struct protocol_input_ref ref[num_inputs(trans)];
	*/
};

/* This block contains an transaction with an invalid input,
 * ie PROTOCOL_ERROR_TRANS_BAD_INPUT. */
struct protocol_req_block_bad_trans_input {
	le32 len; /* sizeof(struct protocol_req_block_bad_trans_input) */
	le32 type; /* PROTOCOL_REQ_BLOCK_BAD_TRANS_INPUT */

	/* Which block & input I am referring to. */
	struct protocol_double_sha block;
	le32 inputnum;

	struct protocol_proof proof;

	/* The transaction whose input was bad:
	     union protocol_transaction trans;
	     struct protocol_input_ref ref[num_inputs(trans)];
	   The bad input:
	     union protocol_transaction input;
	*/
};

/* This block contains an input ref with an invalid input (wrong trans!)
 * ie PROTOCOL_ERROR_BATCH_BAD_REF_TRANS. */
struct protocol_req_block_bad_input_ref_trans {
	le32 len; /* sizeof(struct protocol_req_block_bad_input_ref_trans) */
	le32 type; /* PROTOCOL_REQ_BLOCK_BAD_INPUT_REF_TRANS */

	/* Which block & input I am referring to. */
	struct protocol_double_sha block;
	le32 inputnum;

	/* Proof of the inputs & trans being in inputnum */
	struct protocol_proof proof;

	/* Proof that bad_input is where ref points. */
	struct protocol_proof input_proof;

	/* The transaction whose input was bad:
	     union protocol_transaction trans;
	     struct protocol_input_ref ref[num_inputs(trans)];
	     union protocol_transaction bad_input;
	     struct protocol_input_ref bad_inputref[num_inputs(bad_input)];
	*/
};

/* This block contains an transaction with an invalid total,
 * ie PROTOCOL_ERROR_TRANS_BAD_AMOUNTS. */
struct protocol_req_block_bad_trans_amount {
	le32 len; /* sizeof(struct protocol_req_block_bad_trans_amount) */
	le32 type; /* PROTOCOL_REQ_BLOCK_BAD_TRANS_AMOUNT */

	/* Which block & transaction I am referring to. */
	struct protocol_double_sha block;

	struct protocol_proof proof;

	/* The transaction whose inputs were bad:
	     union protocol_transaction t;
	     struct protocol_input_ref ref[num_inputs(trans)];
	   The inputs:
	     union protocol_transaction input[t->normal.num_inputs]; */
};

/* This block contains out-of-order transaction. */
struct protocol_req_block_trans_misorder {
	le32 len; /* sizeof(struct protocol_req_block_trans_misorder) */
	le32 type; /* PROTOCOL_REQ_BLOCK_TRANS_MISORDER */

	/* Which block & transactions I am referring to. */
	struct protocol_double_sha block;

	struct protocol_proof proof1;
	struct protocol_proof proof2;

	/* union protocol_transaction trans1;
	   struct protocol_input_ref ref1[num_inputs(trans1)];
	   union protocol_transaction trans2;
	   struct protocol_input_ref ref2[num_inputs(trans2)];
	*/
};

#endif /* PETTYCOIN_PROTOCOL_NET_H */
