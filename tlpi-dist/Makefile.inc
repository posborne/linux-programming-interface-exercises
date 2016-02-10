# Makefile.inc - common definitions used by all makefiles

TLPI_DIR = ..
TLPI_LIB = ${TLPI_DIR}/libtlpi.a
TLPI_INCL_DIR = ${TLPI_DIR}/lib

LINUX_LIBRT = -lrt
LINUX_LIBDL = -ldl
LINUX_LIBACL = -lacl
LINUX_LIBCRYPT = -lcrypt
LINUX_LIBCAP = -lcap

# "-Wextra" is a more descriptive synonym for "-W", but only
# available in more recent gcc versions

# Defining _DEFAULT_SOURCE is a workaround to avoid the warnings that
# would otherwise be produced when compiling code that defines _BSD_SOURCE
# or _SVID_SOURCE against glibc headers in version 2.20 and later.
# (The alternative would be to replace each instance of "#define _SVID_SOURCE"
# or "#define _BSD_SOURCE" in the example programs with
# "#define _DEFAULT_SOURCE".)

IMPL_CFLAGS = -std=c99 -D_XOPEN_SOURCE=600 \
	  -D_DEFAULT_SOURCE \
		-g -I${TLPI_INCL_DIR} \
		-pedantic \
		-Wall \
		-W \
    -Wpointer-arith \
		-Wmissing-prototypes \
		-Wno-sign-compare \
		-Wno-unused-parameter \
    -Wno-format-y2k \
    -Wno-long-long \
		-lrt       #for time.h

CFLAGS = ${IMPL_CFLAGS}

IMPL_THREAD_FLAGS = -pthread

IMPL_LDLIBS = ${TLPI_LIB} -lm

LDLIBS = ${IMPL_LDLIBS}

RM = rm -f

#<<predefined implicit rules >>
#<1>.Compliling C program:
#"n.o" is made automatically from "n.c" with a recipe of the form
#"$(CC) $(CPPFLAGS) $(CFLAGS) -c"

#<2>.Compliling C++ program:
#"n.o" is made automatically from "n.cc, c.cpp or n.C"  with a recipe of the form
#"$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(CFLAGS) -c"
# We encourage you to use the suffix ".cc" for C++ source file instead of ".C"

#<3>.Linking a single object file
#n is made automatically from n.o. The precise recipe used is 
#"$(CC) $(LDFLAGS) n.o $(LOADLIBSES) $(LDLIBS) "
