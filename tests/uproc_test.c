#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <uproc.h>

int main(int argc, char const *argv[]) {
    int ret;
    uproc_ctx_t uproc_ctx;
    
    ret = uproc_ctx_init(&uproc_ctx, "uproc", 1);
    if (ret) {
        fprintf(stderr, "failed to initialize uproc %s\n", strerror(ret));
    }

    uproc_dentry_t *ent1 = uproc_create_entry(&uproc_ctx, "var1", 0, 4096, NULL, NULL, NULL, NULL);
    assert(ent1);
    // existed
    assert(uproc_create_entry(&uproc_ctx, "var1", 0, 4096, NULL, NULL, NULL, NULL) == 0);
    uproc_dentry_t *dir  = uproc_mkdir(&uproc_ctx, "dir", NULL);
    assert(dir);
    uproc_dentry_t *ent2 = uproc_create_entry(&uproc_ctx, "var2", 0, 4096, dir, NULL, NULL, NULL);
    assert(ent2);

    uproc_dentry_t *ent3 = uproc_create_entry(&uproc_ctx, "/var3", 0700, 4096, dir, NULL, NULL, NULL);
    assert(ent3);
    uproc_dentry_t *ent4 = uproc_create_entry(&uproc_ctx, "/dir/var4", 0700, 4096, dir, NULL, NULL, NULL);
    assert(ent4);
    uproc_dentry_t *subdir = uproc_mkdir(&uproc_ctx, "/dir/subdir", NULL);
    assert(subdir);
    //uproc_loop(&uproc_ctx);
    return 0;
}