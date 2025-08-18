QT += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Administrador.cpp \
    Estandar.cpp \
    Usuario.cpp \
    album.cpp \
    albumdetailwindow.cpp \
    albumwidget.cpp \
    artistawindow.cpp \
    biblioteca.cpp \
    cancion.cpp \
    cuentas.cpp \
    homewindow.cpp \
    main.cpp \
    mainwindow.cpp \
    playlist.cpp \
    songwidget.cpp

HEADERS += \
    Administrador.h \
    Estandar.h \
    Usuario.h \
    album.h \
    albumdetailwindow.h \
    albumwidget.h \
    artistawindow.h \
    biblioteca.h \
    cancion.h \
    cuentas.h \
    homewindow.h \
    mainwindow.h \
    playlist.h \
    songwidget.h

FORMS += \
    artistawindow.ui \
    homewindow.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    imgs.qrc
