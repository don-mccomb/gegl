
# The setup is as follows:
#  * Each test comes with one <name>.xml GEGL XML and one reference
#    output file in 'reference' called <name>.<ext>, where <ext> is a
#    supported output file type.
#  * The test is run by processing the GEGL XML into an output file in
#    'output', named <name>.<ext>, and then compared with the file in
#    'reference'. If these files are equal, the test is a 'pass'.


SUBDIRS=data

EXTRA_DIST=$(wildcard $(srcdir)/*.xml) $(wildcard $(srcdir)/reference/*) run-compositions.py

TESTS = \
  affine-with-negative.xml        \
  alien-map.xml                   \
  apply-lens.xml                  \
  apply-lens2.xml                 \
  apply-lens3.xml                 \
  checkerboard.xml                \
  clones.xml                      \
  colors.xml                      \
  color-to-alpha.xml              \
  composite-transform.xml         \
  contrast-curve.xml              \
  edge-laplace.xml                \
  edge-sobel.xml                  \
  fattal02.xml                    \
	gegl.xml                        \
  gamma.xml                       \
  hdr-color.xml                   \
  image-compare.xml               \
  mantiuk06.xml                   \
  noise-cell.xml        	  			\
  noise-hurl.xml        	  			\
  noise-simplex.xml        	  		\
  pixelize.xml                    \
  posterize.xml                   \
  red-eye-removal.xml             \
  reflect.xml                     \
  reflect2.xml                    \
  reinhard05.xml                  \
  rgb-params.xml                  \
  rotate.xml                      \
  simple-scale.xml                \
  softglow.xml                    \
  stretch-contrast.xml            \
  transform.xml                   \
  weighted-blend.xml              \
  rectangles.xml

  #run-matting-global.xml.sh
  #run-matting-global-big.xml.sh

NO_OPENCL_TESTS = \
  color-reduction.xml           \
  rgbe-load.xml                 \
  rgbe-save.xml                 \
  shift.xml                     \
  stretch-contrast-hsv.xml      \
  pnm-raw-load.xml              \
  pnm-ascii-load.xml            \
  tile.xml

# Conditional tests
if HAVE_JASPER
NO_OPENCL_TESTS += jp2-load.xml
endif
if HAVE_UMFPACK
TESTS += matting-levin.xml
endif

check-TESTS: $(TESTS)
	$(PYTHON) $(srcdir)/run-compositions.py \
	  --build-dir=$(top_builddir) --src-dir=$(top_srcdir) --xml-dir=$(srcdir) \
	  $(TESTS)
	$(PYTHON) $(srcdir)/run-compositions.py --without-opencl \
	  --build-dir=$(top_builddir) --src-dir=$(top_srcdir) --xml-dir=$(srcdir) \
	  $(NO_OPENCL_TESTS)

clean-local:
	rm -f output/*
