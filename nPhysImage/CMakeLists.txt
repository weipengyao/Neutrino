# this is the frigging nPhysImage cmakelists
cmake_minimum_required (VERSION 3.9)

add_library (nPhysImageF SHARED
    NaNStream.h
    anymap.cc
    anymap.h
    bidimvec.h
    mcomplex.h
    nPhysFormats.cc
    nPhysFormats.h
    nPhysImageF.h
    nPhysMaths.cc
    nPhysMaths.h
    nPhysWave.cc
    nPhysWave.h
    tools.h
    unwrap/unwrap.h unwrap/goldstein.cc unwrap/quality.cc unwrap/simple.cc
    )

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/../resources/cmake)
include(FindNeutrinoDeps)

target_link_libraries(nPhysImageF m ${LIBS})

# install
if (WIN32)
    install (TARGETS nPhysImageF COMPONENT binary DESTINATION "bin")
elseif(LINUX)
    install (TARGETS nPhysImageF COMPONENT binary DESTINATION lib)
elseif (APPLE)
    install (TARGETS nPhysImageF COMPONENT binary DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/../Neutrino.app/Contents/Frameworks")
endif()


