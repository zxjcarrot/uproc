#include <uproc.h>

#include <stdint.h>
#include <float.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>


static int __uchar_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    unsigned char *v = (unsigned char *)private_data;
    int n = snprintf(buf->mem, buf->size, "%c", *v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __ushort_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    unsigned short *v = (unsigned short *)private_data;
    int n = snprintf(buf->mem, buf->size, "%u", *v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __uint_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    unsigned int *v = (unsigned int *)private_data;
    int n = snprintf(buf->mem, buf->size, "%u", *v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __ulong_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    unsigned long *v = (unsigned long *)private_data;
    int n = snprintf(buf->mem, buf->size, "%lu", *v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __ullong_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    unsigned long long*v = (unsigned long long *)private_data;
    int n = snprintf(buf->mem, buf->size, "%llu", *v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __uint16_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    uint16_t *v = (uint16_t *)private_data;
    int n = snprintf(buf->mem, buf->size, "%u", (unsigned int)*v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __uint32_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    uint32_t *v = (uint32_t *)private_data;
    int n = snprintf(buf->mem, buf->size, "%lu", (unsigned long)*v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __uint64_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    uint64_t *v = (uint64_t *)private_data;
    int n = snprintf(buf->mem, buf->size, "%llu", (unsigned long long)*v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __char_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    return __uchar_read_proc(buf, done, fileoff, private_data);
}

static int __short_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    short *v = (short *)private_data;
    int n = snprintf(buf->mem, buf->size, "%d", *v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __int_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int *v = (int *)private_data;
    int n = snprintf(buf->mem, buf->size, "%d", *v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __long_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    long *v = (long *)private_data;
    int n = snprintf(buf->mem, buf->size, "%ld", *v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __llong_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    long long *v = (long long *)private_data;
    int n = snprintf(buf->mem, buf->size, "%lld", *v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __float_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    float *v = (float *)private_data;
    int n = snprintf(buf->mem, buf->size, "%f", *v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __double_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    double *v = (double *)private_data;
    int n = snprintf(buf->mem, buf->size, "%f", *v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __ldouble_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    long double *v = (long double *)private_data;
    int n = snprintf(buf->mem, buf->size, "%Lf", *v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __int16_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int16_t *v = (int16_t *)private_data;
    int n = snprintf(buf->mem, buf->size, "%d", (int)*v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __int32_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int32_t *v = (int32_t *)private_data;
    int n = snprintf(buf->mem, buf->size, "%ld", (long)*v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __int64_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int64_t *v = (int64_t *)private_data;
    int n = snprintf(buf->mem, buf->size, "%lld", (long long)*v);
    if (n > 0) {
        if (n >= buf->size){
            buf->mem[buf->size - 1] = '\n';
        } else {
            buf->mem[n] = '\n';
            ++n;
        }
    }
    *done = 1;
    return n;
}

static int __cstring_read_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    const char *v = (const char *)private_data;
    size_t len;
    size_t size = (buf->entry->size > buf->size ? buf->size : buf->entry->size);

    *done = 1;
    if (size <= 1) {
        return 0;
    }

    len = strlen(v);
    if (len < size) {
        size = len;
    }

    --size; // save a byte for '\n'

    memcpy(buf->mem, v, size);

    buf->mem[size] = '\n';
    return size + 1;
}


#define __WRITE_BUF_SIZE 5000
// buffer for making uproc_buf_t into a null-terminated string.
static char __write_buf[__WRITE_BUF_SIZE];

static inline void __copy_to_write_buf(uproc_buf_t *buf) {
    uproc_dentry_t *entry = buf->entry;
    size_t size = buf->size > entry->size ? entry->size : buf->size;
    if (size > __WRITE_BUF_SIZE - 1) {
        size = __WRITE_BUF_SIZE - 1;
    }
    memcpy(__write_buf, buf->mem, size);
    __write_buf[size] = '\0';
}

static int __uchar_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = 0;
    unsigned char *v = (unsigned char *)private_data;
    *done = 1;
    if (buf->size > 0) {
        *v = buf->mem[0];
        n = 1;
    }
    return n;
}

static int __ushort_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = buf->size;
    unsigned short *v = (unsigned short *)private_data;
    unsigned long int x;
    char *endptr;
    __copy_to_write_buf(buf);
    *done = 1;

    x = strtoul(__write_buf, &endptr, 10);

    if (errno == ERANGE || x > USHRT_MAX) {
        n = -ERANGE;
    } else if (endptr == __write_buf) {
        n = -EINVAL;
    } else {
        *v = x;
    }

    return n;
}

static int __uint_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = buf->size;
    unsigned int *v = (unsigned int *)private_data;
    unsigned long long int x;
    char *endptr;
    __copy_to_write_buf(buf);
    *done = 1;

    x = strtoull(__write_buf, &endptr, 10);

    if (errno == ERANGE || x > UINT_MAX) {
        n = -ERANGE;
    } else if (endptr == __write_buf) {
        n = -EINVAL;
    } else {
        *v = x;
    }
    return n;
}

static int __ulong_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = buf->size;
    unsigned long *v = (unsigned long *)private_data;
    unsigned long long int x;
    char *endptr;
    __copy_to_write_buf(buf);
    *done = 1;

    x = strtoull(__write_buf, &endptr, 10);

    if (errno == ERANGE || x > ULONG_MAX) {
        n = -ERANGE;
    } else if (endptr == __write_buf) {
        n = -EINVAL;
    } else {
        *v = x;
    }
    return n;
}

static int __ullong_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = buf->size;
    unsigned long long *v = (unsigned long long *)private_data;
    unsigned long long int x;
    char *endptr;
    __copy_to_write_buf(buf);
    *done = 1;

    x = strtoull(__write_buf, &endptr, 10);

    if (errno == ERANGE) {
        n = -ERANGE;
    } else if (endptr == __write_buf) {
        n = -EINVAL;
    } else {
        *v = x;
    }
    return n;
}

static int __uint16_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = buf->size;
    uint16_t *v = (uint16_t *)private_data;
    unsigned long int x;
    char *endptr;
    __copy_to_write_buf(buf);
    *done = 1;

    x = strtoul(__write_buf, &endptr, 10);

    if (errno == ERANGE || x > UINT16_MAX) {
        n = -ERANGE;
    } else if (endptr == __write_buf) {
        n = -EINVAL;
    } else {
        *v = x;
    }

    return n;
}

static int __uint32_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = buf->size;
    uint32_t *v = (uint32_t *)private_data;
    unsigned long long int x;
    char *endptr;
    __copy_to_write_buf(buf);
    *done = 1;

    x = strtoull(__write_buf, &endptr, 10);

    if (errno == ERANGE || x > UINT32_MAX) {
        n = -ERANGE;
    } else if (endptr == __write_buf) {
        n = -EINVAL;
    } else {
        *v = x;
    }
    return n;
}

static int __uint64_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = buf->size;
    uint64_t *v = (uint64_t *)private_data;
    unsigned long long int x;
    char *endptr;
    __copy_to_write_buf(buf);
    *done = 1;

    x = strtoull(__write_buf, &endptr, 10);

    if (errno == ERANGE || x > UINT64_MAX) {
        n = -ERANGE;
    } else if (endptr == __write_buf) {
        n = -EINVAL;
    } else {
        *v = x;
    }
    return n;
}

static int __char_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    return __uchar_write_proc(buf, done, fileoff, private_data);
}

static int __short_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = buf->size;
    short *v = (short *)private_data;
    long int x;
    char *endptr;
    __copy_to_write_buf(buf);
    *done = 1;

    x = strtol(__write_buf, &endptr, 10);

    if (errno == ERANGE || x > SHRT_MAX || x < SHRT_MIN) {
        n = -ERANGE;
    } else if (endptr == __write_buf) {
        n = -EINVAL;
    } else {
        *v = x;
    }
    return n;
}

static int __int_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = buf->size;
    int *v = (int *)private_data;
    long long int x;
    char *endptr;
    __copy_to_write_buf(buf);
    *done = 1;

    x = strtoll(__write_buf, &endptr, 10);

    if (errno == ERANGE || x > INT_MAX || x < INT_MIN) {
        n = -ERANGE;
    } else if (endptr == __write_buf) {
        n = -EINVAL;
    } else {
        *v = x;
    }
    return n;
}

static int __long_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = buf->size;
    long *v = (long *)private_data;
    long long int x;
    char *endptr;
    __copy_to_write_buf(buf);
    *done = 1;

    x = strtoll(__write_buf, &endptr, 10);

    if (errno == ERANGE || x > LONG_MAX || x < LONG_MIN) {
        n = -ERANGE;
    } else if (endptr == __write_buf) {
        n = -EINVAL;
    } else {
        *v = x;
    }
    return n;
}

static int __llong_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = buf->size;
    long long *v = (long long *)private_data;
    long long int x;
    char *endptr;
    __copy_to_write_buf(buf);
    *done = 1;

    x = strtoll(__write_buf, &endptr, 10);

    if (errno == ERANGE) {
        n = -ERANGE;
    } else if (endptr == __write_buf) {
        n = -EINVAL;
    } else {
        *v = x;
    }
    return n;
}

static int __float_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = buf->size;
    float *v = (float *)private_data;
    float x;
    char *endptr;
    __copy_to_write_buf(buf);
    *done = 1;

    x = strtof(__write_buf, &endptr);

    if (errno == ERANGE) {
        n = -ERANGE;
    } else if (endptr == __write_buf) {
        n = -EINVAL;
    } else {
        *v = x;
    }
    return n;
}

