#include "VSPServer.h"

#include<exception>

#include "../mm/profile.h"
#include "../runtime/dcps/dcps.h"
//#include "MySafeUI.h"

CVSPServer::CVSPServer(QObject *parent) : QObject(parent)
{
    DobotProtocolSlave::instance();

    auto profile = new Profile(this);
    profile->init();
    /*
     * auto pUI = new MySafeUI();
     * pUI->CreateUI();
    */
}

CVSPServer::~CVSPServer()
{
}
	
void CVSPServer::start(const QString &strPort)
{
	try
    {
        DobotProtocolSlave::instance()->init(strPort);
    }
    catch (...)
    {
    }
}

void CVSPServer::changePortName(const QString &strPort)
{
    DobotProtocolSlave::instance()->setPortName(strPort);
}
