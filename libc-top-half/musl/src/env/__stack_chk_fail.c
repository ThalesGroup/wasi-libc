#include <string.h>
#include <stdint.h>
#include <unistd.h>
#if defined(__wasilibc_unmodified_upstream) || defined(_REENTRANT)
#include "pthread_impl.h"
#else
// In non-_REENTRANT, include it for `a_crash`
# include "atomic.h"
#endif

__asm__(
	".globaltype __stack_chk_guard, i32\n"
	".global __stack_chk_guard, __stack_chk_guard\n"
	"__stack_chk_guard:\n"
);

void __init_ssp(void *entropy)
{
	if (!entropy) _exit(1);

	__asm__ volatile (
		"i32.const %0\n"
		"i32.load 0\n"
		"global.set __stack_chk_guard\n"
		:
		: "r" (entropy)
	);

}

void __stack_chk_fail(void)
{
	a_crash();
}

hidden void __stack_chk_fail_local(void);

weak_alias(__stack_chk_fail, __stack_chk_fail_local);

#ifndef __wasilibc_unmodified_upstream
#ifdef __wasilibc_use_wasip2
# include <wasi/libc.h>
#else
# include <wasi/api.h>
#endif

__attribute__((constructor(60)))
static void __wasilibc_init_ssp(void) {
	uintptr_t entropy;
#ifdef __wasilibc_use_wasip2
        int len = sizeof(uintptr_t);

        int r = __wasilibc_random(&entropy, len);
#else
	int r = __wasi_random_get((uint8_t *)&entropy, sizeof(uintptr_t));
#endif
	__init_ssp(r ? NULL : &entropy);
}
#endif
