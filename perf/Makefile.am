
noinst_PROGRAMS = \
	test-blur \
	test-bcontrast \
	test-bcontrast-minichunk \
	test-unsharpmask \
	test-bcontrast-4x \
	test-gegl-buffer-access \
	test-samplers \
	test-rotate

INCLUDES = \
	-I$(top_srcdir)/ \
	-I$(top_srcdir)/gegl/ \
	-I$(top_srcdir)/gegl/buffer \
	-I$(top_srcdir)/gegl/operation \
	-I$(top_srcdir)/gegl/graph \
	-I$(top_srcdir)/gegl/process \
	-I$(top_srcdir)/gegl/property-types \
	-I$(top_builddir)/gegl

common_ldadd = $(top_builddir)/gegl/libgegl-@GEGL_API_VERSION@.la

LDADD = $(common_ldadd) $(DEP_LIBS) $(BABL_LIBS) -lm

AM_CFLAGS = \
	$(DEP_CFLAGS) $(BABL_CFLAGS)\
	-DG_DISABLE_SINGLE_INCLUDES \
	-DGLIB_DISABLE_DEPRECATION_WARNINGS \
	-DCLUTTER_DISABLE_DEPRECATION_WARNINGS \
	-DTESTS_DATA_DIR=\""$(top_srcdir)/tests/data/"\"

perf-report: check

check:
	for a in $(noinst_PROGRAMS);do GEGL_PATH=../operations ./$$a;done;true

test_rotate_SOURCES = test-rotate.c
test_blur_SOURCES = test-blur.c
test_bcontrast_SOURCES = test-bcontrast.c
test_bcontrast_minichunk_SOURCES = test-bcontrast-minichunk.c
test_bcontrast_4x_SOURCES = test-bcontrast-4x.c
test_unsharpmask_SOURCES = test-unsharpmask.c
test_gegl_buffer_access_SOURCES = test-gegl-buffer-access.c
test_samplers_SOURCES = test-samplers.c

EXTRA_DIST = Makefile-retrospect Makefile-tests create-report.rb test-common.h


