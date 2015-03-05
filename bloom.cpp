/*
 * bloom.cpp
 *
 *  Created on: 2014年5月26日
 *      Author: Roy
 */

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/time.h>
#include "bloom.h"

#define SETBIT(a, n) (a[n/CHAR_BIT] |= (1<<(n%CHAR_BIT)))
#define GETBIT(a, n) (a[n/CHAR_BIT] & (1<<(n%CHAR_BIT)))

BLOOM *bloom_create(size_t size, size_t nfuncs, ...)
{
    BLOOM *bloom;
//    va_list l;
//    int n;

    if (!(bloom=(BLOOM *)malloc(sizeof(BLOOM)))) return NULL;
    bloom->nfuncs=nfuncs;
    bloom->hashfunc = MurmurHash2;
    bloom->asize= size * nfuncs *2;

    if (!(bloom->a=(unsigned char *)calloc((bloom->asize + CHAR_BIT-1)/CHAR_BIT, sizeof(char)))) {
        free(bloom);
        return NULL;
    }
//    if(!(bloom->funcs=(hashfunc_t*)malloc(nfuncs*sizeof(hashfunc_t)))) {
//        free(bloom->a);
//        free(bloom);
//        return NULL;
//    }

//    va_start(l, nfuncs);
//    for(n=0; n<nfuncs; ++n) {
//        bloom->funcs[n]=va_arg(l, hashfunc_t);
//    }
//    va_end(l);

    return bloom;
}

int bloom_destroy(BLOOM *bloom)
{
    if (bloom == NULL)
    {
        return 0;
    }
    free(bloom->a);
//    free(bloom->funcs);
    free(bloom);

    return 0;
}

int bloom_add(BLOOM *bloom, const char *s)
{
    size_t n;
    if (bloom == NULL)
    {
        return -1;
    }

    for (n=0; n<bloom->nfuncs; ++n) {
        SETBIT(bloom->a, bloom->hashfunc(s, strlen(s), n+1) % bloom->asize);
//        SETBIT(bloom->a, bloom->funcs[n](s)%bloom->asize);
    }

    return 0;
}

int bloom_check(BLOOM *bloom, const char *s)
{
    size_t n;
    if (bloom == NULL || s == NULL)
    {
        return -1;
    }
    for (n=0; n<bloom->nfuncs; ++n) {
        if (!(GETBIT(bloom->a, bloom->hashfunc(s, strlen(s), n+1)%bloom->asize))) return 0;
    }

    return 1;
}

unsigned int sax_hash(const char *key)
{
    unsigned int h=0;

    while (*key) h^=(h<<5)+(h>>2)+(unsigned char)*key++;

    return h;
}

unsigned int sdbm_hash(const char *key)
{
    unsigned int h=0;
    while (*key) h=(unsigned char)*key++ + (h<<6) + (h<<16) - h;
    return h;
}

//-----------------------------------------------------------------------------
// MurmurHash2, by Austin Appleby

// Note - This code makes a few assumptions about how your machine behaves -

// 1. We can read a 4-byte value from any address without crashing
// 2. sizeof(int) == 4

// And it has a few limitations -

// 1. It will not work incrementally.
// 2. It will not produce the same results on little-endian and big-endian
//    machines.

unsigned int MurmurHash2 ( const void * key, int len, unsigned int seed )
{
    // 'm' and 'r' are mixing constants generated offline.
    // They're not really 'magic', they just happen to work well.

    const unsigned int m = 0x5bd1e995;
    const int r = 24;

    // Initialize the hash to a 'random' value

    unsigned int h = seed ^ len;

    // Mix 4 bytes at a time into the hash

    const unsigned char * data = (const unsigned char *)key;

    while (len >= 4)
    {
        unsigned int k = *(unsigned int *)data;

        k *= m;
        k ^= k >> r;
        k *= m;

        h *= m;
        h ^= k;

        data += 4;
        len -= 4;
    }

    // Handle the last few bytes of the input array
    switch (len)
    {
    case 3:
        h ^= data[2] << 16;
    case 2:
        h ^= data[1] << 8;
    case 1:
        h ^= data[0];
        h *= m;
    };

    // Do a few final mixes of the hash to ensure the last few
    // bytes are well-incorporated.

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;

    return h;
}
#if 0
int main(int argc, char *argv[])
{
    FILE *fp;
    char line[1024];
    char *p;
    BLOOM *bloom;

    if (argc<2) {
        fprintf(stderr, "ERROR: No word file specified\n");
        return EXIT_FAILURE;
    }

//    if(!(bloom=bloom_create(2500000, 2, sax_hash, sdbm_hash))) {
    if (!(bloom=bloom_create(125000000, 8))) {
        fprintf(stderr, "ERROR: Could not create bloom filter\n");
        return EXIT_FAILURE;
    }

    if (!(fp=fopen(argv[1], "r"))) {
        fprintf(stderr, "ERROR: Could not open file %s\n", argv[1]);
        return EXIT_FAILURE;
    }

    struct timeval tp;
    gettimeofday(&tp, NULL);
    printf("begin create bloom ----%ld  %ld\n", tp.tv_sec, tp.tv_usec);

    while (fgets(line, 1024, fp)) {
        if ((p=strchr(line, '\r'))) *p='\0';
        if ((p=strchr(line, '\n'))) *p='\0';
        bloom_add(bloom, line);

        p = line + strlen(line);
        for (int i = 1; i < 50; i++) {
            *p='\0';
            char append[20];
            sprintf(append, "test%d", i);
            strcat(line, append);
            bloom_add(bloom, line);
        }
    }
    gettimeofday(&tp, NULL);
    printf("end create bloom ----%ld  %ld", tp.tv_sec, tp.tv_usec);
    fclose(fp);

    if (!(fp=fopen(argv[2], "r"))) {
        fprintf(stderr, "ERROR: Could not open query file %s\n", argv[2]);
        return EXIT_FAILURE;
    }

    gettimeofday(&tp, NULL);
    printf("begin query bloom ----%ld  %ld\n", tp.tv_sec, tp.tv_usec);

    int total = 0, find = 0, nofind = 0;
    while (fgets(line, 1024, fp)) {
        if ((p=strchr(line, '\r'))) *p='\0';
        if ((p=strchr(line, '\n'))) *p='\0';

//        p=strtok(line, " \t,.;:\r\n?!-/()");
        total++;
//        while(p) {
        if (!bloom_check(bloom, line)) {
//            printf("No match for ford \"%s\"\n", p);
            nofind++;
        } else {
            find++;
        }

        total++;
        strcat(line, "test50");
        if (!bloom_check(bloom, line)) {
//            printf("No match for ford \"%s\"\n", p);
            nofind++;
        } else {
            find++;
        }
//            p=strtok(NULL, " \t,.;:\r\n?!-/()");
//        }
    }

    gettimeofday(&tp, NULL);
    printf("end query bloom ----%ld  %ld\n", tp.tv_sec, tp.tv_usec);
    printf("query result: total:%d, find:%d, nofind %d\n", total, find, nofind);

    bloom_destroy(bloom);

    return EXIT_SUCCESS;
}
#endif


