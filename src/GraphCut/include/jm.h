#ifndef __JM_H
#define __JM_H

#include <mex.h>

#ifndef __USE_MISC
#ifndef __UINT
#define __UINT
typedef unsigned int uint;
#endif
#endif


#define JM_MALLOC  mxMalloc
#define JM_FREE    mxFree
#define JM_REALLOC mxRealloc

#define ASSERT(exp) mxAssert(exp, #exp);

#define MALLOC_INIT(ty, var, cnt) \
    ty *(var) = (ty *)JM_MALLOC( (cnt) * sizeof(var[0]) ); ASSERT(var); \
    mexMakeMemoryPersistent(var);

#define MALLOC_SET(ty, dest, cnt)                              \
    dest = (ty *)JM_MALLOC( (cnt) * sizeof(dest[0]) ); ASSERT(dest);    \
    mexMakeMemoryPersistent(dest);

#define REALLOC_SET(ty, dest, cnt)                                      \
    dest = (ty *)JM_REALLOC( dest, (cnt) * sizeof(dest[0]) );  ASSERT(dest); \
    mexMakeMemoryPersistent(dest);

#define MSG(msg,...) do { char buf[300]; snprintf(buf, sizeof(buf), __FILE__":%d(%s) " msg "\n", __LINE__, __FUNCTION__, ##__VA_ARGS__); mexPrintf(buf); } while (0)

#define SHOW(exp) do { char buf[300]; snprintf(buf, sizeof(buf), __FILE__":%d(%s) %s %d\n", __LINE__, __FUNCTION__, #exp, exp); mexPrintf(buf); } while (0)

#define MAT(m) mexPrintf("%-20s %d  (%08x  %08x)\n", #m, mxIsSharedArray(m), m, mxGetData(m));

#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#endif
