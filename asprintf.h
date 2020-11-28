
#ifndef _ASPRINTF_H
#define _ASPRINTF_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static int vasprintf(char **buf, const char *format, va_list va) {
	int len, ret;
	va_list tmp_va;
	char dummy;

	va_copy(tmp_va, va);
	len = vsnprintf(&dummy, 0, format, tmp_va);
	va_end(tmp_va);
	if (len < 0) {
		*buf = NULL;
		return (len);
	}

	len++;
	*buf = malloc(len);
	if (*buf == NULL)
		return -1;

	ret = vsnprintf(*buf, len, format, va);

	if (ret < 0) {
		free(*buf);
		*buf = NULL;
	}

	return ret;
}

static int asprintf(char **buf, const char *format, ...) {
	int ret;
	va_list va;

	va_start(va, format);
	ret = vasprintf(buf, format, va);
	va_end(va);

	return ret;
}

#endif
