CLEANFILES = $(BUILT_SOURCES) *.core core
#CLEANFILES += *.in
AM_MAKEFLAGS = -s

# consider headers:
#   - include_HEADERS
#   - nobase_include_HEADERS
#   - pkginclude_HEADERS
#   - dist_HEADERS
#   - nodist_HEADERS
#   - noinst_HEADERS

noinst_PROGRAMS=testfoo

diags = ../src/diag.txt diag.txt
BUILT_SOURCES = diag.c diag.h
EXTRA_DIST = diag.txt runscripts

noinst_HEADERS = unittest.h ../src/mrkyaml.h

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
testfoo_LDFLAGS = -all-static
else
testfoo_LDFLAGS =
endif

nodist_testfoo_SOURCES = diag.c
testfoo_SOURCES = testfoo.c
testfoo_CFLAGS = $(DEBUG_FLAGS) -Wall -Wextra -Werror -std=c99 @_GNU_SOURCE_MACRO@ @_XOPEN_SOURCE_MACRO@ -I$(top_srcdir)/test -I$(top_srcdir)/src -I$(top_srcdir) -I$(includedir)
testfoo_LDFLAGS += $(DEBUG_LD_FLAGS) -L$(top_srcdir)/src/.libs -L$(libdir)
testfoo_LDADD = -lmrkyaml -lmrkcommon -lmndiag

diag.c diag.h: $(diags)
	$(AM_V_GEN) cat $(diags) | sort -u >diag.txt.tmp && mndiagen -v -S diag.txt.tmp -L mrkyaml -H diag.h -C diag.c ../*.[ch] ./*.[ch]

testrun: all
	for i in $(noinst_PROGRAMS); do if test -x ./$$i; then LD_LIBRARY_PATH=$(libdir) ./$$i; fi; done;
