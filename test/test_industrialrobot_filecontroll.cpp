#include "test_industrialrobot_filecontroll.h"

#include <QtTest>

const QString TEST_IP = "192.168.1.6";
const int TIMEOUT = 2000;
const QString FILE_NAME = "project/dobotlink_unittest.txt";
const QString PATH = "project";
const QString NEW_FOLDER_NAME = "dobotlink_unittest";
const QString NEW_FILE_OBJ_NAME = "project/dobotlink_unittest/xxxx.json";
const QString NEW_FILE_STR_NAME = "project/dobotlink_unittest/yyyy.txt";
const QString COPY_FOLDER_NAME = "dobotlink_unittest_copy";
const QString RENAME_FOLDER_NAME = "dobotlink_unittest_rename";
const QString COPY_FILE_STR_NAME = "project/dobotlink_unittest_rename/yyyy.txt";
const QString CHANGE_FILE_NAME = "project/dobotlink_unittest_rename/xxxx.json";
const QString DECODE_FILE_NAME = "project/dobotlink_unittest_rename/zzzz";
const QString DECODE_FILE_CONTENT = "proasdfa,9012O74185(^*&@$*)(*^暗色调福利卡闪电发货";
const QString DECODE_FILE_BASE64 = "cHJvYXNkZmEsOTAxMk83NDE4NSheKiZAJCopKCpe5pqX6Imy6LCD56aP5Yip5Y2h6Zeq55S15Y+R6LSn";
const QJsonObject DATA_OBJ = {
    {"123", 123},
    {"中文", "中文"},
    {"bool", true}
};
const QString DATA_STR = "123123asddfasdf阿斯蒂芬、/?asdf128645";
const QString WRONG_FOLDER_NAME = "worng";
Test_IndustrialRobot_FileControll::Test_IndustrialRobot_FileControll(QObject *parent):
    QObject(parent),
    m_id(123)
{
}

void Test_IndustrialRobot_FileControll::initTestCase()
{
    m_fileCtrl = new FileControll(TEST_IP, this);
}

void Test_IndustrialRobot_FileControll::cleanupTestCase()
{
    m_fileCtrl->deleteLater();
}

quint64 Test_IndustrialRobot_FileControll::genId()
{
    return m_id++;
}

void Test_IndustrialRobot_FileControll::writeFile_obj()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->writeFile(id, FILE_NAME, DATA_OBJ, TIMEOUT);

    QVERIFY(spy.wait(TIMEOUT)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() == 0);
}

void Test_IndustrialRobot_FileControll::readFile_obj()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->readFile(id, FILE_NAME, TIMEOUT);

    QVERIFY(spy.wait(TIMEOUT)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() == 0);

    QVERIFY(arguments.at(2).type() == QVariant::ByteArray);
    QVERIFY(!arguments.at(2).toByteArray().isEmpty());
    // QByteArray转换成QJsonObject
    QJsonDocument doc = QJsonDocument::fromJson(arguments.at(2).toByteArray());
    QCOMPARE(doc.object(), DATA_OBJ);
}

void Test_IndustrialRobot_FileControll::writeFile_str()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->writeFile(id, FILE_NAME, DATA_STR, TIMEOUT);

    QVERIFY(spy.wait(TIMEOUT)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() == 0);
}

void Test_IndustrialRobot_FileControll::readFile_str()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->readFile(id, FILE_NAME, TIMEOUT);

    QVERIFY(spy.wait(TIMEOUT)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() == 0);

    QVERIFY(arguments.at(2).type() == QVariant::ByteArray);
    QVERIFY(!arguments.at(2).toByteArray().isEmpty());
    QCOMPARE(QString(arguments.at(2).toByteArray()), DATA_STR);
}

void Test_IndustrialRobot_FileControll::newFolder()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->newFolder(id, PATH, NEW_FOLDER_NAME, TIMEOUT);

    QVERIFY(spy.wait(TIMEOUT)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() == 0);
}

void Test_IndustrialRobot_FileControll::newFile_obj()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->newFile(id, NEW_FILE_OBJ_NAME, DATA_OBJ, TIMEOUT);

    QVERIFY(spy.wait(TIMEOUT)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() == 0);
}

void Test_IndustrialRobot_FileControll::readNewFile_obj()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->readFile(id, NEW_FILE_OBJ_NAME, TIMEOUT);

    QVERIFY(spy.wait(TIMEOUT)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() == 0);

    QVERIFY(arguments.at(2).type() == QVariant::ByteArray);
    QVERIFY(!arguments.at(2).toByteArray().isEmpty());
    // QByteArray转换成QJsonObject
    QJsonDocument doc = QJsonDocument::fromJson(arguments.at(2).toByteArray());
    QCOMPARE(doc.object(), DATA_OBJ);
}

void Test_IndustrialRobot_FileControll::newFile_str()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->newFile(id, NEW_FILE_STR_NAME, DATA_STR, TIMEOUT);

    QVERIFY(spy.wait(TIMEOUT)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() == 0);
}

