remove_definitions(-DQT_NO_CAST_FROM_ASCII)

include(ECMAddTests)

find_package(Qt5Test ${REQUIRED_QT_VERSION} CONFIG QUIET)

if(NOT Qt5Test_FOUND)
    message(STATUS "Qt5Test not found, autotests will not be built.")
    return()
endif()

ecm_add_tests(
    kgpiotest.cpp
    LINK_LIBRARIES KF5::Gpio Qt5::Test
)

ecm_add_tests(
    testlightsensor.cpp
    LINK_LIBRARIES KF5::Gpio Qt5::Test
)

ecm_add_tests(
    testthermosensor.cpp
    LINK_LIBRARIES KF5::Gpio Qt5::Test
)

