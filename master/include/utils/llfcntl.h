#ifndef LLFCNTL_H
#define LLFCNTL_H

// ---------------------
// Internal definitions.
// ---------------------

#ifdef __cplusplus
extern "C" {
#endif

int llfcntl(int fd, int op, int flags);

#ifdef __cplusplus
}
#endif

#endif // LLFCNTL_H