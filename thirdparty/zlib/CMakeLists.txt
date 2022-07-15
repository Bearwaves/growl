cmake_minimum_required (VERSION 3.19)

set(SOURCES
	"adler32.c"
	"compress.c"
	"crc32.c"
	"deflate.c"
	"infback.c"
	"inffast.c"
	"inflate.c"
	"inftrees.c"
	"trees.c"
	"uncompr.c"
	"zutil.c"
	)

set(HEADERS
	"crc32.h"
	"deflate.h"
	"inffast.h"
	"inffixed.h"
	"inflate.h"
	"inftrees.h"
	"trees.h"
	"zconf.h"
	"zlib.h"
	"zutil.h"
	)

add_library(zlib ${SOURCES} ${HEADERS})
target_include_directories(zlib PUBLIC ${CMAKE_SOURCE_DIR})
