#include "MonitorUI.h"

#include <QVBoxLayout>
#include <QLCDNumber>

#include "../mm/profile.h"

MonitorUI::MonitorUI(QWidget *parent) : QWidget(parent)
{
    SetupUi();

    auto profile = new Profile(this);
    profile->init();
    connect(profile, &Profile::updateJoints, this, &MonitorUI::updateJoint);
}

void MonitorUI::SetupUi()
{
    QVBoxLayout* pLayout = new QVBoxLayout(this);
    this->setLayout(pLayout);

    m_pLcd1 = new QLCDNumber(this);
    pLayout->addWidget(m_pLcd1);

    m_pLcd2 = new QLCDNumber(this);
    pLayout->addWidget(m_pLcd2);

    m_pLcd3 = new QLCDNumber(this);
    pLayout->addWidget(m_pLcd3);

    m_pLcd4 = new QLCDNumber(this);
    pLayout->addWidget(m_pLcd4);
}

void MonitorUI::updateJoint(float joint1, float joint2, float joint3, float joint4)
{
    m_pLcd1->display(joint1);
    m_pLcd2->display(joint2);
    m_pLcd3->display(joint3);
    m_pLcd4->display(joint4);
}
