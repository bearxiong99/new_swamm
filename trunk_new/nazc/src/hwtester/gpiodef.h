
#ifndef __GPIO_DEF_H__
#define __GPIO_DEF_H__

#define     GPIO_READ       0x01
#define     GPIO_WRITE      0x02

typedef struct _tagGPIOTBL
{
    const char    *name;
    int     port;
    int     type;
    int     value;
} GPIOTBL;

#endif
