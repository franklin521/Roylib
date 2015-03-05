#ifndef BLOOM_H
#define BLOOM_H

#ifdef __cplusplus
extern "C"
{
#endif

    typedef unsigned int (*hashfunc_t)(const void * key, int len, unsigned int seed);
    typedef struct {
        size_t asize;
        unsigned char *a;
        size_t nfuncs;
        hashfunc_t hashfunc;
    } BLOOM;

    BLOOM *bloom_create(size_t size, size_t nfuncs, ...);
    int bloom_destroy(BLOOM *bloom);
    int bloom_add(BLOOM *bloom, const char *s);
    int bloom_check(BLOOM *bloom, const char *s);
    unsigned int MurmurHash2 ( const void * key, int len, unsigned int seed );
#ifdef __cplusplus
}
#endif

#endif
