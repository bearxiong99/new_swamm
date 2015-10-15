#ifndef __MEMORY_DEBUG_H__
#define __MEMORY_DEBUG_H__

#ifdef __DEBUG_MEMORY__

#define MALLOC(x)                   debugMALLOC(__FILE__, __LINE__, x)
#define REALLOC(x,y)                debugREALLOC(__FILE__, __LINE__, x, y)
#define FREE(x)                     debugFREE(__FILE__, __LINE__, x)

#else

#define MALLOC(x)					malloc(x)
#define REALLOC(a,b)				realloc(a,b)
#define FREE(x)						free(x)

#endif	// __DEBUG_MEMORY__

#define CheckPoint(x)				debugCheckPoint(x)
#define Difference(x)				debugDifference(x)

void debugCheckPoint(unsigned int *nIndex);
void debugDifference(unsigned int nCheckPoint);

void *debugMALLOC(const char *file, int linenum, size_t size);
void *debugREALLOC(const char *file, int linenum, void *memblock, size_t size);
void debugFREE(const char *file, int linenum, void *memblock);

#endif  // __MEMORY_DEBUG_H__
