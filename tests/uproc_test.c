#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include <uproc.h>

#include <pthread.h>

#include "uproc_test.h"

typedef void (*uproc_test_func_t)();

typedef struct{
    const char       *name;
    uproc_test_func_t f;
}uproc_test_t;

void test_uproc_ctx_init() {
    int ret;
    uproc_ctx_t uproc_ctx;

    ret = uproc_ctx_init(&uproc_ctx, "uproc", 1);

    ASSERT(!ret);

    uproc_destroy(&uproc_ctx);
}

void test_uproc_create_entries() {
    int ret;
    uproc_ctx_t uproc_ctx;
    uproc_dentry_t *ent1, *ent2, *ent3, *ent4, *subdir, *dir;
    ret = uproc_ctx_init(&uproc_ctx, "uproc", 1);
    if (ret) {
        fprintf(stderr, "failed to initialize uproc %s\n", strerror(ret));
    }

    ent1 = uproc_create_entry(&uproc_ctx, "var1", 0, 4096, NULL, NULL, NULL, NULL);
    ASSERT(ent1)
    // existed
    ASSERT_BOTH(uproc_create_entry(&uproc_ctx, "var1", 0, 4096, NULL, NULL, NULL, NULL) == 0, uproc_errno(), -EEXIST);
    dir  = uproc_mkdir(&uproc_ctx, "dir", NULL);
    ASSERT(dir);
    ent2 = uproc_create_entry(&uproc_ctx, "var2", 0, 4096, dir, NULL, NULL, NULL);
    ASSERT(ent2);

    ent3 = uproc_create_entry(&uproc_ctx, "/var3", 0700, 4096, dir, NULL, NULL, NULL);
    ASSERT(ent3);
    ent4 = uproc_create_entry(&uproc_ctx, "/dir/var4/DSA", 0700, 4096, dir, NULL, NULL, NULL);
    ASSERT_BOTH(ent4 == NULL, uproc_errno(), -ENOENT);
    subdir = uproc_mkdir(&uproc_ctx, "/dir/subdir", NULL);
    ASSERT(subdir);

    uproc_destroy(&uproc_ctx);
}

uproc_ctx_t global_ctx;
void* uproc_thread(void*data) {
    // uproc_run() implies uproc_destroy()
    uproc_run(&global_ctx);

    return NULL;
}


int global_var1 = 100;
const char * global_const_str = "global_const_str";
char global_str[100] = "global_str[100]";

int read_str_from_file(const char *filename, char *buf, int size) {
    FILE *fp = fopen(filename, "r");
    int nread;

    if (fp == NULL) {
        return -1;
    }
    
    nread = read(fileno(fp), buf, size - 1);
    
    if (nread > 0) {
        buf[nread] = '\0';
    }
    fclose(fp);
    
    return nread;
}

int write_str_to_file(const char *filename, char *buf, int size) {
    FILE *fp = fopen(filename, "w");
    int nwritten;

    if (fp == NULL) {
        return -1;
    }
    
    nwritten = write(fileno(fp), buf, size);
    
    fclose(fp);
    
    return nwritten;
}

void test_uproc_general() {
    int ret, n, x;
    char buf[1024];
    pthread_t tid;
    uproc_dentry_t *ent1, *ent2, *ent3;

    ret = uproc_ctx_init(&global_ctx, "uproc", 1);

    ASSERT(!ret);
    ent1 = uproc_create_entry_int(&global_ctx, "global_var1",
                          S_IRUSR | S_IWUSR,
                          /* set root as the parent*/ NULL,
                          /* readonly ?*/ 0,
                          /* pointer to the variable */ &global_var1);
    ASSERT(ent1);

    ent2 = uproc_create_entry_cstring(&global_ctx, "global_const_str",
                          /* default permission */0,
                          /* set root as the parent*/ NULL,
                          /* size of the entry */ strlen(global_const_str),
                          /* pointer to the variable */ global_const_str);
    ASSERT(ent2)
    ent3 = uproc_create_entry_string(&global_ctx, "global_str",
                          /* default permission */S_IWUSR | S_IRUSR | S_IRGRP | S_IROTH,
                          /* set root as the parent*/ NULL,
                          /* size of the entry */ sizeof(global_str),
                          /* readonly ? */ 0,
                          /* pointer to the variable */ global_str);
    ASSERT(ent3);

    if (pthread_create(&tid, NULL, uproc_thread, NULL)) {
        exit(1);
    }
    // get uproc ready
    sleep(1);
    
    { //test case for global_var1
        n = read_str_from_file("uproc/global_var1", buf, sizeof(buf));
        if (n < 0) {
            ASSERT(0);
        }
        x = atoi(buf);
        ASSERT(x == 100);

        n = snprintf(buf, sizeof(buf), "%d", 456);
        n = write_str_to_file("uproc/global_var1", buf, n);
        if (n < 0){
            ASSERT(0);
        }

        n = read_str_from_file("uproc/global_var1", buf, sizeof(buf));
        if (n < 0) {
            ASSERT(0);
        }
        x = atoi(buf);
        ASSERT(x == 456);
    }

    { //test case for global_const_str
        n = read_str_from_file("uproc/global_const_str", buf, sizeof(buf));
        if (n < 0) {
            ASSERT(0);
        }
        ASSERT(!strcmp(buf, "global_const_str\n"));
    }

    { //test case for global_str
        n = read_str_from_file("uproc/global_str", buf, sizeof(buf));
        if (n < 0) {
            ASSERT(0);
        }
        ASSERT(!strcmp(buf, "global_str[100]\n"));

        n = snprintf(buf, sizeof(buf), "%s", "new_string");
        n = write_str_to_file("uproc/global_str", buf, n);
        if (n < 0){
            ASSERT(0);
        }

        n = read_str_from_file("uproc/global_str", buf, sizeof(buf));
        if (n < 0) {
            ASSERT(0);
        }
        x = atoi(buf);
        ASSERT(strcmp(buf, "new_string"));
    }

    uproc_exit(&global_ctx);
    pthread_join(tid, NULL);
}

uproc_test_t tests[] = {
    {"test_uproc_ctx_init", test_uproc_ctx_init},
    {"test_uproc_create_entries", test_uproc_create_entries},
    {"test_uproc_general", test_uproc_general},
    {"NULL", NULL}
};

int main(int argc, char const *argv[]) {
    int i;
    for (i = 0; tests[i].name && tests[i].f; ++i) {
        fprintf(stderr, "------------running test: %s------------\n\n", tests[i].name);
        tests[i].f();
        fprintf(stderr, "\n------------test %s passed------------\n\n", tests[i].name);
    }
    return 0;
}