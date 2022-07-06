#include <QtTest>
#include "test_industrialrobot_filecontroll.h"
#include "test_industrialrobot_filecontroll_concurrency.h"


int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    app.setAttribute(Qt::AA_Use96Dpi, true);

    int status(0);

    {
        Test_IndustrialRobot_FileControll tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    {
        Test_IndustrialRobot_FileControll_Concurrency tc;
        status |= QTest::qExec(&tc, argc, argv);
    }

    return status;
}
