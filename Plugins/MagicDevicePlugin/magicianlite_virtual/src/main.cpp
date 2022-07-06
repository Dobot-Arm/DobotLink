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

#include "ui/main_menu.h"
#include <QApplication>
#include <dcps/dcps.h>

#include "mm/profile.h"

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    MainMenu main_menu;
    main_menu.show();

    DobotProtocolSlave* dcps = DobotProtocolSlave::instance();
    dcps->init();

    return QApplication::exec();
}
