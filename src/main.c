#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <uproc.h>

int var1;
const char * var2;
int uproc_read_var1(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    size_t n = snprintf(buf->mem, buf->size, "%d\n", var1++) + 1;
    if (n < buf->size) {
        *done = 1;
    }
    return n;
}

int uproc_read_var2(uproc_buf_t *buf, int *done, off_t fileoff, void *private_data) {
    size_t n = snprintf(buf->mem, buf->size, "%s\n", var2) + 1;
    if (n < buf->size) {
        *done = 1;
    }
    return n;
}

int var3;

int main(int argc, char const *argv[]) {
    int ret;
    var1 = 123;
    var2 = "das";
    var3 = 445;
    uproc_ctx_t uproc_ctx;
    
    ret = uproc_ctx_init(&uproc_ctx, "uproc", 1);
    if (ret) {
        fprintf(stderr, "failed to initialize uproc %s\n", strerror(ret));
    }

    uproc_dentry_t *ent1 = uproc_create_entry(&uproc_ctx, "var1", 0, 4096, NULL, uproc_read_var1, NULL, NULL);
    uproc_dentry_t *ent2 = uproc_create_entry(&uproc_ctx, "var2", 0, 4096, NULL, uproc_read_var2, NULL, NULL);
    uproc_dentry_t *ent3 = uproc_create_entry_int(&uproc_ctx, "var3", 0, NULL, 0, &var3);
    assert(ent1);
    assert(ent2);
    assert(ent3);
    uproc_run(&uproc_ctx);
    return 0;
}