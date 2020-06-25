// MODIFY ME

#include <stddef.h>
#include <stdint.h>
#include <nautilus/delegator.h>
#include <arch/hrt/hrt.h>
#include <config/hrt/hihalf/offset.h>

struct hvm_del_req_args {
	size_t arg0;
	size_t arg1;
	size_t arg2;
};

size_t v2p(const void *v) {
	hrt_print(""); // Uh... what? Why does this function not work if I remove this?
	return (size_t)v & ~NAUT_CONFIG_HRT_HIHALF_OFFSET;
}

static size_t del_call(uint32_t req_id, struct hvm_del_req_args *args);

size_t DEL_open(const char *pathname, int flags, int mode) {
	struct hvm_del_req_args args;

	args.arg0 = v2p(pathname);
	args.arg1 = flags;
	args.arg2 = mode;

	return del_call(0x1, &args);
}

size_t DEL_read (int fd, void *buf, size_t nbytes) {
	struct hvm_del_req_args args;

	args.arg0 = fd;
	args.arg1 = v2p(buf);
	args.arg2 = nbytes;

	return del_call(0x2, &args);
}

size_t DEL_write (int fd, const void *buf, size_t nbytes) {
	struct hvm_del_req_args args;
	
	args.arg0 = fd;
	args.arg1 = v2p(buf);
	args.arg2 = nbytes;

	return del_call(0x3, &args);
}

size_t DEL_close (int fd) {
	struct hvm_del_req_args args;

	args.arg0 = fd;

	return del_call(0x4, &args);
}

static size_t del_call(uint32_t req_id, struct hvm_del_req_args *args) {
	size_t retval = 0;
	const uint16_t port = 0x7C4;

	size_t argptr = v2p(args);

	asm volatile (
		"movq %1, %%r8;"
		"out %2, %3;"
		"movq %%r8, %0;"

		: "=r" (retval)
		: "r" (argptr), "a" (req_id), "d" (port)
		: "%r8"
	);

	return retval;
}

void test_delcall() {
	char *pathname = "/etc/hostname";
	int fd = DEL_open(pathname, 0, 0);

	char buf[512];
	size_t bytes = DEL_read(fd, buf, 512);
	for (size_t j = 0; j < 100; j++) {
		for (size_t i = 0; i < bytes; i++) {
			hrt_putchar(buf[i]);
		}
		hrt_putchar('\n');
	}
	DEL_close(fd);
}
