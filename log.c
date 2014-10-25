#include "hex.h"
#include "log.h"
#include "pseudorand.h"
#include <ccan/list/list.h>
#include <ccan/read_write_all/read_write_all.h>
#include <ccan/tal/str/str.h>
#include <ccan/time/time.h>
#include <errno.h>
#include <stdio.h>

struct log_entry {
	struct list_node list;
	struct timeabs time;
	enum log_level level;
	unsigned int skipped;
	const char *prefix;
	char *log;
};

struct log_record {
	size_t mem_used;
	size_t max_mem;
	enum log_level print;

	struct list_head log;
};

struct log {
	struct log_record *lr;
	const char *prefix;
};

static size_t log_bufsize(const struct log_entry *e)
{
	if (e->level == LOG_IO)
		return tal_count(e->log);
	else
		return strlen(e->log) + 1;
}

static size_t prune_log(struct log_record *log)
{
	struct log_entry *i, *next, *tail;
	size_t skipped = 0, deleted = 0;

	/* Never delete the last one. */
	tail = list_tail(&log->log, struct log_entry, list);

	list_for_each_safe(&log->log, i, next, list) {
		/* 50% chance of deleting debug, 25% inform, 12.5% unusual. */
		if (i == tail || !isaac64_next_uint(isaac64, 2 << i->level)) {
			i->skipped += skipped;
			skipped = 0;
			continue;
		}

		list_del_from(&log->log, &i->list);
		log->mem_used -= sizeof(*i) + log_bufsize(i);
		tal_free(i);
		skipped++;
		deleted++;
	}

	assert(!skipped);
	return deleted;
}

struct log_record *new_log_record(const tal_t *ctx,
				  size_t max_mem,
				  enum log_level printlevel)
{
	struct log_record *lr = tal(ctx, struct log_record);

	/* Give a reasonable size for memory limit! */
	assert(max_mem > sizeof(struct log) * 2);
	lr->mem_used = 0;
	lr->max_mem = max_mem;
	lr->print = printlevel;
	list_head_init(&lr->log);

	return lr;
}

/* With different entry points */
struct log *PRINTF_FMT(3,4)
new_log(const tal_t *ctx, struct log_record *record, const char *fmt, ...)
{
	struct log *log = tal(ctx, struct log);
	va_list ap;

	log->lr = record;
	va_start(ap, fmt);
	/* log->lr owns this, since its entries keep a pointer to it. */
	log->prefix = tal_vfmt(log->lr, fmt, ap);
	va_end(ap);

	return log;
}

void set_log_level(struct log_record *lr, enum log_level level)
{
	lr->print = level;
}

void set_log_prefix(struct log *log, const char *prefix)
{
	/* log->lr owns this, since it keeps a pointer to it. */
	log->prefix = tal_strdup(log->lr, prefix);
}

const char *log_prefix(const struct log *log)
{
	return log->prefix;
}

static void add_entry(struct log *log, struct log_entry *l)
{
	log->lr->mem_used += sizeof(*l) + log_bufsize(l);
	list_add_tail(&log->lr->log, &l->list);

	if (log->lr->mem_used > log->lr->max_mem) {
		size_t old_mem = log->lr->mem_used, deleted;
		deleted = prune_log(log->lr);
		log_debug(log, "Log pruned %zu entries (mem %zu -> %zu)",
			  deleted, old_mem, log->lr->mem_used);
	}
}

static struct log_entry *new_log_entry(struct log *log, enum log_level level)
{
	struct log_entry *l = tal(log, struct log_entry);

	l->time = time_now();
	l->level = level;
	l->skipped = 0;
	l->prefix = log->prefix;

	return l;
}

void logv(struct log *log, enum log_level level, const char *fmt, va_list ap)
{
	struct log_entry *l = new_log_entry(log, level);

	l->log = tal_vfmt(l, fmt, ap);

	if (level >= log->lr->print)
		printf("%s %s\n", log->prefix, l->log);

	add_entry(log, l);
}

void log_io(struct log *log, bool in, const void *data, size_t len)
{
	int save_errno = errno;
	struct log_entry *l = new_log_entry(log, LOG_IO);

	l->log = tal_arr(l, char, 1 + len);
	l->log[0] = in;
	memcpy(l->log + 1, data, len);

	if (LOG_IO >= log->lr->print) {
		char *hex = to_hex(l, data, len);
		printf("%s[%s] %s\n", log->prefix, in ? "IN" : "OUT", hex);
		tal_free(hex);
	}
	add_entry(log, l);
	errno = save_errno;
}

static void do_log_add(struct log *log, const char *fmt, va_list ap)
{
	struct log_entry *l = list_tail(&log->lr->log, struct log_entry, list);
	size_t oldlen = strlen(l->log);

	/* Remove from list, so it doesn't get pruned. */
	log->lr->mem_used -= sizeof(*l) + oldlen + 1;
	list_del_from(&log->lr->log, &l->list);

	tal_append_vfmt(&l->log, fmt, ap);
	add_entry(log, l);

	if (l->level >= log->lr->print)
		printf("%s \t%s\n", log->prefix, l->log + oldlen);
}

void log_(struct log *log, enum log_level level, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	logv(log, level, fmt, ap);
	va_end(ap);
}

void log_add(struct log *log, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	do_log_add(log, fmt, ap);
	va_end(ap);
}


void log_to_file(int fd, const struct log_record *lr)
{
	const struct log_entry *i;
	char buf[100];
	struct timeabs prev;
	time_t start;
	const char *prefix;

	i = list_top(&lr->log, const struct log_entry, list);
	if (!i) {
		write_all(fd, "0 bytes:\n\n", strlen("0 bytes:\n\n"));
		return;
	}

	/* ctime only does seconds, so start log from 0ns past the second. */
	prev = i->time;
	prev.ts.tv_nsec = 0;
	start = prev.ts.tv_sec;
	sprintf(buf, "%zu bytes, %s", lr->mem_used, ctime(&start));
	write_all(fd, buf, strlen(buf));

	/* ctime includes \n... WTF? */
	prefix = "";

	list_for_each(&lr->log, i, list) {
		struct timerel diff;

		if (i->skipped) {
			sprintf(buf, "%s... %u skipped...", prefix, i->skipped);
			write_all(fd, buf, strlen(buf));
			prefix = "\n";
		}
		diff = time_between(i->time, prev);

		sprintf(buf, "%s+%lu.%09u %s%s: ",
			prefix,
			(unsigned long)diff.ts.tv_sec,
			(unsigned)diff.ts.tv_nsec,
			i->prefix,
			i->level == LOG_IO ? (i->log[0] ? "IO-IN" : "IO-OUT")
			: i->level == LOG_DBG ? "DEBUG"
			: i->level == LOG_INFORM ? "INFO"
			: i->level == LOG_UNUSUAL ? "UNUSUAL"
			: i->level == LOG_BROKEN ? "BROKEN"
			: "**INVALID**");

		write_all(fd, buf, strlen(buf));
		if (i->level == LOG_IO) {
			size_t off, used, len = tal_count(i->log)-1;

			/* No allocations, may be in signal handler. */
			for (off = 0; off < len; off += used) {
				used = to_hex_direct(buf, sizeof(buf),
						     i->log + 1 + off,
						     len - off);
				write_all(fd, buf, strlen(buf));
			}
		} else {
			write_all(fd, i->log, strlen(i->log));
		}
		prefix = "\n";
	}
	write_all(fd, "\n\n", strlen("\n\n"));
}
