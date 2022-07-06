#ifndef TEST_INDUSTRIALROBOT_FILECONTROLL_CONCURRENCY_H
#define TEST_INDUSTRIALROBOT_FILECONTROLL_CONCURRENCY_H

#include <QObject>
#include "FileControll.h"


class Test_IndustrialRobot_FileControll_Concurrency : public QObject
{
    Q_OBJECT

public:
    explicit Test_IndustrialRobot_FileControll_Concurrency(QObject *parent = nullptr);

private:
    FileControll *m_fileCtrl;

    quint64 genId();
    quint64 m_id;

private slots:
    void initTestCase();
    void cleanupTestCase();

    void writeFile();
    void readFile();
    void readWriteFile();
};

#endif // TEST_INDUSTRIALROBOT_FILECONTROLL_CONCURRENCY_H
