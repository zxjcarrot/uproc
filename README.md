# Overview
`uproc` is the acronym for userspace /proc filesystem. `uproc` lets you export your program's states into a directory structure just like linux kernel's /proc filesystem. 

# Design
1. `uproc` runs a eventloop to process read and write requests, so `uproc` should be run in a independent thread.
2. `uproc` provides very few core interfaces, and some utility wrappers for exporting primitive types. Checkout `include/uproc.h` to see detailed usage of the interfaces. 
3. Every pathname in `uproc` is associated with two handlers which handles read and write syscalls respectively. 
4. Handler is in the form of:
```C
    /*
    * @buf: For read request, @buf stores the buffer into which data should be written by your handler.
    *       For write request, @buf stores the buffer from which data should be read by your handler.
    * @done: Indicator of the completion of the request, if *done is set to 1 by your handler,
    *        following read or write syscalls of the request will be simply returned 
    *         with the requested size of the syscall.
    * @fileoff: the offset of the syscall.
    * @private_data: user data provided by your program at the registration.
    */
    int (*uproc_handler_t)(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data);
```

# Dependency
`uproc` is built on `fuse` and written in standard C, so only `libfuse` is required.

#HowToUse
Example: creates 3 entry under root directory of `uproc` filesystem.
```C
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>

#include <uproc/uproc.h>

int var1;
const char * var2;
int var3;

int uproc_read_var1(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    ++var1;
    int n = snprintf(buf->mem, buf->size, "%d\n", var1);
    if (n > 0) {
        *done = 1;
    }
    return n;
}

int uproc_write_var1(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    const char *p = buf->mem;
    int x = 0;
    while (p - buf->mem < buf->size && !isdigit(*p))
        ++p;

    while (p - buf->mem < buf->size && isdigit(*p)){
        x = x * 10 + *p++ - '0';
    }

    var1 = x;
    return buf->size;
}

int uproc_read_var2(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = snprintf(buf->mem, buf->size, "%s\n", var2);
    if (n > 0) {
        *done = 1;
    }
    return n;
}

int main(int argc, char const *argv[]) {
    int ret;
    var1 = 123;
    var2 = "das";
    var3 = 445;
    uproc_ctx_t uproc_ctx;

    ret = uproc_ctx_init(&uproc_ctx, "uproc", 1);
    if (ret) {
        fprintf(stderr, "failed to initialize uproc %s\n", strerror(ret));
        exit(-1);
    }

    /* readable writable entry */
    uproc_dentry_t *ent1 = uproc_create_entry(&uproc_ctx, /* name */"var1",
                                            /* permission */ S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH,
                                            /* entry size */4096,
                                            /* use root the parent*/ NULL,
                                            /* read handler */ uproc_read_var1,
                                            /* write handler */ uproc_write_var1,
                                            /* user data */ NULL);
    /* readonly entry */
    uproc_dentry_t *ent2 = uproc_create_entry(&uproc_ctx, /* name */"var2",
                                            /* default permission S_IRUGO */0,
                                            /* entry size */4096,
                                            /* use root the parent*/ NULL,
                                            /* read handler */ uproc_read_var2,
                                            /* write handler */NULL,
                                            /* user data */ NULL);
    /* create entry using int handler provided by uproc */
    uproc_dentry_t *ent3 = uproc_create_entry_int(&uproc_ctx, 
                                            /* name */ "var3",
                                            /* permission */ 0,
                                            /* parent */ NULL,
                                            /* readonly */ 0,
                                            /* pointer to the variable */ &var3);
    assert(ent1 && ent2 && ent3);
    uproc_run(&uproc_ctx);
    return 0;
}
```
For more examples, see `tests/*`, `example/*`.

#Build&Test
###To build:  
1. first make sure `libfuse` is installed on the system.
2. run following commands:
```shell
git clone https://github.com/zxjcarrot/uproc
cd uproc
make
make install #install headers under /usr/local/include, libs under /usr/local/lib
```

###To examples:  
1. first make sure `libfuse` is installed on the system.
2. run following commands:
```shell
git clone https://github.com/zxjcarrot/uproc
cd uproc
make exmaple
```

###To test:

1. first make sure `libfuse` is installed on the system.
2. run following commands:
```
git clone https://github.com/zxjcarrot/uproc
cd uproc
make test
./uproc_test
```
