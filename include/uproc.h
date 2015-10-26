#ifndef _UPROC_UPROC_H_
#define _UPROC_UPROC_H_
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

#include "list.h"
#include "htable.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct uproc_ctx       uproc_ctx_t;
typedef struct uproc_dentry    uproc_dentry_t;
typedef struct uproc_buf       uproc_buf_t;

/*
* @buf: For read request, @buf stores the buffer into which data should be written by your handler.
*       For write request, @buf stores the buffer from which data should be read by your handler.
* @done: Indicator of the completion of the request, if *done is set to 1 by your handler,
*        following read or write syscalls of the request will be simply returned 
*         with the requested size of the syscall.
* @fileoff: the offset of the syscall.
* @private_data: user data provided by your program at the registration.
*/
typedef int (*uproc_handler_t)(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data);
typedef uproc_handler_t uproc_read_proc_t ;
typedef uproc_handler_t uproc_write_proc_t;

struct uproc_ctx {
    uproc_dentry_t  *root;  // root dir entry
    /*
    * hash table of dentries for rapid lookups.
    * A entry is hashed by its parent entry and the name of the entry.
    * Very much like the dentry hash table in linux kernel.
    */
    uproc_htable_t   htable;
    const char      *mount_point;
    struct fuse     *fuse;
    int              dbg; // if debug is on
};

struct uproc_dentry {
    char               *name;
    size_t              namelen;
    size_t              size;
    uproc_dentry_t     *parent, *next, *children;
    mode_t              mode;     // S_ISDIR, S_ISREG etc..
    uid_t               uid;
    gid_t               gid;
    struct hlist_node   hlink;     // hash link
    void *              private_data; // used by user
    uproc_read_proc_t   read_proc;
    uproc_read_proc_t   write_proc;
};

struct uproc_buf {
    char            *mem;
    size_t           size;
    uproc_dentry_t  *entry;
    int              done;
};

/*
* Allocates memory for uproc filesystem, do proper initialization.
* @ctx: uproc context structure
* @mount_point: path where uproc will be mounted
* @dbg: when turned on, debug error messages will be printed to standard error.
* A return value of 0 indicates success, otherwise error code is returned.
*/
int uproc_ctx_init(uproc_ctx_t *ctx, const char *mount_point, int dbg);

/*
* Starts the event loop of uproc filesystem.
* To cleanly stop uproc, call uproc_exit().
* After receiving the exit signal from uproc_exit(),
* it calls uproc_destroy to release resouces.
*/
int uproc_run(uproc_ctx_t *ctx);

/*
* Unregisters all the uproc entry from the filesystem and deallocates memory. 
*/
void uproc_destroy(uproc_ctx_t *ctx);
/* 
* Tells uproc to exit.
*/
void uproc_exit(uproc_ctx_t *ctx);


/*
* Make a uproc directory and register to the uproc filesystem.
* @ctx: uproc context.
* @name: name of the directory.
* @mode: access permission for the directory.
* @parent: parent directory, the directory is registered under the root directory if NULL provided.
* returns a pointer to the uproc_dentry_t structure.
* Note: if @name starts with '/', @parent argument is ignored.
*/
uproc_dentry_t* uproc_mkdir_mode(uproc_ctx_t *ctx,
                                 const char *name,
                                 mode_t mode,
                                 uproc_dentry_t *parent);

/*
* Same as uproc_mkdir_mode(), but with default access permission of S_IRUGO | S_IXUGO.
*/
uproc_dentry_t* uproc_mkdir(uproc_ctx_t *ctx,
                            const char *name,
                            uproc_dentry_t* parent);

