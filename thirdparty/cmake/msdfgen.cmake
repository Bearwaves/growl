set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/msdfgen)
set(SOURCES
	"${SOURCE_PREFIX}/core/msdfgen.cpp"
	"${SOURCE_PREFIX}/core/Shape.cpp"
	"${SOURCE_PREFIX}/core/EdgeHolder.cpp"
	"${SOURCE_PREFIX}/core/sdf-error-estimation.cpp"
	"${SOURCE_PREFIX}/core/SignedDistance.cpp"
	"${SOURCE_PREFIX}/core/render-sdf.cpp"
	"${SOURCE_PREFIX}/core/Contour.cpp"
	"${SOURCE_PREFIX}/core/Projection.cpp"
	"${SOURCE_PREFIX}/core/msdf-error-correction.cpp"
	"${SOURCE_PREFIX}/core/edge-coloring.cpp"
	"${SOURCE_PREFIX}/core/equation-solver.cpp"
	"${SOURCE_PREFIX}/core/shape-description.cpp"
	"${SOURCE_PREFIX}/core/MSDFErrorCorrection.cpp"
	"${SOURCE_PREFIX}/core/contour-combiners.cpp"
	"${SOURCE_PREFIX}/core/Scanline.cpp"
	"${SOURCE_PREFIX}/core/edge-selectors.cpp"
	"${SOURCE_PREFIX}/core/Vector2.cpp"
	"${SOURCE_PREFIX}/core/edge-segments.cpp"
	"${SOURCE_PREFIX}/core/rasterization.cpp"
	"${SOURCE_PREFIX}/ext/import-font.cpp"
	)

growl_thirdparty_lib(msdfgen
	SOURCES ${SOURCES}
	INCLUDES PUBLIC ${SOURCE_PREFIX}
	LINK growl-thirdparty::freetype
	)
