#ifndef BASECACHE_H
#define BASECACHE_H

#include <QString>

class BaseCache{
public:
	virtual QString getInfo() const = 0;
};

#endif // BASECACHE_H
