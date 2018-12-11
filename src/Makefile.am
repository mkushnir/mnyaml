# consider headers:
#   - include_HEADERS
#   - nobase_include_HEADERS
#   - pkginclude_HEADERS
#   - dist_HEADERS
#   - nodist_HEADERS
#   - noinst_HEADERS

diags = diag.txt
BUILT_SOURCES = diag.c diag.h
EXTRA_DIST = $(diags)
CLEANFILES = $(BUILT_SOURCES) *.core core
#CLEANFILES += *.in
AM_MAKEFLAGS = -s

lib_LTLIBRARIES = libmrkyaml.la

nobase_include_HEADERS = mrkyaml.h

noinst_HEADERS =

libmrkyaml_la_SOURCES = mrkyaml.c
nodist_libmrkyaml_la_SOURCES = diag.c

DEBUG_LD_FLAGS =
if DEBUG
DEBUG_CC_FLAGS = -g -O0 @CC_DEBUG@
DEBUG_LD_FLAGS += -no-install
else
DEBUG_CC_FLAGS = -DNDEBUG -O3 -fomit-frame-pointer
if LTO
DEBUG_CC_FLAGS += @CC_LTO@
DEBUG_LD_FLAGS += @LD_LTO@
endif
endif

if ALLSTATIC
libmrkyaml_la_LDFLAGS = -all-static
else
libmrkyaml_la_LDFLAGS =
endif

libmrkyaml_la_CFLAGS = $(DEBUG_CC_FLAGS) -Wall -Wextra -Werror -std=c99 @_GNU_SOURCE_MACRO@ @_XOPEN_SOURCE_MACRO@ -I$(top_srcdir)/src -I$(top_srcdir) -I$(includedir)
libmrkyaml_la_LDFLAGS += $(DEBUG_LD_FLAGS) -version-info 0:0:0 -L$(libdir)
libmrkyaml_la_LIBADD = -lmrkcommon -lmndiag -lyaml

SUBDIRS = .

diag.c diag.h: $(diags)
	$(AM_V_GEN) cat $(diags) | sort -u >diag.txt.tmp && mndiagen -v -S diag.txt.tmp -L mrkyaml -H diag.h -C diag.c *.[ch]

run: all
	for i in $(bin_PROGRAMS); do if test -x ./$$i; then LD_LIBRARY_PATH=$(libdir) ./$$i; fi; done;

testrun:
	for i in $(SUBDIRS); do if test "$$i" != "."; then cd $$i && $(MAKE) testrun && cd ..; fi; done;