static int __double_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = buf->size;
    double *v = (double *)private_data;
    double x;
    char *endptr;
    __copy_to_write_buf(buf);
    *done = 1;

    x = strtod(__write_buf, &endptr);

    if (errno == ERANGE) {
        n = -ERANGE;
    } else if (endptr == __write_buf) {
        n = -EINVAL;
    } else {
        *v = x;
    }
    return n;
}

static int __ldouble_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = buf->size;
    long double *v = (long double *)private_data;
    long double x;
    char *endptr;
    __copy_to_write_buf(buf);
    *done = 1;

    x = strtold(__write_buf, &endptr);

    if (errno == ERANGE) {
        n = -ERANGE;
    } else if (endptr == __write_buf) {
        n = -EINVAL;
    } else {
        *v = x;
    }
    return n;
}

static int __int16_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = buf->size;
    int16_t *v = (int16_t *)private_data;
    long int x;
    char *endptr;
    __copy_to_write_buf(buf);
    *done = 1;

    x = strtol(__write_buf, &endptr, 10);

    if (errno == ERANGE || x > INT16_MAX || x < INT16_MIN) {
        n = -ERANGE;
    } else if (endptr == __write_buf) {
        n = -EINVAL;
    } else {
        *v = x;
    }

    return n;
}

static int __int32_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = buf->size;
    int32_t *v = (int32_t *)private_data;
    long long int x;
    char *endptr;
    __copy_to_write_buf(buf);
    *done = 1;

    x = strtoll(__write_buf, &endptr, 10);

    if (errno == ERANGE || x > INT32_MAX || x < INT32_MIN) {
        n = -ERANGE;
    } else if (endptr == __write_buf) {
        n = -EINVAL;
    } else {
        *v = x;
    }

    return n;
}

