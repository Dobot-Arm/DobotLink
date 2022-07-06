
#ifndef TEST_MAIN_MENU_H
#define TEST_MAIN_MENU_H

#include <QWidget>
#include "ui_main_menu.h"

class MainMenu : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenu(QWidget* parent = nullptr);

public slots:
    void updateJoint(float joint1, float joint2, float joint3, float joint4);

private:
    Ui::Form* ui_;


};

#endif //TEST_MAIN_MENU_H
