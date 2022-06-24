cmake_minimum_required(VERSION 3.1)

project(soloud VERSION 20200207 LANGUAGES CXX)

if (MSVC)
	set(CMAKE_CXX_FLAGS "/W0")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set(CMAKE_CXX_FLAGS "-Wno-everything")
else ()
	set(CMAKE_CXX_FLAGS "-Wno-all")
endif ()

set(HEADER_PATH "include")
set(SOURCE_PATH "src")

set(HEADERS
	${HEADER_PATH}/soloud.h
	${HEADER_PATH}/soloud_audiosource.h
	${HEADER_PATH}/soloud_bassboostfilter.h
	${HEADER_PATH}/soloud_biquadresonantfilter.h
	${HEADER_PATH}/soloud_bus.h
	${HEADER_PATH}/soloud_dcremovalfilter.h
	${HEADER_PATH}/soloud_echofilter.h
	${HEADER_PATH}/soloud_error.h
	${HEADER_PATH}/soloud_fader.h
	${HEADER_PATH}/soloud_fft.h
	${HEADER_PATH}/soloud_fftfilter.h
	${HEADER_PATH}/soloud_file.h
	${HEADER_PATH}/soloud_file_hack_off.h
	${HEADER_PATH}/soloud_file_hack_on.h
	${HEADER_PATH}/soloud_filter.h
	${HEADER_PATH}/soloud_flangerfilter.h
	${HEADER_PATH}/soloud_freeverbfilter.h
	${HEADER_PATH}/soloud_internal.h
	${HEADER_PATH}/soloud_lofifilter.h
	${HEADER_PATH}/soloud_misc.h
	${HEADER_PATH}/soloud_monotone.h
	${HEADER_PATH}/soloud_noise.h
	${HEADER_PATH}/soloud_openmpt.h
	${HEADER_PATH}/soloud_queue.h
	${HEADER_PATH}/soloud_robotizefilter.h
	${HEADER_PATH}/soloud_sfxr.h
	${HEADER_PATH}/soloud_speech.h
	${HEADER_PATH}/soloud_tedsid.h
	${HEADER_PATH}/soloud_thread.h
	${HEADER_PATH}/soloud_vic.h
	${HEADER_PATH}/soloud_vizsn.h
	${HEADER_PATH}/soloud_wav.h
	${HEADER_PATH}/soloud_waveshaperfilter.h
	${HEADER_PATH}/soloud_wavstream.h
	)

set(CORE_PATH ${SOURCE_PATH}/core)
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

set(AUDIOSOURCES_PATH ${SOURCE_PATH}/audiosource)
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

set(FILTERS_PATH ${SOURCE_PATH}/filter)
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

set(BACKENDS_PATH ${SOURCE_PATH}/backend)
set(BACKENDS_SOURCES
	${BACKENDS_PATH}/miniaudio/soloud_miniaudio.cpp
	)

set(SOURCES
	${CORE_SOURCES}
	${AUDIOSOURCES_SOURCES}
	${FILTERS_SOURCES}
	${BACKENDS_SOURCES}
	)

add_library(soloud ${SOURCES} ${HEADERS})
target_include_directories(soloud PUBLIC ${HEADER_PATH})
target_compile_definitions(soloud PRIVATE "WITH_MINIAUDIO")
