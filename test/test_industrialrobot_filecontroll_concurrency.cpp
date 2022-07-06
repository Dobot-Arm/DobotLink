#include "test_industrialrobot_filecontroll_concurrency.h"

#include <QtTest>

const QString TEST_IP = "192.168.1.6";
const QJsonObject DATA_OBJ = {
    {"123", 123},
    {"中文", "中文"},
    {"bool", true}
};
const QString FILE_NAME = "project/dobotlink_unittest.json";
const int CONCURRENCY_TIMES = 100;

Test_IndustrialRobot_FileControll_Concurrency::Test_IndustrialRobot_FileControll_Concurrency(QObject *parent):
    QObject(parent),
    m_id(430)
{
}

void Test_IndustrialRobot_FileControll_Concurrency::initTestCase()
{
    m_fileCtrl = new FileControll(TEST_IP, this);
}

void Test_IndustrialRobot_FileControll_Concurrency::cleanupTestCase()
{
    m_fileCtrl->deleteLater();
}

quint64 Test_IndustrialRobot_FileControll_Concurrency::genId()
{
    return m_id++;
}


void Test_IndustrialRobot_FileControll_Concurrency::writeFile()
{
    QSignalSpy spy(m_fileCtrl, &FileControll::onFinish_signal);
    QMap<quint64, QTime> resMap;

    for (int i(0); i < CONCURRENCY_TIMES; ++i) {
        quint64 sendId(genId());
        m_fileCtrl->writeFile(sendId, FILE_NAME, DATA_OBJ);
        resMap.insert(sendId, QTime::currentTime());
    }

    while(resMap.size()) {
        QVERIFY(spy.wait(20000)); // 等待信号触发
        QTime endTime = QTime::currentTime();
        while(spy.size()) {
            QList<QVariant> arguments = spy.takeFirst();

            QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
            quint64 recvId = arguments.at(0).toULongLong();

            QVERIFY(resMap.contains(recvId));

            QTime startTime = resMap.take(recvId);
            qDebug() << recvId << startTime.msecsTo(endTime);

            QVERIFY(arguments.at(1).type() == QVariant::Int);
            QVERIFY(arguments.at(1).toInt() == 0);
        }
    }
}

void Test_IndustrialRobot_FileControll_Concurrency::readFile()
{
    QSignalSpy spy(m_fileCtrl, &FileControll::onFinish_signal);
    QMap<quint64, QTime> resMap;

    for (int i(0); i < CONCURRENCY_TIMES; ++i) {
        quint64 sendId(genId());
        m_fileCtrl->readFile(sendId, FILE_NAME);
        resMap.insert(sendId, QTime::currentTime());
    }

    while(resMap.size()) {
        QVERIFY(spy.wait(2000)); // 等待信号触发
        QTime endTime = QTime::currentTime();
        while(spy.size()) {
            QList<QVariant> arguments = spy.takeFirst();

            QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
            quint64 recvId = arguments.at(0).toULongLong();

            QVERIFY(resMap.contains(recvId));

            QTime startTime = resMap.take(recvId);
            qDebug() << recvId << startTime.msecsTo(endTime);

            QVERIFY(arguments.at(1).type() == QVariant::Int);
            QVERIFY(arguments.at(1).toInt() == 0);

            QVERIFY(arguments.at(2).type() == QVariant::ByteArray);
            QVERIFY(!arguments.at(2).toByteArray().isEmpty());
            // QByteArray转换成QJsonObject
            QJsonDocument doc = QJsonDocument::fromJson(arguments.at(2).toByteArray());
            QCOMPARE(doc.object(), DATA_OBJ);
        }
    }
}

void Test_IndustrialRobot_FileControll_Concurrency::readWriteFile()
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    QSignalSpy spy(m_fileCtrl, &FileControll::onFinish_signal);
    QMap<quint64, QTime> resMap;
    for (int i(0); i < CONCURRENCY_TIMES; ++i) {
        int flag = qrand() % 2;
        if (flag == 0) {
            quint64 sendId(genId());
            m_fileCtrl->readFile(sendId, FILE_NAME);
            resMap.insert(sendId, QTime::currentTime());
        } else {
            quint64 sendId(genId());
            m_fileCtrl->writeFile(sendId, FILE_NAME, DATA_OBJ);
            resMap.insert(sendId, QTime::currentTime());
        }
    }

    while(resMap.size()) {
        // 等待两个信号被触发
        QVERIFY(spy.wait(2000));

        QTime endTime = QTime::currentTime();
        while(spy.size()) {
            QList<QVariant> arguments = spy.takeFirst();

            QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
            quint64 recvId = arguments.at(0).toULongLong();

            QVERIFY(resMap.contains(recvId));

            QTime startTime = resMap.take(recvId);
            qDebug() << recvId << startTime.msecsTo(endTime);

            QVERIFY(arguments.at(1).type() == QVariant::Int);
            QVERIFY(arguments.at(1).toInt() == 0);
        }
    }
}
