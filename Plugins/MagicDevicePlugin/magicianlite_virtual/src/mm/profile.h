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

#pragma once

#include <QTimer>

typedef uint8_t PacketData[256];

class Profile : public QObject
{
    Q_OBJECT

private:
    QTimer* timer_;
    uint64_t prev_time_;

public:
    explicit Profile(QObject* parent = nullptr);
    void init();
    void elapse();

Q_SIGNALS:
    void updateJoints(float joint1, float joint2, float joint3, float joint4);

protected:
    static void KineticFdCal();
    static void ProfileCalculate();
    void jointCal(float duration);
    static void InitInterpolation();
    static void InitRobot();

    static float getJoint(float min_val, float max_val, float val);
};
