#include <allocator_test.h>

static void basic_test(void)
{
    void *a = alloc(16);
    void *b = alloc(64);
    void *c = alloc(4096);

    ASSERT(a != NULL, "basic: a");
    ASSERT(b != NULL, "basic: b");
    ASSERT(c != NULL, "basic: c");

    hal_print("[PASS] basic_test\n",0x00ff00,1);
}

static void pattern_test(void)
{
    const size_t sizes[] =
    {
        16,64,123,4096,8192
    };

    const uint8_t patterns[] =
    {
        0x11,0x22,0x33,0x44,0x55
    };

    void *ptrs[5];

    for(int i=0;i<5;i++)
    {
        ptrs[i]=alloc(sizes[i]);
        ASSERT(ptrs[i]!=NULL,"pattern alloc");

        memset(ptrs[i],patterns[i],sizes[i]);
    }

    for(int i=0;i<5;i++)
    {
        uint8_t *p=ptrs[i];

        for(size_t j=0;j<sizes[i];j++)
            ASSERT(p[j]==patterns[i],"pattern verify");
    }

    hal_print("[PASS] pattern_test\n",0x00ff00,1);
}


static void overlap_test(void)
{
    void *ptrs[128];

    for(int i=0;i<128;i++)
        ptrs[i]=alloc(64);

    for(int i=0;i<128;i++)
    {
        uintptr_t a=(uintptr_t)ptrs[i];

        for(int j=i+1;j<128;j++)
        {
            uintptr_t b=(uintptr_t)ptrs[j];

            ASSERT(a!=b,"duplicate pointer");

            ASSERT(a+64<=b || b+64<=a,
                   "memory overlap");
        }
    }

    hal_print("[PASS] overlap_test\n",0x00ff00,1);
}


static void alignment_test(void)
{
    for(int i=0;i<100;i++)
    {
        void *p=alloc(i+1);

        ASSERT((((uintptr_t)p)&7)==0,
               "bad alignment");
    }

    hal_print("[PASS] alignment_test\n",0x00ff00,1);
}

static void random_sizes_test(void)
{
    const size_t sizes[] =
    {
        1,2,3,5,7,13,17,
        31,63,64,65,
        127,128,129,
        255,256,257,
        511,512,513,
        1023,1024,
        2047,2048,
        4095,4096,4097,
        8191,8192,8193
    };

    for(size_t i=0;i<sizeof(sizes)/sizeof(sizes[0]);i++)
    {
        void *p=alloc(sizes[i]);

        ASSERT(p!=NULL,"random alloc");

        memset(p,0xAA,sizes[i]);

        uint8_t *x=p;

        for(size_t j=0;j<sizes[i];j++)
            ASSERT(x[j]==0xAA,
                   "random verify");
    }

    hal_print("[PASS] random_sizes_test\n",0x00ff00,1);
}


static void huge_alloc_test(void)
{
    void *a=alloc(1024*1024);
    ASSERT(a!=NULL,"1MB");

    memset(a,0x55,1024*1024);

    uint8_t *p=a;

    for(size_t i=0;i<1024*1024;i++)
        ASSERT(p[i]==0x55,"1MB verify");

    hal_print("[PASS] huge_alloc_test\n",0x00ff00,1);
}

static void stress_test(void)
{
    enum { COUNT = 1000 };

    void *ptrs[COUNT];
    uint16_t sizes[COUNT];

    for(int i=0;i<COUNT;i++)
    {
        sizes[i]=(i*37)%5000+1;

        ptrs[i]=alloc(sizes[i]);

        ASSERT(ptrs[i]!=NULL,"stress alloc");

        memset(ptrs[i],(uint8_t)i,sizes[i]);
    }

    for(int i=0;i<COUNT;i++)
    {
        uint8_t *p=ptrs[i];

        for(size_t j=0;j<sizes[i];j++)
            ASSERT(p[j]==(uint8_t)i,
                   "stress corruption");
    }

    hal_print("[PASS] stress_test\n",0x00ff00,1);
}



void allocator_test(void)
{
    hal_print("\n========== Allocator Tests ==========\n",
              0x00ff00, 1);

    basic_test();
    pattern_test();
    overlap_test();
    alignment_test();
    random_sizes_test();
    stress_test();
    huge_alloc_test();

    hal_print("\nALL TESTS PASSED\n",
              0x00ff00, 2);
}