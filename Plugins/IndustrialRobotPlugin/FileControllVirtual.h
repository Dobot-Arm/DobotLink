#ifndef FILECONTROLLVIRTUAL_H
#define FILECONTROLLVIRTUAL_H

#include "FileControll.h"

class FileControllVirtual : public FileControll
{
    Q_OBJECT
public:
    FileControllVirtual(const QString &ip, QObject *parent = nullptr);
    ~FileControllVirtual();

    // FileControll interface
protected:
    QString joinPath(const QStringList &allPath) Q_DECL_OVERRIDE;
};

#endif // FILECONTROLLVIRTUAL_H
