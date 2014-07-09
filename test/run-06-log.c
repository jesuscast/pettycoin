#include <ccan/tal/tal.h>
#include <ccan/tal/str/str.h>
#include <ccan/time/time.h>
#include <ccan/isaac/isaac64.h>
#include <ccan/err/err.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
static struct timeabs my_time;

/* Prune everything! */
#define isaac64_next_uint(isaac64, n) ((n) - 1)
#define time_now() my_time
#include "../log.c"
#include "../log_helper.c"
#include "../base58.c"
#include "../marshal.c"
#include "../shadouble.c"

void hash_tx(const union protocol_tx *tx,
	     struct protocol_double_sha *sha)
{
	abort();
}

static char *read_from(const tal_t *ctx, int fd)
{
	size_t max = 128, done = 0;
	int r;
	char *p = tal_arr(ctx, char, max);

	while ((r = read(fd, p + done, max - done)) > 0) {
		done += r;
		if (done == max)
			tal_resize(&p, max *= 2);
	}
	tal_resize(&p, done + 1);
	p[done] = '\0';

	return p;
}

int main(void)
{
	struct protocol_double_sha dsha;
	struct protocol_net_address netaddr;
	int fds[2];
	char *p, *mem1, *mem2, *mem3;
	int status;
	size_t maxmem = sizeof(struct log_entry) * 4 + 25 + 25 + 28 + 144;
	void *ctx = tal(NULL, char);
	struct log *log = new_log(ctx, NULL, "PREFIX", LOG_BROKEN+1, maxmem);

	assert(tal_parent(log) == ctx);
	my_time.ts.tv_sec = 1384064855;
	my_time.ts.tv_nsec = 500;

	log_debug(log, "This is a debug %s!", "message");
	my_time.ts.tv_nsec++;
	log_info(log, "This is an info %s!", "message");
	my_time.ts.tv_nsec++;
	log_unusual(log, "This is an unusual %s!", "message");
	my_time.ts.tv_nsec++;
	log_broken(log, "This is a broken %s!", "message");
	my_time.ts.tv_nsec++;

	log_add(log, "the sha is ");
	memset(&dsha, 0xFF, sizeof(dsha));
	log_add_struct(log, struct protocol_double_sha, &dsha);

	log_add(log, " and the address is: ");
	memset(netaddr.addr, 0, 10);
	memset(netaddr.addr + 10, 0xFF, 2);
	netaddr.addr[12] = 127;
	netaddr.addr[13] = 0;
	netaddr.addr[14] = 0;
	netaddr.addr[15] = 1;
	netaddr.port = cpu_to_be16(65000);
	log_add_struct(log, struct protocol_net_address, &netaddr);

	/* Make child write log, be sure it's correct. */
	pipe(fds);
	switch (fork()) {
	case -1:
		err(1, "forking");
	case 0:
		close(fds[0]);
		log_to_file(fds[1], log);
		tal_free(ctx);
		exit(0);
	}

	close(fds[1]);
	p = read_from(ctx, fds[0]);
	/* Shouldn't contain any NUL chars */
	assert(strlen(p) + 1 == tal_count(p));

	assert(tal_strreg(p, p,
			  "PREFIX ([0-9])* bytes, Sun Nov 10 16:57:35 2013\n"
			  "\\+0\\.000000500 DEBUG: This is a debug message!\n"
			  "\\+0\\.000000501 INFO: This is an info message!\n"
			  "\\+0\\.000000502 UNUSUAL: This is an unusual message!\n"
			  "\\+0\\.000000503 BROKEN: This is a broken message!the sha is ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff and the address is: ::ffff:127\\.0\\.0\\.1:65000\n\n", &mem1));
	assert(atoi(mem1) < maxmem);
	tal_free(p);

	wait(&status);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);

	/* This cleans us out! */
	log_debug(log, "Overflow!");
	
	/* Make child write log, be sure it's correct. */
	pipe(fds);
	switch (fork()) {
	case -1:
		err(1, "forking");
	case 0:
		close(fds[0]);
		log_to_file(fds[1], log);
		tal_free(ctx);
		exit(0);
	}

	close(fds[1]);

	p = read_from(ctx, fds[0]);
	/* Shouldn't contain any NUL chars */
	assert(strlen(p) + 1 == tal_count(p));

	assert(tal_strreg(p, p,
			  "PREFIX ([0-9]*) bytes, Sun Nov 10 16:57:35 2013\n"
			  "\\.\\.\\. 4 skipped\\.\\.\\.\n"
			  "\\+0.000000504 DEBUG: Overflow!\n"
			  "\\+0.000000504 DEBUG: Log pruned 4 entries \\(mem ([0-9]*) -> ([0-9]*)\\)\n\n", &mem1, &mem2, &mem3));
	assert(atoi(mem1) < maxmem);
	assert(atoi(mem2) >= maxmem);
	assert(atoi(mem3) < maxmem);
	tal_free(ctx);
	wait(&status);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);
	return 0;
}