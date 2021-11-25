CONFIG += object_parallel_to_source

SOURCES += \
    $$PWD/apcu2.cpp \
    $$PWD/apcuTest.cpp \
    $$PWD/filefunction.cpp \
    $$PWD/folder.cpp
    
HEADERS += \
	$$PWD/apcu2.h \
	$$PWD/ffCommon.h \
	$$PWD/filefunction.h \
	$$PWD/folder.h \
	$$PWD/serialize.h

#zypper in libzip-devel
LIBS += -lzip
LIBS += -L'$$PWD/GeoLite2PP' -lgeolite2++
LIBS += -lmaxminddb
