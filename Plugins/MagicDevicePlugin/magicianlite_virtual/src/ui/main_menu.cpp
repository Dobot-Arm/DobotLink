/**
 ***********************************************************************************************************************
 *
 * @author  ZhangRan
 * @version 1.0.0
 *
 * <h2><center>&copy; COPYRIGHT 2022 </center></h2>
 *
 ***********************************************************************************************************************
 */

#include "main_menu.h"
#include "../mm/profile.h"

MainMenu::MainMenu(QWidget* parent) : QWidget(parent)
{
    ui_ = new Ui::Form();
    ui_->setupUi(this);

    auto profile = new Profile(this);
    profile->init();
    connect(profile, &Profile::updateJoints, this, &MainMenu::updateJoint);
}

void MainMenu::updateJoint(float joint1, float joint2, float joint3, float joint4)
{
    ui_->lcdNumber->display(joint1);
    ui_->lcdNumber_2->display(joint2);
    ui_->lcdNumber_3->display(joint3);
    ui_->lcdNumber_4->display(joint4);
}
