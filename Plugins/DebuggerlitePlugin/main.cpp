#include <QCoreApplication>
#include "StartPython.h"
#include <QProcess>
#include <qdebug.h>

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    StartPython* t= new StartPython("cHJpbnQoImhlbGxvd29ybGQiKQ==");
    t->onRead();

    return a.exec();
}
