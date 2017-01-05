cmake_minimum_required(VERSION 2.8.12)

project(KGpio)

include(FeatureSummary)
find_package(ECM 5.28.0  NO_MODULE)
set_package_properties(ECM PROPERTIES TYPE REQUIRED DESCRIPTION "Extra CMake Modules." URL "https://projects.kde.org/projects/kdesupport/extra-cmake-modules")
feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)


set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(KDECMakeSettings)


include(GenerateExportHeader)

set(REQUIRED_QT_VERSION 5.5.0)
#find_package(Qt5Core ${REQUIRED_QT_VERSION} REQUIRED NO_MODULE)

find_package(Qt5 ${REQUIRED_QT_VERSION} CONFIG REQUIRED Core Qml Quick Test)

include(ECMSetupVersion)
include(ECMGenerateHeaders)
include(ECMQtDeclareLoggingCategory)

set(KF5_VERSION "5.29.0") # handled by release scripts

ecm_setup_version(${KF5_VERSION}
    VARIABLE_PREFIX KARCHIVE
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/kgpio_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KF5GpioConfigVersion.cmake"
    SOVERSION 5)

add_subdirectory(src)
add_subdirectory(autotests)
#add_subdirectory(tests)


# create a Config.cmake and a ConfigVersion.cmake file and install them
set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KF5Gpio")

include(ECMPackageConfigHelpers)

ecm_configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KF5GpioConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KF5GpioConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/kgpio_version.h
        DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}
        COMPONENT Devel)

install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/KF5GpioConfig.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/KF5GpioConfigVersion.cmake"
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        COMPONENT Devel)

install(EXPORT KF5GpioTargets
        DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
        FILE KF5GpioTargets.cmake
        NAMESPACE KF5::)

feature_summary(WHAT ALL   FATAL_ON_MISSING_REQUIRED_PACKAGES)
