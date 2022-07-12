CONFIG += object_parallel_to_source

SOURCES += \
    $$PWD/UaDecoder.cpp \
    $$PWD/apcu2.cpp \
    $$PWD/apcuTest.cpp \
    $$PWD/b64.cpp \
    $$PWD/crc.cpp \
    $$PWD/filefunction.cpp \
	$$PWD/folder.cpp \
	$$PWD/Qt-AES/qaesencryption.cpp \
    $$PWD/qDateTimeUtil.cpp \
    $$PWD/randutil.cpp \
    $$PWD/snowflake.cpp \
	$$PWD/sourcelocation.cpp \
	$$PWD/localev2.cpp \
    $$PWD/threadstatush.cpp

HEADERS += \
	$$PWD/BaseCache.h \
	$$PWD/GeoLite2PP/GeoLite2PP.hpp \
	$$PWD/GeoLite2PP/GeoLite2PP_error_category.hpp \
	$$PWD/UaDecoder.h \
	$$PWD/apcu2.h \
	$$PWD/b64.h \
	$$PWD/crc.h \
	$$PWD/ffCommon.h \
	$$PWD/filefunction.h \
	$$PWD/folder.h \
	$$PWD/mixin.h \
	$$PWD/randutil.h \
	$$PWD/resetAfterUse.h \
	$$PWD/serialize.h \
	$$PWD/Qt-AES/aesni/aesni-enc-cbc.h \
	$$PWD/Qt-AES/aesni/aesni-enc-ecb.h \
	$$PWD/Qt-AES/aesni/aesni-key-exp.h \
	$$PWD/Qt-AES/qaesencryption.h \
	$$PWD/snowflake.h \
	$$PWD/sourcelocation.h \
	$$PWD/stringDefine.h \
	$$PWD/localev2.h \
	$$PWD/threadstatush.h
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
