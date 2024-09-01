#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

/* LD_PRELOAD override that causes the process to immediately return
 * EXIT_SUCCESS. This is useful if you'd like to time the startup of a process
 * without actually accounting for any work the process does.
 *
 * Loosely based on libminijailpreload.c by Chromium OS authors:
 * https://android.googlesource.com/platform/external/minijail/+/master/libminijailpreload.c
 * and
 * https://github.com/thomasnyman/abort_at_exit
 */
static int fake_main(int argc, char **argv, char **envp) {
  // do nothing
  return EXIT_SUCCESS;
}

/* LD_PRELOAD override of __libc_start_main.
 *
 * We cannot use LD_PRELOAD to override main directly as
 * LD_PRELOAD can only be used to override functions in dynamically linked
 * shared libraries whose addresses are determined via the Procedure
 * Linkage Table (PLT). However, main's location is not determined via the PLT,
 * but is statically linked to the executable entry routine at __start which
 * pushes main's address onto the stack, then invokes libc's startup routine,
 * which obtains main's address from the stack.
 *
 * Instead, we use LD_PRELOAD to override libc's startup routine,
 * __libc_start_main, which is normally responsible for calling main.
 */
int __libc_start_main(int (*main)(int, char **, char **), int argc,
                      char **ubp_av, void (*init)(void), void (*fini)(void),
                      void (*rtld_fini)(void), void(*stack_end)) {
  union {
    int (*fn)(int (*)(int, char **, char **), int, char **, void (*)(void),
              void (*)(void), void (*)(void), void *);
    void *sym;
  } real_libc_start_main;

  // Fix the warning by first casting to (void *)
  real_libc_start_main.sym = dlsym(RTLD_NEXT, "__libc_start_main");

  if (!real_libc_start_main.sym) {
    fprintf(stderr, "can't find __libc_start_main():%s\n", dlerror());
    _exit(EXIT_FAILURE);
  }

  // Call the original __libc_start_main with a dummy main function that just
  // exits
  return real_libc_start_main.fn(fake_main, argc, ubp_av, init, fini, rtld_fini,
                                 stack_end);
}
