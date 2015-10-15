#ifndef __KETI_FRAME_HEADER__
#define __KETI_FRAME_HEADER__

#include "typedef.h"

#ifdef _WIN32
#pragma pack(push, 1)
#endif	/* _WIN32 */
typedef struct
{
    BYTE        scid[5];
    TIMESTAMP   sct;
    WORD        ct;
    WORD        ch;
    WORD        cb;
    BYTE        period;
    BYTE        tu;
    BYTE        hu;
    BYTE        bu;
}	__ATTRIBUTE_PACKED__ KETI_TEMP_ST;

typedef struct
{
    WORD        cx;
    WORD        cy;
    WORD        cz;
    WORD        cg;
}	__ATTRIBUTE_PACKED__ KETI_ACC_LP_ST;

typedef struct
{
    BYTE        scid[5];
    TIMESTAMP   sct;
    WORD        cx;
    WORD        cy;
    WORD        cz;
    WORD        cg;
    WORD        cb;
    BYTE        period;
    BYTE        bcnt;
    TIMESTAMP   lt;
    BYTE        lcnt;
    KETI_ACC_LP_ST lp[0];
}	__ATTRIBUTE_PACKED__ KETI_ACC_ST;


typedef struct
{
    BYTE        scid[5];
    TIMESTAMP   sct;
    UINT        ctv;
    UINT        ci;
    UINT        ca;
    UINT        cp;
    BYTE        period;
    BYTE        tvu;
    BYTE        iu;
    BYTE        au;
    BYTE        pu;
}	__ATTRIBUTE_PACKED__ KETI_FLOW_ST;

typedef struct
{
    BYTE        scid[5];
}	__ATTRIBUTE_PACKED__ KETI_HDR_ST;

#ifdef _WIN32
#pragma pack(pop)
#endif	/* _WIN32 */

#endif
