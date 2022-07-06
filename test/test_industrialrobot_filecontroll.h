#ifndef TEST_INDUSTRIALROBOT_FILECONTROLL_H
#define TEST_INDUSTRIALROBOT_FILECONTROLL_H

#include <QObject>
#include "FileControll.h"

class Test_IndustrialRobot_FileControll : public QObject
{
    Q_OBJECT

public:
    Test_IndustrialRobot_FileControll(QObject *parent = nullptr);

private:
    FileControll *m_fileCtrl;

    quint64 genId();
    quint64 m_id;

private slots:
    void initTestCase();
    void cleanupTestCase();

    //![0] 将json对象写入文件
    void writeFile_obj();
    //![1] 读取刚写入文件的json对象
    void readFile_obj();
    //![2] 将string对象写入文件
    void writeFile_str();
    //![3] 读取刚写入文件的string
    void readFile_str();
    //![4] 新建一个文件夹
    void newFolder();
    //![5] 新建一个文件，并写入json对象
    void newFile_obj();
    //![6] 读取刚写入文件的json对象
    void readNewFile_obj();
    //![7] 新建一个文件，并写入string
    void newFile_str();
    //![8] 读取刚写入文件的string
    void readNewFile_str();
    //![9] 复制文件到这个文件夹
    void copyFolder();
    //![10] 重命名文件夹
    void renameFolder();
    //![11] 读取刚复制的文件内容
    void readCopyFile_str();
    //![12] 修改json文件内容
    void changeFile();
    //![13] 读取刚修改的修改json文件内容
    void readChangeFile();
    //![14] 写入Base64内容
    void decodeFile();
    //![15] 写入Base64内容
    void readDecodeFile();
    //![16] 删除文件夹
    void deleteFolder();
    //![17] 删除不存在的文件夹
    void deleteFolder_wrong();
};


#endif // TEST_INDUSTRIALROBOT_FILECONTROLL_H
