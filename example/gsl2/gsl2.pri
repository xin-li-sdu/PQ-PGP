DEPENDPATH += $$PWD/

INCLUDEPATH += $$PWD/include

#LIBS +=-L$$PWD/
LIBS += -L$$PWD/ -llibcrypto
\
-llibssl
\
-lcipi
