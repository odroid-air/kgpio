# set(HAVE_BZIP2_SUPPORT ${BZIP2_FOUND})
# if(BZIP2_FOUND AND BZIP2_NEED_PREFIX)
#     set(NEED_BZ2_PREFIX 1)
# endif()
#
# set(HAVE_XZ_SUPPORT ${LIBLZMA_FOUND})
#
# configure_file(config-compression.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/config-compression.h)
# add_definitions(-DQT_NO_CAST_FROM_ASCII)
#
# if(BZIP2_FOUND)
#     include_directories(${BZIP2_INCLUDE_DIR})
#     set(kgpio_OPTIONAL_SRCS ${kgpio_OPTIONAL_SRCS} kbzip2filter.cpp)
#     set(kgpio_OPTIONAL_LIBS ${kgpio_OPTIONAL_LIBS} ${BZIP2_LIBRARIES})
# endif()
#
# if(LIBLZMA_FOUND)
#     include_directories(${LIBLZMA_INCLUDE_DIRS})
#     set(kgpio_OPTIONAL_SRCS ${kgpio_OPTIONAL_SRCS} kxzfilter.cpp k7zip.cpp)
#     set(kgpio_OPTIONAL_LIBS ${kgpio_OPTIONAL_LIBS} ${LIBLZMA_LIBRARIES})
# endif()


set(kgpio_SRCS
    kgpio.cpp
    gpiopin.cpp
    lightsensor.cpp
#     kar.cpp
#     kcompressiondevice.cpp
#     kfilterbase.cpp
#     kfilterdev.cpp
#     kgzipfilter.cpp
#     klimitediodevice.cpp
#     knonefilter.cpp
#     ktar.cpp
#     kzip.cpp
#     krcc.cpp
)

add_library(KF5Gpio ${kgpio_SRCS} ${kgpio_OPTIONAL_SRCS})
generate_export_header(KF5Gpio BASE_NAME KGpio)
add_library(KF5::Gpio ALIAS KF5Gpio)

target_include_directories(KF5Gpio
    INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/KGpio>"
)

target_link_libraries(KF5Gpio
    PUBLIC
        Qt5::Core
    PRIVATE
        ${kgpio_OPTIONAL_LIBS}
        ${ZLIB_LIBRARY}
)

set_target_properties(KF5Gpio PROPERTIES
    VERSION     ${KGPIO_VERSION_STRING}
    SOVERSION   ${KGPIO_SOVERSION}
    EXPORT_NAME "Gpio"
)

ecm_generate_headers(KGpio_HEADERS
    HEADER_NAMES
    KGpio
    GpioPin
    LightSensor
    REQUIRED_HEADERS KGpio_HEADERS
)

install(TARGETS KF5Gpio
        EXPORT KF5GpioTargets
        ${KF5_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES
            ${CMAKE_CURRENT_BINARY_DIR}/kgpio_export.h
            ${KGpio_HEADERS}
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KGpio
        COMPONENT Devel)

include(ECMGeneratePriFile)
ecm_generate_pri_file(BASE_NAME KGpio LIB_NAME KF5Gpio DEPS "core" FILENAME_VAR PRI_FILENAME INCLUDE_INSTALL_DIR ${KDE_INSTALL_INCLUDEDIR_KF5}/KGpio)
install(FILES ${PRI_FILENAME}
        DESTINATION ${ECM_MKSPECS_INSTALL_DIR})

add_subdirectory(import)

