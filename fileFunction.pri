CONFIG += object_parallel_to_source

SOURCES += \
    $$PWD/apcu2.cpp \
    $$PWD/apcuinclude.cpp \
    $$PWD/filefunction.cpp \
    $$PWD/folder.cpp
    
HEADERS += \
	$$PWD/apcu2.h \
	$$PWD/apcuinclude.h \
	$$PWD/ffCommon.h \
	$$PWD/filefunction.h \
	$$PWD/folder.h \
	$$PWD/serialize.h

#zypper in libzip-devel
LIBS += -lzip
