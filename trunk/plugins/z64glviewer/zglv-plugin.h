/****************************
* Z64 Viewer Plugin Wrapper *
****************************/
#ifndef Z64VIEWER_PLUGIN_H
#define Z64VIEWER_PLUGIN_H

#define GZRT_FUNC __functions

#define CALLOC(x)	((GZRT_FUNC)->calloc(x))

#define U64(x) ( \
	(u64)(x)[0] << 56 | \
	(u64)(x)[1] << 48 | \
	(u64)(x)[2] << 40 | \
	(u64)(x)[3] << 32 | \
	(u64)(x)[4] << 24 | \
	(u64)(x)[5] << 16 | \
	(u64)(x)[6] << 8  | \
	(u64)(x)[7] )

typedef unsigned long long u64;
typedef unsigned long u32;

#define DEBUG(fmt, ...) \
	((GZRT_FUNC)->debug(__FILE__,__LINE__,fmt "\n",##__VA_ARGS__))
#define NOTICE(h, t) \
	((GZRT_FUNC)->notice(h, t))
#define PLUGIN_DATA_FREE(t) \
	((GZRT_FUNC)->close((void*)t))

#endif
