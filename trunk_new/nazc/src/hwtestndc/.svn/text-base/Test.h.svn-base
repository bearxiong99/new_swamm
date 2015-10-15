
#ifndef __NDC_HW_TEST_H__
#define __NDC_HW_TEST_H__

#include <stdio.h>
#include <string.h>

typedef char* (*TestActFn)(const void*, const void*, const void*, const void*);

typedef struct
{
    const char* name;
    TestActFn   fn;
    const void* param[4];
} TEST_GROUP;

#endif  // __NDC_HW_TEST_H__
