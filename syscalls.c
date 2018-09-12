
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "nrfx_uarte.h"

extern nrfx_uarte_t uart0;

__attribute__((used))
int _write_r(struct _reent *r, int file, char *data, int len)
{
	(void)r;
	(void)file;

	if(file != STDOUT_FILENO)
		return -ENODEV;

	if(NRFX_SUCCESS != nrfx_uarte_tx(&uart0, (unsigned char*)data, len))
		return -EINVAL;

	return len;
}

__attribute__((used))
int _read_r(struct _reent *r, int file, char *data, int len)
{
	(void)r;
	(void)file;

	if(file != STDIN_FILENO)
		return -ENODEV;

	if(NRFX_SUCCESS != nrfx_uarte_rx(&uart0, (unsigned char*)data, len))
		return -EINVAL;

	return len;
}

