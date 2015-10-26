#ifndef _UPROC_TEST_H_
#define _UPROC_TEST_H_
#define UPORC_MAKESTR(X) #X
#define UPROC_ASSERT_ERRN(errn, exp_errn, lineno)\
    if ((errn) != (exp_errn)) {\
        fprintf(stderr, "errno %d != expected errno %d, in file %s, function %s, at line %d\n",\
                (errn), (exp_errn), __FILE__, __func__, lineno);\
        exit(1);\
    }

#define UPROC_ASSERT(e, lineno)\
    if (!(e)) {\
        fprintf(stderr, "assertion `%s' failed, in file %s, function %s, at line %d\n",\
                UPORC_MAKESTR(e), __FILE__, __func__, lineno);\
        exit(1);\
    }

#define UPROC_ASSERT_BOTH(e, errn, exp_errn, lineno)\
    UPROC_ASSERT(e, lineno) \
    UPROC_ASSERT_ERRN(errn, exp_errn, lineno)

#define ASSERT(e) UPROC_ASSERT(e, __LINE__)

#define ASSERT_ERRN(errn, exp_errn) UPROC_ASSERT_ERRN(errn, exp_errn, __LINE__)

#define ASSERT_BOTH(e, errn, exp_errn) UPROC_ASSERT_BOTH(e, errn, exp_errn, __LINE__)

#endif