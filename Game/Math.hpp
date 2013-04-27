#include <cmath>

#pragma once

namespace Detail
{
    struct Spinor
    {
        float Real;
        float Complex;
    };
}

inline float slerp(float a, float b, float dt)
{
    Detail::Spinor start, end;
    start.Real = cos(a/2);
    start.Complex = sin(a/2);
    end.Real = cos(b/2);
    end.Complex = sin(b/2);

    float tr, tc, omega, cosom, sinom, s0, s1;

    cosom = start.Real * end.Real + start.Complex * end.Complex;

    if (cosom < 0)
    {
        cosom = -cosom;
        tc = -end.Complex;
        tr = -end.Real;
    }
    else
    {
        tc = end.Complex;
        tr = end.Real;
    }

    if (1 - cosom > 0.001)
    {
        omega = acos(cosom);
        sinom = sin(omega);
        s0 = sin((1-dt) * omega) / sinom;
        s1 = sin(dt * omega) / sinom;
    }
    else
    {
        s0 = 1 - dt;
        s1 = dt;
    }

    Detail::Spinor ret;
    ret.Complex = s0 * start.Complex + s1 * tc;
    ret.Real = s0 * start.Real + s1 * tr;

    return atan2(ret.Complex, ret.Real) * 2;
};

auto dot = [](const sf::Vector2f& a, const sf::Vector2f& b) -> float{ sf::Vector2f c = b-a; return ((c.x*c.x) + (c.y*c.y)); };

static const float pi = 3.14159;
static const float phi = 0.61803;

static const float rad2deg = 180/pi;
static const float deg2rad = pi/180;