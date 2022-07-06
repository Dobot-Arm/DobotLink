#include "MySafeUI.h"

#include <QApplication>

#include "MonitorUI.h"

static MonitorUI* g_MonitorUI = nullptr;

MySafeUI::MySafeUI() : QObject(nullptr)
{
    connect(this, &MySafeUI::signalCreatUI, this, &MySafeUI::slotCreateUI);
}

void MySafeUI::slotCreateUI()
{
    g_MonitorUI = new MonitorUI();
    g_MonitorUI->show();
}

void MySafeUI::CreateUI()
{
    if (g_MonitorUI == nullptr)
    {
        this->moveToThread(qApp->thread());
        emit signalCreatUI();
    }
}
