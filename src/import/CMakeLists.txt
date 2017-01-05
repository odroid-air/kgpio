set(kgpioplugin_SRCS
    kgpioplugin.cpp
    kgpiocontroller.cpp
    )

ecm_qt_declare_logging_category(kgpioplugin_SRCS HEADER debug.h
                                               IDENTIFIER WMD
                                               CATEGORY_NAME kde.kgpio
                                               DEFAULT_SEVERITY Info)

add_library(kgpioplugin SHARED ${kgpioplugin_SRCS})

target_link_libraries(kgpioplugin
        Qt5::Quick
        Qt5::Qml
        KF5::Gpio)


install(TARGETS kgpioplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kgpio)

install(DIRECTORY qml/ DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/kgpio)