/*
* Make a uproc entry and register to the uproc filesystem.
* @ctx: uproc context.
* @name: name of the entry.
* @mode: access permission for the entry.
* @size: maximum size of content the entry could hold.
* @parent: parent directory, the entry is registered under the root directory if NULL provided.
* @read_proc: read handler, the read syscall is ignored if NULL provided.
* @write_proc: read handler, the write syscall is ignored if NULL provided.
* @private_data: user data to pass to the handler.
* returns a pointer to the uproc_dentry_t structure, otherwise NULL is returned.
* Note: if @name starts with '/', @parent argument is ignored.
*
* examples:
    1.create a entry named "var1" and register it under @dir directory.
    uproc_create_entry(ctx, "var1", 0, 4096, dir, NULL, NULL, NULL);

    2.create a entry named "var1" and register under root direcotry
    uproc_create_entry(ctx, "var1", 0, 4096, NULL, NULL, NULL, NULL);
    
    3.create a entry named "var1" and register under "/dir" directory
    uproc_create_entry(ctx, "dir/var1", 0, 4096, NULL, NULL, NULL, NULL);

    4.create a entry named "var1" and register under @dir"/subdir" directory
    uproc_create_entry(ctx, "subdir/var1", 0, 4096, dir, NULL, NULL, NULL);
*/
uproc_dentry_t* uproc_create_entry(uproc_ctx_t *ctx,
                                   const char *name,
                                   mode_t mode,
                                   size_t size,
                                   uproc_dentry_t* parent,
                                   uproc_read_proc_t read_proc,   //read handler
                                   uproc_write_proc_t write_proc, // write handler
                                   void *private_data);           // user data

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
                                       );

uproc_dentry_t* uproc_create_entry_ushort(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       unsigned short *v
                                       );

uproc_dentry_t* uproc_create_entry_uint(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       unsigned int *v
                                       );

uproc_dentry_t* uproc_create_entry_ulong(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       unsigned long *v
                                       );

uproc_dentry_t* uproc_create_entry_ullong(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       unsigned long long *v
                                       );

uproc_dentry_t* uproc_create_entry_uint16(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       uint16_t *v
                                       );

uproc_dentry_t* uproc_create_entry_uint32(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       uint32_t *v
                                       );

uproc_dentry_t* uproc_create_entry_uint64(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       uint64_t *v
                                       );

uproc_dentry_t* uproc_create_entry_char(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       char *v
                                       );

uproc_dentry_t* uproc_create_entry_short(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       short *v
                                       );

uproc_dentry_t* uproc_create_entry_int(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       int *v
                                       );

uproc_dentry_t* uproc_create_entry_long(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       long *v
                                       );

uproc_dentry_t* uproc_create_entry_llong(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       long long *v
                                       );

uproc_dentry_t* uproc_create_entry_float(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       float *v
                                       );

uproc_dentry_t* uproc_create_entry_double(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       double *v
                                       );

uproc_dentry_t* uproc_create_entry_ldouble(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       long double *v
                                       );

uproc_dentry_t* uproc_create_entry_int16(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       int16_t *v
                                       );

uproc_dentry_t* uproc_create_entry_int32(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       int32_t *v
                                       );

uproc_dentry_t* uproc_create_entry_int64(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       int readonly,
                                       int64_t *v
                                       );

// wrapper for const char *, it's readonly by definition.
// The handler always append a '\n' to the end of the buffer.
uproc_dentry_t* uproc_create_entry_cstring(uproc_ctx_t *ctx,
                                       const char *name, // name of the entry
                                       mode_t mode,      // permissions
                                       uproc_dentry_t* parent,
                                       size_t size, // maximum size of content the entry could hold, content wil be trauncated if exceeds this limit
                                       const char *v
                                       );

// wrapper for char *.
// Note: when writing to the entry, uproc honors the @size variable.
//  That is, the string pointed by @v must have at least @size bytes of memory.
//  Specifically, uproc overrides the content of the char arry
//  pointed by @v and puts '\0' at the end of the string.
uproc_dentry_t* uproc_create_entry_string(uproc_ctx_t *ctx,
                                           const char *name, // name of the entry
                                           mode_t mode,      // permissions
                                           uproc_dentry_t* parent,
                                           size_t size, // maximum size of content the entry could hold, content wil be trauncated if exceeds this limit
                                           int readonly,
                                           char *v
                                           );

#ifdef _UPROC_TEST
int uproc_errno();
#endif
#ifdef __cplusplus
}
#endif

#endif