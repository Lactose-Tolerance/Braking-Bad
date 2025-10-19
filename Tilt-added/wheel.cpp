#include "wheel.h"
#include <cmath>
#include <algorithm>

Wheel::Wheel(int x_, int y_, int radius)
    : x(x_), y(y_), m_radius(radius) {}

void Wheel::attach(Wheel* other) {
    m_others.append(other);
    double dx = other->x - x;
    double dy = other->y - y;
    m_distances.append(std::sqrt(dx * dx + dy * dy));

    // designate this as the root wheel for the pair
    m_isRoot = true;
    other->m_isRoot = false;
}

void Wheel::simulate(const QList<Line>& lines, bool accelerating, bool braking) {
    // Integrate position
    x += m_vx;
    y -= m_vy;

    // Gravity
    m_vy -= GRAVITY;

    // Air resistance / drag
    m_vx *= AIR_RESISTANCE;
    m_vy *= AIR_RESISTANCE;

    // Ground collision / response (same as before)
    for (const Line& line : lines) {
        double m = line.getSlope();
        double b = line.getIntercept();

        double dist = std::abs(m * x - y + b) / std::sqrt(m * m + 1);
        double intersection_x = (m * (y - b) + x) / (m * m + 1);

        int minX = std::min(line.getX1(), line.getX2());
        int maxX = std::max(line.getX1(), line.getX2());

        if (dist < m_radius && intersection_x >= minX && intersection_x <= maxX) {
            double overlap = m_radius - dist;
            double normal_angle = std::atan2(-m, 1.0);
            y -= overlap * std::cos(normal_angle);
            x -= overlap * std::sin(normal_angle);

            // Rotate velocity into line coordinates
            double theta = -std::atan(m);
            double vAlongLine     = m_vx * std::cos(theta) + m_vy * std::sin(theta);
            double vNormalToLine  = m_vy * std::cos(theta) - m_vx * std::sin(theta);

            // Response
            vNormalToLine *= (vNormalToLine < 0.2) ? RESTITUTION : 1;
            // vAlongLine    *= FRICTION;
            if ((vAlongLine > MAX_VELOCITY / 1000) || vAlongLine < -MAX_VELOCITY / 1000) vAlongLine += ((vAlongLine > 0) ? -1 : 1) * MAX_VELOCITY / 1500;
            else vAlongLine = 0;

            // Player input along the tangent
            if (accelerating && vAlongLine <  MAX_VELOCITY) vAlongLine += (ACCELERATION * (1 - (vAlongLine / MAX_VELOCITY)));
            if (braking      && vAlongLine > -MAX_VELOCITY) vAlongLine -= (DECELERATION * (1 + (vAlongLine / MAX_VELOCITY)));

            // Rotate back to world coordinates
            m_vx = vAlongLine * std::cos(theta) - vNormalToLine * std::sin(theta);
            m_vy = vAlongLine * std::sin(theta) + vNormalToLine * std::cos(theta);

            if (m_isRoot && m_others.size() > 0 && dist < m_radius) {
                // Colliding wheel: lose some rotational energy
                m_omega *= 0.9;
            }

        }
    }

    // --- Rotational inertia of the two-wheel system (around center of mass) ---
    if (m_isRoot && m_others.size() > 0) {
        Wheel* other = m_others.first();

        // --- Update angular velocity ---
        if (accelerating && braking) {
            m_omega *= 0.5;
            // Apply a restoring torque to bring angle back to horizontal
            const double RESTORING_FACTOR = 0.002;  // tune this
            m_omega -= RESTORING_FACTOR * m_angle;
            if (std::abs(m_omega) < 1e-4) m_omega = 0;

            m_vy -= GRAVITY / 1.5;
        }
        else if (accelerating) {
            m_omega -= ANGULAR_ACCELERATION;
            if (m_omega < -MAX_ANGULAR_VELOCITY) m_omega = -MAX_ANGULAR_VELOCITY;
        }
        else if (braking) {
            m_omega += ANGULAR_DECELERATION;
            if (m_omega > MAX_ANGULAR_VELOCITY) m_omega = MAX_ANGULAR_VELOCITY;
        }
        else {
            m_omega *= ANGULAR_DAMPING;
            if (std::abs(m_omega) < 1e-4) m_omega = 0.0;
        }

        // --- Integrate angle ---
        m_angle += m_omega;

        if (m_angle > 2 * M_PI) m_angle -= 2 * M_PI;
        else if (m_angle < -2 * M_PI) m_angle += 2 * M_PI;

        // --- Apply *additional* rotation instead of forcing positions ---

        // Only adjust if there is active rotation (ω ≠ 0)
        if (std::abs(m_omega) > 1e-6) {
            double cx = (x + other->x) / 2.0;
            double cy = (y + other->y) / 2.0;

            // Current relative offsets from COM
            double rx1 = x - cx;
            double ry1 = y - cy;
            double rx2 = other->x - cx;
            double ry2 = other->y - cy;

            // Small incremental rotation by ω (not full angle)
            double sinA = std::sin(m_omega);
            double cosA = std::cos(m_omega);

            // Rotate both around COM by the small angular step
            double nx1 = rx1 * cosA - ry1 * sinA;
            double ny1 = rx1 * sinA + ry1 * cosA;
            double nx2 = rx2 * cosA - ry2 * sinA;
            double ny2 = rx2 * sinA + ry2 * cosA;

            // Apply back to world positions
            x        = cx + nx1;
            y        = cy + ny1;
            other->x = cx + nx2;
            other->y = cy + ny2;
        }

    }

    // Spring-damper constraints between attached wheels (same)
    for (int i = 0; i < m_others.size(); ++i) {
        Wheel* other = m_others.at(i);
        double desiredDistance = m_distances.at(i);

        double deltaX = other->x - x;
        double deltaY = other->y - y;
        double actualDistance = std::sqrt(deltaX * deltaX + deltaY * deltaY);
        if (actualDistance == 0) continue;

        double displacement = actualDistance - desiredDistance;
        double springForceMagnitude = displacement * SPRING_CONSTANT;

        double unitX = -deltaX / actualDistance;
        double unitY =  deltaY / actualDistance;

        double forceX = unitX * springForceMagnitude;
        double forceY = unitY * springForceMagnitude;

        double relativeVx = other->m_vx - m_vx;
        double relativeVy = other->m_vy - m_vy;
        double dampingForceX = relativeVx * DAMPING_FACTOR;
        double dampingForceY = relativeVy * DAMPING_FACTOR;

        this->m_vx -= (forceX - dampingForceX);
        this->m_vy -= (forceY - dampingForceY);
        other->m_vx += (forceX - dampingForceX);
        other->m_vy += (forceY - dampingForceY);
    }


}

std::optional<std::array<int, 3>> Wheel::get(int x1, int y1, int x2, int y2, int cx, int cy) const {
    if (x + m_radius + cx < x1 || x - m_radius + cx > x2 || y + m_radius + cy < y1 || y - m_radius + cy > y2)
        return std::nullopt;
    return std::array<int, 3>{
        static_cast<int>(std::lround(x + cx)),
        static_cast<int>(std::lround(y + cy)),
        m_radius
    };
}