static int __int64_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    int n = buf->size;
    int64_t *v = (int64_t *)private_data;
    long long int x;
    char *endptr;
    __copy_to_write_buf(buf);
    *done = 1;

    x = strtoll(__write_buf, &endptr, 10);

    if (errno == ERANGE) {
        n = -ERANGE;
    } else if (endptr == __write_buf) {
        n = -EINVAL;
    } else {
        *v = x;
    }

    return n;
}

static int __string_write_proc(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    char *v = (char *)private_data;
    size_t size = (buf->entry->size > buf->size ? buf->size : buf->entry->size);
    *done = 1;
    memcpy(v, buf->mem , size);
    return size + 1;
}

static uproc_dentry_t* __uproc_utility_create_internal(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       size_t size,
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       uproc_read_proc_t read_proc,
                                       uproc_write_proc_t write_proc,
                                       void *private_data) {
    if (size == 0)
        size = 4096;
    if (readonly)
        write_proc = NULL;
    return uproc_create_entry(ctx, name, mode, size, parent, read_proc, write_proc, private_data);
}


/*
* Wrappers for primitive types.
* @readonly: if set, only the default read hanlder will be installed.
*/

uproc_dentry_t* uproc_create_entry_uchar(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       unsigned char *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                           __uchar_read_proc, __uchar_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_ushort(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       unsigned short *v
                                   ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __ushort_read_proc, __ushort_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_uint(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       unsigned int *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __uint_read_proc, __uint_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_ulong(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       unsigned long *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __ulong_read_proc, __ulong_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_ullong(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       unsigned long long *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __ullong_read_proc, __ullong_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_uint16(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       uint16_t *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __uint16_read_proc, __uint16_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_uint32(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       uint32_t *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __uint32_read_proc, __uint32_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_uint64(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       uint64_t *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __uint64_read_proc, __uint64_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_char(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       char *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __char_read_proc, __char_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_short(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       short *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __short_read_proc, __short_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_int(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       int *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __int_read_proc, __int_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_long(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       long *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __long_read_proc, __long_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_llong(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       long long *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __llong_read_proc, __llong_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_float(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       float *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __float_read_proc, __float_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_double(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       double *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __double_read_proc, __double_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_ldouble(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       long double *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __ldouble_read_proc, __ldouble_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_int16(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       int16_t *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __int16_read_proc, __int16_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_int32(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       int32_t *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __int32_read_proc, __int32_write_proc, (void*)v);
}

uproc_dentry_t* uproc_create_entry_int64(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       int64_t *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                       __int64_read_proc, __int64_write_proc, (void*)v);
}

// wrapper for const char *, it's readonly by definition.
uproc_dentry_t* uproc_create_entry_cstring(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       size_t size, // maximum size of content the entry could hold, content wil be trauncated if exceeds this limit
                                       const char *v
                                       ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, /* readonly */ 1,
                                       __cstring_read_proc, /* write_proc */ NULL, (void*)v);
}

// wrapper for char *.
// Note: when writing to the entry, uproc honors the @size variable.
//  Specifically,uproc overrides the content of the char arry
//  pointed by @v and puts '\0' at the end of the string.
uproc_dentry_t* uproc_create_entry_string(uproc_ctx_t *ctx,
                                           const char *name, // name of the entry
                                           mode_t mode,      // permissions
                                           uproc_dentry_t* parent,
                                           int readonly,
                                           size_t size, // maximum size of content the entry could hold, content wil be trauncated if exceeds this limit
                                           char *v
                                           ) {
    return __uproc_utility_create_internal(ctx, name, mode, 0, parent, readonly,
                                   __cstring_read_proc, __string_write_proc, (void*)v);
}