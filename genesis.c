#include "genesis.h"
#include "protocol.h"

static struct protocol_block_header genesis_hdr = {
	.version = 1,
	.features_vote = 0,
	.nonce2 = { 0x53, 0x6f, 0x6d, 0x65, 0x20, 0x4e, 0x59, 0x54, 0x20, 0x48, 0x65, 0x61, 0x64, 0x21  },
	.fees_to = { { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  } }
};
static struct protocol_block_tailer genesis_tlr = {
	.timestamp = CPU_TO_LE32(1380540708),
	.difficulty = CPU_TO_LE32(0x1effffff),
	.nonce1 = CPU_TO_LE32(4860)
};
struct block genesis = {
	.hdr = &genesis_hdr,
	.tailer = &genesis_tlr,
	.sha = { { 0x73, 0xba, 0xcd, 0x26, 0x86, 0x8f, 0xb4, 0x9e, 0xa9, 0x29, 0xd1, 0x51, 0x9b, 0xa7, 0x9e, 0x22, 0xb1, 0x59, 0x15, 0xec, 0x57, 0x96, 0xfb, 0xca, 0x24, 0xb3, 0x4a, 0xf2, 0xf6, 0xbf, 0x00, 0x00  }}
};
