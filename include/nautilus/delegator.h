// MODIFY ME

#include <stdint.h>
#include <stddef.h>

size_t DEL_open(const char *pathname, int flags, int mode);
size_t DEL_read (int fd, void *buf, size_t nbytes);
size_t DEL_write (int fd, const void *buf, size_t n);
size_t DEL_close (int fd);

void test_delcall();
