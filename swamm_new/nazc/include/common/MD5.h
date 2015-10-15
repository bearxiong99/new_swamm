#ifndef __COMMON_MD5_H__
#define __COMMON_MD5_H__

#ifdef _WIN32
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#endif

class CMD5  
{
	typedef struct 
    {
        UINT state[4];		/* state (ABCD) */
        UINT count[2];		/* number of bits, modulo 2^64 (lsb first) */
        BYTE buffer[64];	/* input buffer */
    } MD5_CTX;
 
public:
    CMD5();
    virtual ~CMD5();
    BOOL GetString(const char *pszSource, char *pszDest);
 
protected:
	void MD5Init(MD5_CTX* ctx);
	void MD5Update(MD5_CTX* ctx, const BYTE* input, UINT inputlen);
	void MD5Final(BYTE* digest, MD5_CTX* ctx);
};
 
#endif // __COMMON_MD5_H__
