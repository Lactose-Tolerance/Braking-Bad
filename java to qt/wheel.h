#ifndef WHEEL_H
#define WHEEL_H

#include "line.h"
#include <QList>
#include <optional>
#include <array>

class Wheel {
public:
    double x = 0.0, y = 0.0;
    double m_vx = 0.0, m_vy = 0.0;

    Wheel(int x, int y, int radius);

    void attach(Wheel* other);
    void simulate(const QList<Line>& lines, bool accelerating, bool braking);
    std::optional<std::array<int, 3>> get(int x1, int y1, int x2, int y2, int cx, int cy) const;

private:
    int m_radius;
    QList<Wheel*> m_others;
    QList<double> m_distances;

    // -------- Tunable physics constants --------
    static constexpr double GRAVITY         = 0.02;    // downward pull
    static constexpr double AIR_RESISTANCE  = 0.9999;  // velocity multiplier each step
    static constexpr double MAX_VELOCITY    = 10.0;    // cap along the ground
    static constexpr double RESTITUTION     = -0.2;    // bounce normal component
    static constexpr double FRICTION        = 0.99;    // along-surface damping
    static constexpr double ACCELERATION    = 0.1;     // push forward when accelerating
    static constexpr double DECELERATION    = 0.075;     // braking strength
    static constexpr double SPRING_CONSTANT = 0.01;    // wheel-to-wheel spring
    static constexpr double DAMPING_FACTOR  = 0.05;    // spring velocity damping
};

#endif // WHEEL_H
