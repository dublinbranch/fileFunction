CONFIG += object_parallel_to_source

SOURCES += \
    $$PWD/filefunction.cpp \
    $$PWD/folder.cpp
    
HEADERS += \
	$$PWD/ffCommon.h \
	$$PWD/filefunction.h \
	$$PWD/folder.h \
	$$PWD/serialize.h

#zypper in libzip-devel
LIBS += -lzip
