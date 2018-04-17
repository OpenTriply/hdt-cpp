bin_PROGRAMS = HDTgeneration

AM_DEFAULT_SOURCE_EXT = .cpp

AM_CPPFLAGS = -I@top_srcdir@/libhdt/include $(WARN_CFLAGS) $(EXTRAFLAGS)
AM_LDFLAGS = $(SERD_LIBS) $(ZLIB_LIBS) $(KYOTO_LIBS)
LDADD = ../../libhdt.la

if WANTS_LIBCDS
AM_CPPFLAGS += -DHAVE_CDS -I$(top_builddir)/libcds/include
LDADD += $(top_builddir)/libcds/libcds.la
endif
