#include <errno.h>
#include <string.h>
#include <stdio.h>

#include <uproc.h>


#define FUSE_USE_VERSION 26
#include <fuse.h>

#define _UPROC_LOAD_FACTOR 0.75

#define S_IRWXUGO   (S_IRWXU|S_IRWXG|S_IRWXO)
#define S_IALLUGO   (S_ISUID|S_ISGID|S_ISVTX|S_IRWXUGO)
#define S_IRUGO     (S_IRUSR|S_IRGRP|S_IROTH)
#define S_IWUGO     (S_IWUSR|S_IWGRP|S_IWOTH)
#define S_IXUGO     (S_IXUSR|S_IXGRP|S_IXOTH)


#ifdef _UPROC_TEST
static int _uproc_errno;
int uproc_errno() {
    return _uproc_errno;
}
#define _SET_UPROC_ERRNO(n) if ((n) < 0) _uproc_errno = (n);
#else
#define _SET_UPROC_ERRNO(n)
#endif

/* Jenkins' one-at-a-time hash function */
static 
unsigned __uproc_dentry_hash(uproc_dentry_t *parent, const char *key, size_t len) {
    unsigned hash, i;

    for(hash = i = 0; i < len; ++i) {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    for (i = 0; i < sizeof(uproc_dentry_t*); ++i) {
        hash += (char)((long)parent & (255 << (i * 8)));
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}

static unsigned uproc_dentry_hash(struct hlist_node* p) {
    uproc_dentry_t *entry = hlist_entry(p, uproc_dentry_t, hlink);
    
    return __uproc_dentry_hash(entry->parent, entry->name, entry->namelen);
}

static 
unsigned uproc_dentry_equal(struct hlist_node* p, void *d1, void*d2, void*d3) {
    uproc_dentry_t *parent = (uproc_dentry_t*)d1;
    const char *name = (const char *)d2;
    size_t namelen = (size_t)d3;

    uproc_dentry_t *entry = hlist_entry(p, uproc_dentry_t, hlink);

    return entry->parent == parent &&
           namelen == entry->namelen &&
           !strncmp(name, entry->name, namelen);
}

/* 
* find the range in @str delimited by @c
* After returned, @p points to the first occurrence of @c, 
* @next points to the second occurrence of @c.
* if @c is not found in @str, 0 is stored in @p and next points
* to a null byte at the end of @str.
*/
static inline void __range_strchr(const char *str, const char **p, const char **next, char c) {
    *p = strchr(str, c);
    if (!*p) {
        *next = str + strlen(str); /* point to the null byte */
        return;
    }
    *next = strchr(*p + 1, c);
    if (!*next)
        *next = *p + strlen(*p); /* point to the null byte */
}

/*
* search for the last part of the path which does not contain '/'.
* @ret should store the parent of the path @name, otherwise
* the search will start at the root.
* After returned, *lp stores the start of the last part,
* *ret stores the parent dir. 
* return -ENOENT if the path does not exist.
*/
static int __find_last_part(uproc_ctx_t *ctx, const char *name,
                            uproc_dentry_t **ret, const char **lp) {
    const char *p, *pnext;
    const char *prev = name;
    uproc_dentry_t *parent = NULL, *entry = NULL;
    struct hlist_node *n;
    size_t namelen;
    parent = *ret;
    if (!parent || *prev == '/')
        parent = ctx->root;
    
    entry = parent;
    // skip over front '/'s
    while (*prev && *prev == '/')
        ++prev;

    if (prev > name) {// point @prev to the last '/'
        *lp = prev--;
    } else {
        *lp = prev;    
    }

    for (;*prev;) {
        __range_strchr(prev, &p, &pnext, '/');
        if (!p) {
            *lp = prev;
            break;
        } else if (!*pnext) {
            *lp = p + 1;
            break;
        }

        ++p;
        namelen = pnext - p;

        n = uproc_htable_find(&ctx->htable, __uproc_dentry_hash(parent, p, namelen),
                          (void*)parent, (void*)p, (void*)namelen);
        if (!n) {
            _SET_UPROC_ERRNO(-ENOENT);
            return -ENOENT;
        }
        entry = hlist_entry(n, uproc_dentry_t, hlink);
        prev = p;
        parent = entry;
    }

    *ret = parent;
    _SET_UPROC_ERRNO(-0);
    return 0;
}

static int __uproc_lookup(uproc_ctx_t *ctx, const char *name,
                          uproc_dentry_t **pentry) {
    uproc_dentry_t *parent;
    struct hlist_node *n;
    size_t namelen;
    int ret = 0;
    const char *p;

    if (!name || !strlen(name)) {
        _SET_UPROC_ERRNO(-EINVAL);
        return -EINVAL;
    }
    if ((ret = __find_last_part(ctx, name, &parent, &p))) {
        return ret;
    }

    namelen = strlen(p);

    if (namelen == 0) {
        if (parent == ctx->root){
            *pentry = ctx->root;
            return 0;
        } else {
            /* /xxx/yyy/ */
            *pentry = parent;
            return 0;
        }
    }

    n = uproc_htable_find(&ctx->htable, __uproc_dentry_hash(parent, p, namelen),
                          (void*)parent, (void*)p, (void*)namelen);

    if (!n) {
        _SET_UPROC_ERRNO(-ENOENT);
        return -ENOENT;
    }

    *pentry = hlist_entry(n, uproc_dentry_t, hlink);

    _SET_UPROC_ERRNO(-0);
    return ret;
}

static uproc_dentry_t* __uproc_create(uproc_ctx_t *ctx,
                                      const char *name,
                                      mode_t mode,
                                      uproc_dentry_t **parent) {
    const char *lp;
    uproc_dentry_t *new_entry = NULL;
    size_t namelen;
    if (!name || !strlen(name)) {
        if (ctx->dbg)
            fprintf(stderr, "uproc: empty pathname!\n");
        goto out;
    }

    if (__find_last_part(ctx, name, parent, &lp)) {
        if (ctx->dbg)
            fprintf(stderr, "uproc: some parts of \"%s\" does not exist!\n", name);
        goto out;
    }

    if (!*lp) {
        if (ctx->dbg)
            fprintf(stderr, "uproc: invalid pathname \"%s\"\n", name);
        goto out;
    }

    namelen = strlen(lp);
    new_entry = malloc(sizeof(*new_entry) + namelen + 1);
    if (!new_entry) {
        if (ctx->dbg)
            fprintf(stderr, "uproc: memory shortage, can't allocate memory for entry \"%s\"\n", name);
        goto out;
    }

    memset(new_entry, 0, sizeof(*new_entry) + namelen + 1);
    new_entry->name = (char *)new_entry + sizeof(*new_entry);
    memcpy(new_entry->name, lp, namelen);
    new_entry->namelen = namelen;
    new_entry->uid = getuid();
    new_entry->gid = getgid();
    new_entry->mode = mode;
    INIT_HLIST_NODE(&new_entry->hlink);
out:
    return new_entry;
}

static int __uproc_register(uproc_ctx_t *ctx,
                            uproc_dentry_t *entry,
                            uproc_dentry_t *parent) {
    int ret = 0;
    if (!parent)
        parent = ctx->root;

    if ((ret = uproc_htable_insert(&ctx->htable, &entry->hlink,
                __uproc_dentry_hash(parent, entry->name, entry->namelen), 
                (void*)parent, (void*)entry->name, (void*)entry->namelen))) {
        _SET_UPROC_ERRNO(ret);
        return ret;
    }

    entry->parent = parent;
    entry->next = parent->children;
    parent->children = entry;

    _SET_UPROC_ERRNO(-0);
    return ret;
}

int uproc_ctx_init(uproc_ctx_t *ctx, const char *mount_point, int dbg) {
    int ret = 0;

    ctx->root = malloc(sizeof(uproc_dentry_t) + strlen("/") + 1);
    if (!ctx->root) {
        _SET_UPROC_ERRNO(-ENOMEM);
        return -ENOMEM;
    }

    if ((ret = uproc_htable_init(&ctx->htable, _UPROC_LOAD_FACTOR,
                                uproc_dentry_hash, uproc_dentry_equal))) {
        free(ctx->root);
        _SET_UPROC_ERRNO(ret);
        return ret;
    }

    ctx->dbg = dbg;
    ctx->mount_point = mount_point;
    memset(ctx->root, 0, sizeof(*ctx->root));
    ctx->root->namelen = 1;
    ctx->root->name = "/";
    ctx->root->parent = NULL;
    ctx->root->children = NULL;
    ctx->root->next = NULL;
    ctx->root->mode = S_IFDIR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
    INIT_HLIST_NODE(&ctx->root->hlink);

    uproc_htable_insert(&ctx->htable, &ctx->root->hlink,
                        uproc_dentry_hash(&ctx->root->hlink),
                        (void*)ctx->root, (void*)ctx->root->name, (void*)ctx->root->namelen);
    _SET_UPROC_ERRNO(-0);
    return 0;
}


// recursively release resources of the tree rooted at @r
void __uproc_destroy_dentry(uproc_dentry_t *r) {
    uproc_dentry_t *p;
    for (p = r->children; p; p = p->next) {
        __uproc_destroy_dentry(p);
    }
    free(r);
}

void uproc_destroy(uproc_ctx_t *ctx) {
    uproc_dentry_t *p;
    if (!ctx)
        return;
    for (p = ctx->root->children; p; p = p->next) {
        __uproc_destroy_dentry(p);
    }
    uproc_htable_free(&ctx->htable);
}

uproc_dentry_t* uproc_mkdir_mode(uproc_ctx_t *ctx,
                                 const char *name,
                                 mode_t mode,
                                 uproc_dentry_t *parent) {
    int ret;
    uproc_dentry_t *ent;

    if (!ctx)
        return NULL;
    // clear file types other than dir
    mode &= ~S_IFMT;
    // if permission bits are 0, set default to read permission for owner, group and other.
    if ((mode & S_IALLUGO) == 0) { 
        mode |= S_IRUGO;
    }
    mode |= S_IFDIR;

    ent = __uproc_create(ctx, name, mode, &parent);
    if (ent && (ret = __uproc_register(ctx, ent, parent))) {
        if (ctx->dbg)
            fprintf(stderr, "uproc: failed to register \"%s\" to uproc, reason: %s\n", name, strerror(-ret));
        free(ent);
        ent = NULL;
    }

    return ent;
}

uproc_dentry_t* uproc_mkdir(uproc_ctx_t *ctx,
                                 const char *name,
                                 uproc_dentry_t *parent) {
    return uproc_mkdir_mode(ctx, name, S_IRUGO | S_IXUGO, parent);
}

uproc_dentry_t* uproc_create_entry(uproc_ctx_t *ctx,
                                   const char *name,
                                   mode_t mode,
                                   size_t size,
                                   uproc_dentry_t* parent,
                                   uproc_read_proc_t read_proc,
                                   uproc_write_proc_t write_proc,
                                   void *private_data) {
    int ret;
    uproc_dentry_t *ent;

    if (!ctx)
        return NULL;
    // clear file types other than regular file
    mode &= ~S_IFMT;
    if ((mode & S_IALLUGO) == 0) { 
        mode |= S_IRUGO;
    }
    mode |= S_IFREG;

    ent = __uproc_create(ctx, name, mode, &parent);
    if (ent) {
        ent->read_proc = read_proc;
        ent->write_proc = write_proc;
        ent->private_data = private_data;
        ent->size = size;
        if ((ret = __uproc_register(ctx, ent, parent))) {
            if (ctx->dbg)
                fprintf(stderr, "uproc: failed to register \"%s\" to uproc, reason: %s\n", name, strerror(-ret));

            free(ent);
            ent = NULL;
        }
    }

    return ent;
}

static int uproc_opendir(const char *path, struct fuse_file_info *fi) {
    struct fuse_context* fctx = fuse_get_context();
    uproc_ctx_t * ctx = (uproc_ctx_t *)fctx->private_data;
    uproc_dentry_t *ent;
    int ret = __uproc_lookup(ctx, path, &ent);
    if (ret) {
        if (ctx->dbg)
            fprintf(stderr, "\"%s\" does not exist\n", path);
        return ret;
    }

    if (!S_ISDIR(ent->mode)) {
        _SET_UPROC_ERRNO(-ENOTDIR);
        return -ENOTDIR;
    }

    uproc_buf_t *b = malloc(sizeof(uproc_buf_t));
    if (!b) {
        _SET_UPROC_ERRNO(-ENOMEM);
        return -ENOMEM;
    }
    memset(b, 0, sizeof(*b));
    b->entry = ent;

    /* TODO: permission checks */
    fi->fh = (uint64_t)b;
    fi->nonseekable = 1;

    _SET_UPROC_ERRNO(-0);
    return 0;
}


static int uproc_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
             off_t offset, struct fuse_file_info *fi)
{
    uproc_buf_t    *b = (uproc_buf_t*)fi->fh;;
    uproc_dentry_t *entry, *p;

    if (!b) {
        _SET_UPROC_ERRNO(-EINVAL);
        return -EINVAL;
    }
    entry = b->entry;
    if (!entry) {
        _SET_UPROC_ERRNO(-EINVAL);
        return -EINVAL;
    }

    if (!S_ISDIR(entry->mode)) {
        _SET_UPROC_ERRNO(-ENOTDIR);
        return -ENOTDIR;
    }

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    for (p = entry->children; p; p = p->next) {
        filler(buf, p->name, NULL, 0);
    }

    _SET_UPROC_ERRNO(-0);
    return 0;
}

static int uproc_getattr(const char *path, struct stat *stbuf)
{
    int ret = 0;

    struct fuse_context* fctx = fuse_get_context();
    uproc_ctx_t * ctx = (uproc_ctx_t*)fctx->private_data;
    uproc_dentry_t *ent;
    ret = __uproc_lookup(ctx, path, &ent);

    if (ret) {
        if (ctx->dbg)
            fprintf(stderr, "\"%s\" does not exist\n", path);
        _SET_UPROC_ERRNO(ret);
        return ret;
    }

    memset(stbuf, 0, sizeof(struct stat));

    if (S_ISDIR(ent->mode)) {
        stbuf->st_nlink = 2;
    } else {
        stbuf->st_nlink = 1;
        stbuf->st_size = ent->size;
    }

    stbuf->st_mode = ent->mode;
    stbuf->st_uid = ent->uid;
    stbuf->st_gid = ent->gid;
    stbuf->st_atime = time(NULL);
    stbuf->st_ctime = stbuf->st_atime;
    _SET_UPROC_ERRNO(-0);
    return ret;
}

static int uproc_open(const char *path, struct fuse_file_info *fi)
{
    struct fuse_context* fctx = fuse_get_context();
    uproc_ctx_t * ctx = (uproc_ctx_t*)fctx->private_data;
    uproc_dentry_t *ent;
    int ret = __uproc_lookup(ctx, path, &ent);

    if (ret) {
        if (ctx->dbg)
            fprintf(stderr, "\"%s\" does not exist\n", path);
        _SET_UPROC_ERRNO(ret);
        return ret;
    }

    uproc_buf_t *b = malloc(sizeof(uproc_buf_t));
    if (!b)
        return -ENOMEM;
    memset(b, 0, sizeof(*b));
    b->entry = ent;

    /* TODO: permission checks */
    fi->fh = (uint64_t)b;
    fi->nonseekable = 1;

    _SET_UPROC_ERRNO(-0);
    return 0;
}

int uproc_release(const char *path, struct fuse_file_info *fi) {
    uproc_buf_t *b = (uproc_buf_t*)fi->fh;
    if (b) {
        free(b);
        fprintf(stderr, "uproc_release: released buffer %p\n", b);
    }

    return 0;
}


int uproc_releasedir(const char *path, struct fuse_file_info *fi) {
    uproc_buf_t *b = (uproc_buf_t*)fi->fh;
    if (b) {
        free(b);
        fprintf(stderr, "uproc_releasedir: released buffer %p\n", b);
    }
    return 0;
}

static int uproc_read(const char *path, char *buf, size_t size, off_t offset,
              struct fuse_file_info *fi)
{
    uproc_buf_t    *b = (uproc_buf_t*)fi->fh;;
    uproc_dentry_t *entry;
    int nread = 0;

    if (!b) {
        _SET_UPROC_ERRNO(-EINVAL);
        return -EINVAL;
    }
    entry = b->entry;
    if (!entry) {
        _SET_UPROC_ERRNO(-EINVAL);
        return -EINVAL;
    }

    if (S_ISDIR(entry->mode)) {
        _SET_UPROC_ERRNO(-EISDIR);
        return -EISDIR;
    }

    if (!entry->read_proc) {
        _SET_UPROC_ERRNO(-ENOSYS);
        return -ENOSYS;
    }

    if (offset < entry->size && entry->read_proc && !b->done) {
        if (offset + size > entry->size)
            size = entry->size - offset;
        b->mem = buf;
        b->size = size;
        nread = entry->read_proc(b, &b->done, offset, entry->private_data);
    }

    // careful, @nread might be a error number
    if (nread > 0 && nread > entry->size)
        nread = entry->size;

    _SET_UPROC_ERRNO(nread);
    return nread;
}

static int uproc_write(const char * path, const char *buf, size_t size, off_t offset,
                       struct fuse_file_info *fi) {
    uproc_buf_t    *b = (uproc_buf_t*)fi->fh;;
    uproc_dentry_t *entry;
    int written = 0; // bytes written by 

    if (!b) {
        _SET_UPROC_ERRNO(-EINVAL);
        return -EINVAL;
    }
    entry = b->entry;
    if (!entry) {
        _SET_UPROC_ERRNO(-EINVAL);
        return -EINVAL;
    }

    if (S_ISDIR(entry->mode)) {
        _SET_UPROC_ERRNO(-EISDIR);
        return -EISDIR;
    }

    if (!entry->write_proc) {
        _SET_UPROC_ERRNO(-ENOSYS);
        return -ENOSYS;
    }

    if (entry->write_proc && !b->done) {
        b->mem = (char*)buf;
        b->size = size;
        written = entry->write_proc(b, &b->done, offset, entry->private_data);
    } else {
        return size;
    }

    // careful, @written might be a error number
    if (written > 0 && written > entry->size)
        written = entry->size;
    _SET_UPROC_ERRNO(written);
    return written;
}

int uproc_truncate(const char *path, off_t offset) {
    return 0;
}

static uproc_ctx_t  *uproc_instance;
static char *argv[] = {"uproc", "-s", "-d", "./uproc"};
static int argc = sizeof(argv) / sizeof(char *);

static void* uproc_init(struct fuse_conn_info *conn) {
    return (void*)uproc_instance;
}

static struct fuse_operations uproc_ops = {
    .init       = uproc_init,
    .getattr    = uproc_getattr,
    .opendir    = uproc_opendir,
    .readdir    = uproc_readdir,
    .open       = uproc_open,
    .release    = uproc_release,
    .releasedir = uproc_releasedir,
    .read       = uproc_read,
    .write      = uproc_write,
    .truncate   = uproc_truncate,
};

int uproc_run(uproc_ctx_t *ctx) {
    struct fuse *fuse;
    char *mountpoint = (char*)ctx->mount_point;
    int multithreaded = 0;
    int res;

    argv[3] = (char*)ctx->mount_point;
    uproc_instance = ctx;

    fuse = fuse_setup(argc, argv, &uproc_ops, sizeof(uproc_ops), &mountpoint,
                 &multithreaded, NULL);
    if (fuse == NULL)
        return -1;

    ctx->fuse = fuse;
    res = fuse_loop(fuse);

    fuse_teardown(fuse, mountpoint);

    uproc_destroy(ctx);

    return res;
}

/* 
* Tells uproc to exit.
*/
void uproc_exit(uproc_ctx_t *ctx) {
    fuse_exit(ctx->fuse);
}