TEMPLATE = app
QT += quick quickcontrols2 concurrent network multimedia
#QT += widgets
QT += charts
QT += core
CONFIG += c++17
QMAKE_CXXFLAGS_RELEASE += -O2
DEFINES += QT_DEPRECATED_WARNINGS QT_NO_WARNING_OUTPUT
HEADERS += \
    Algebra.h \
    ENC_SIGN.h \
    FFT.h \
    HISE.h \
    Random.h \
    Sampling.h \
    Scheme.h \
    aes.hpp \
    aes_ccm.h \
    aes_ctr_drbg.hpp \
    aes_ni.hpp \
    aes_storage.hpp \
    aes_sw.hpp \
    api.h \
    base64.h \
    convert.h \
    csprng.hpp \
    fileread.h \
    fips202.h \
    io.h \
    lest.hpp \
    params.h \
    phantom.hpp \
    phantom_machine.hpp \
    phantom_memory.hpp \
    phantom_types.hpp \
    randombytes.h \
    smtp.h

SOURCES += \
        Algebra.cc \
        ENC_SIGN.cpp \
        FFT.cc \
        HISE.cc \
        Random.cc \
        Sampling.cc \
        Scheme.cc \
        aes.cc \
        aes_ccm.cc \
        aes_ctr_drbg.cc \
        aes_ni.cc \
        aes_sw.cc \
        api.cc \
        base64.cpp \
        convert.cc \
        csprng.cc \
        fips202.cc \
        flieread.cpp \
        io.cc \
        main.cpp \
        randombytes.cc \
        smtp.cpp


RESOURCES += qml.qrc
include($$PWD/gsl/gsl.pri)
RC_ICONS = favicon1.ico
QML_IMPORT_PATH =
QML_DESIGNER_IMPORT_PATH =

CONFIG(debug,debug|release) {
    DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/../bin/debug)
} else {
    DESTDIR = $$absolute_path($${_PRO_FILE_PWD_}/../bin/release)
}

win32 {
contains(QT_ARCH, i386) {
    COPYDLL = $$absolute_path($${_PRO_FILE_PWD_}/../third/Win_x86/*.dll) $$DESTDIR
    QMAKE_PRE_LINK += $$QMAKE_COPY $$replace(COPYDLL, /, $$QMAKE_DIR_SEP)
} else {
    COPYDLL = $$absolute_path($${_PRO_FILE_PWD_}/../third/Win_x64/*.dll) $$DESTDIR
    QMAKE_PRE_LINK += $$QMAKE_COPY $$replace(COPYDLL, /, $$QMAKE_DIR_SEP)
}
}

qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

mac: {
    QMAKE_INFO_PLIST =
}
include($$PWD/gsl/gsl.pri)
include($$PWD/gsl2/gsl2.pri)
include($$PWD/gsl3/gsl3.pri)

DISTFILES += \
    gsl/gsl.pri \
    gsl2/gsl2.pri \
    gsl3/gsl3.pri


