set(SOURCE_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/soloud)

set(CORE_PATH ${SOURCE_PREFIX}/src/core)
set(CORE_SOURCES
	${CORE_PATH}/soloud.cpp
	${CORE_PATH}/soloud_audiosource.cpp
	${CORE_PATH}/soloud_bus.cpp
	${CORE_PATH}/soloud_core_3d.cpp
	${CORE_PATH}/soloud_core_basicops.cpp
	${CORE_PATH}/soloud_core_faderops.cpp
	${CORE_PATH}/soloud_core_filterops.cpp
	${CORE_PATH}/soloud_core_getters.cpp
	${CORE_PATH}/soloud_core_setters.cpp
	${CORE_PATH}/soloud_core_voicegroup.cpp
	${CORE_PATH}/soloud_core_voiceops.cpp
	${CORE_PATH}/soloud_fader.cpp
	${CORE_PATH}/soloud_fft.cpp
	${CORE_PATH}/soloud_fft_lut.cpp
	${CORE_PATH}/soloud_file.cpp
	${CORE_PATH}/soloud_filter.cpp
	${CORE_PATH}/soloud_misc.cpp
	${CORE_PATH}/soloud_queue.cpp
	${CORE_PATH}/soloud_thread.cpp
)

set(AUDIOSOURCES_PATH ${SOURCE_PREFIX}/src/audiosource)
set(AUDIOSOURCES_SOURCES
	${AUDIOSOURCES_PATH}/wav/dr_flac.h
	${AUDIOSOURCES_PATH}/wav/dr_impl.cpp
	${AUDIOSOURCES_PATH}/wav/dr_mp3.h
	${AUDIOSOURCES_PATH}/wav/dr_wav.h
	${AUDIOSOURCES_PATH}/wav/soloud_wav.cpp
	${AUDIOSOURCES_PATH}/wav/soloud_wavstream.cpp
	${AUDIOSOURCES_PATH}/wav/stb_vorbis.c
	${AUDIOSOURCES_PATH}/wav/stb_vorbis.h
)

set(FILTERS_PATH ${SOURCE_PREFIX}/src/filter)
set(FILTERS_SOURCES
	${FILTERS_PATH}/soloud_bassboostfilter.cpp
	${FILTERS_PATH}/soloud_biquadresonantfilter.cpp
	${FILTERS_PATH}/soloud_dcremovalfilter.cpp
	${FILTERS_PATH}/soloud_echofilter.cpp
	${FILTERS_PATH}/soloud_fftfilter.cpp
	${FILTERS_PATH}/soloud_flangerfilter.cpp
	${FILTERS_PATH}/soloud_freeverbfilter.cpp
	${FILTERS_PATH}/soloud_lofifilter.cpp
	${FILTERS_PATH}/soloud_robotizefilter.cpp
	${FILTERS_PATH}/soloud_waveshaperfilter.cpp
)

set(BACKENDS_PATH ${SOURCE_PREFIX}/src/backend)
set(BACKENDS_SOURCES
	${BACKENDS_PATH}/miniaudio/soloud_miniaudio.cpp
)

set(SOURCES
	${CORE_SOURCES}
	${AUDIOSOURCES_SOURCES}
	${FILTERS_SOURCES}
	${BACKENDS_SOURCES}
)

set(LINKS "")
set(DEFINITIONS "WITH_MINIAUDIO")

if (APPLE)
	set(LINKS
		"-framework CoreFoundation"
		"-framework AVFAudio"
		"-framework AudioToolbox"
		"-framework CoreAudio"
	)
	set(DEFINITIONS
		${DEFINITIONS}
		"MA_NO_RUNTIME_LINKING"
	)
endif()

if (GROWL_ANDROID)
	set(LINKS
		"-lOpenSLES"
	)
	set(DEFINITIONS
		${DEFINITIONS}
		"MA_NO_RUNTIME_LINKING"
		"MA_NO_AAUDIO"
	)
endif()

growl_thirdparty_lib(soloud
	SOURCES ${SOURCES}
	INCLUDES PUBLIC "${SOURCE_PREFIX}/include"
	DEFINITIONS ${DEFINITIONS}
	LINK ${LINKS}
)