void Test_IndustrialRobot_FileControll::readNewFile_str()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->readFile(id, NEW_FILE_STR_NAME, TIMEOUT);

    QVERIFY(spy.wait(TIMEOUT)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() == 0);

    QVERIFY(arguments.at(2).type() == QVariant::ByteArray);
    QVERIFY(!arguments.at(2).toByteArray().isEmpty());
    QCOMPARE(QString(arguments.at(2).toByteArray()), DATA_STR);
}

void Test_IndustrialRobot_FileControll::copyFolder()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->copyFolder(id, PATH, NEW_FOLDER_NAME, COPY_FOLDER_NAME, TIMEOUT);

    QVERIFY(spy.wait(1000)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() == 0);
}

void Test_IndustrialRobot_FileControll::renameFolder()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    QString old_folder = QString("%1/%2").arg(PATH, NEW_FOLDER_NAME);
    QString new_folder = QString("%1/%2").arg(PATH, RENAME_FOLDER_NAME);
    m_fileCtrl->renameFolder(id, old_folder, new_folder, TIMEOUT);

    QVERIFY(spy.wait(1000)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() == 0);
}

void Test_IndustrialRobot_FileControll::readCopyFile_str()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->readFile(id, COPY_FILE_STR_NAME, TIMEOUT);

    QVERIFY(spy.wait(TIMEOUT)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() == 0);

    QVERIFY(arguments.at(2).type() == QVariant::ByteArray);
    QVERIFY(!arguments.at(2).toByteArray().isEmpty());
    QCOMPARE(QString(arguments.at(2).toByteArray()), DATA_STR);
}

void Test_IndustrialRobot_FileControll::changeFile()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->changeFile(id, CHANGE_FILE_NAME, "abc", false, TIMEOUT);

    QVERIFY(spy.wait(TIMEOUT)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() == 0);
}


void Test_IndustrialRobot_FileControll::readChangeFile()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->readFile(id, CHANGE_FILE_NAME, TIMEOUT);

    QVERIFY(spy.wait(TIMEOUT)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() == 0);

    QVERIFY(arguments.at(2).type() == QVariant::ByteArray);
    QVERIFY(!arguments.at(2).toByteArray().isEmpty());
    // QByteArray转换成QJsonObject
    QJsonDocument doc = QJsonDocument::fromJson(arguments.at(2).toByteArray());
    QJsonObject data(DATA_OBJ);
    data.insert("abc", false);
    QCOMPARE(doc.object(), data);
}

void Test_IndustrialRobot_FileControll::decodeFile()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->decodeFile(id, DECODE_FILE_NAME, DECODE_FILE_BASE64, TIMEOUT);

    QVERIFY(spy.wait(TIMEOUT)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() == 0);
}

void Test_IndustrialRobot_FileControll::readDecodeFile()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->readFile(id, DECODE_FILE_NAME, TIMEOUT);

    QVERIFY(spy.wait(TIMEOUT)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() == 0);

    QVERIFY(arguments.at(2).type() == QVariant::ByteArray);
    QVERIFY(!arguments.at(2).toByteArray().isEmpty());
    QCOMPARE(QString(arguments.at(2).toByteArray()), DECODE_FILE_CONTENT);
}

void Test_IndustrialRobot_FileControll::deleteFolder()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->deleteFolder(id, PATH, RENAME_FOLDER_NAME, TIMEOUT);

    QVERIFY(spy.wait(TIMEOUT)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() == 0);

    quint64 id2(genId());
    QSignalSpy spy2(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->deleteFolder(id2, PATH, COPY_FOLDER_NAME, TIMEOUT);

    QVERIFY(spy2.wait(TIMEOUT)); // 等待信号触发
    QCOMPARE(spy2.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments2 = spy2.takeFirst();

    QVERIFY(arguments2.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments2.at(0).toUInt(), id2);

    QVERIFY(arguments2.at(1).type() == QVariant::Int);
    QVERIFY(arguments2.at(1).toInt() == 0);
}

void Test_IndustrialRobot_FileControll::deleteFolder_wrong()
{
    quint64 id(genId());
    QSignalSpy spy(m_fileCtrl,&FileControll::onFinish_signal);

    m_fileCtrl->deleteFolder(id, PATH, WRONG_FOLDER_NAME, TIMEOUT);

    QVERIFY(spy.wait(TIMEOUT)); // 等待信号触发
    QCOMPARE(spy.count(), 1); // 确认信号只发送了一次

    //** 判断返回值 **/
    QList<QVariant> arguments = spy.takeFirst();

    QVERIFY(arguments.at(0).type() == QVariant::ULongLong);
    QCOMPARE(arguments.at(0).toULongLong(), id);

    QVERIFY(arguments.at(1).type() == QVariant::Int);
    QVERIFY(arguments.at(1).toInt() != 0);
}
