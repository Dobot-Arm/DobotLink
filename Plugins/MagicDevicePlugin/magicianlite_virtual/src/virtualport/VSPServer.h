#pragma once

#include <QObject>

class CVSPServer : public QObject
{
    Q_OBJECT
public:
    explicit CVSPServer(QObject *parent = nullptr);
    virtual ~CVSPServer();
	
	void start(const QString& strPort);
    void changePortName(const QString& strPort);
};
