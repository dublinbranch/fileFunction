#ifndef FOLDER_H
#define FOLDER_H

#include <QString>

bool    mkdir(const QString& dirName);
void    cleanFolder(const QString& folder);
QString getMostRecent(const QString pathDir, const QString& filter);

#endif // FOLDER_H
