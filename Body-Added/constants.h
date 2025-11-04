#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QColor>

struct Constants {
    //physics
    static constexpr double GRAVITY         = 0.1;
    static constexpr double AIR_RESISTANCE  = 0.9992;
    static constexpr double MAX_VELOCITY    = 30.0;
    static constexpr double RESTITUTION     = -0.2;
    static constexpr double FRICTION        = 0.003;
    static constexpr double ACCELERATION    = 0.3;
    static constexpr double DECELERATION    = 0.3;
    static constexpr double SPRING_CONSTANT = 0.6;
    static constexpr double DAMPING  = 0.06;

    static constexpr double ROTATION = 0.5;

    //car
    static constexpr QColor CAR_COLOR = QColor(0, 150, 50);
    static constexpr QColor TYRE_COLOR = QColor(50, 50, 50);
    static constexpr int TYRE_THICKNESS = 10;
    static constexpr QColor WHEEL_COLOR = QColor(100, 100, 100);
};

#endif // CONSTANTS_H
