#include "FileControllVirtual.h"

FileControllVirtual::FileControllVirtual(const QString &ip, QObject *parent):
    FileControll(ip,parent)
{

}

FileControllVirtual::~FileControllVirtual()
{

}

QString FileControllVirtual::joinPath(const QStringList &allPath)
{
    if (allPath.isEmpty()) return "";
    QStringList lstPath(allPath);
#ifdef __arm__
    Q_UNUSED(ip)
    QString path("/dobot/userdata");
#else
    QString path = lstPath.takeFirst();
#endif
    foreach(QString str, lstPath)
    {
        if (!str.startsWith("/")) {
            str = "/" + str;
        }
        path.append(str);
    }
    return path;
}
