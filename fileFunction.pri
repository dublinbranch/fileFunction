CONFIG += object_parallel_to_source

SOURCES += \
    $$PWD/apcu2.cpp \
    $$PWD/apcuTest.cpp \
    $$PWD/filefunction.cpp \
	$$PWD/folder.cpp \
	$$PWD/Qt-AES/qaesencryption.cpp

#non funziona
#include(Qt-AES/qaesencryption.pro)
    
HEADERS += \
	$$PWD/GeoLite2PP/GeoLite2PP.hpp \
	$$PWD/GeoLite2PP/GeoLite2PP_error_category.hpp \
	$$PWD/apcu2.h \
	$$PWD/ffCommon.h \
	$$PWD/filefunction.h \
	$$PWD/folder.h \
	$$PWD/mixin.h \
	$$PWD/serialize.h \
	$$PWD/Qt-AES/aesni/aesni-enc-cbc.h \
	$$PWD/Qt-AES/aesni/aesni-enc-ecb.h \
	$$PWD/Qt-AES/aesni/aesni-key-exp.h \
	$$PWD/Qt-AES/qaesencryption.h

# - SETUP -
# zypper in libzip-devel
# OR (should be equivalent)
# if compile error because not found "maxminddb.h" file (included in GeoLite2PP.hpp) then install "libmaxminddb-devel" package in YaST2
LIBS += -lzip
LIBS += -L'$$PWD/GeoLite2PP' -lgeolite2++
LIBS += -lmaxminddb

DISTFILES += \
	$$PWD/GeoLite2PP/README.md \
	$$PWD/GeoLite2PP/libgeolite2++.a
