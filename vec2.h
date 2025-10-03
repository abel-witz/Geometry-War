#pragma once

class Vec2 {
public:
    float x = 0;
    float y = 0;

    Vec2();
    Vec2(float xin, float yin);
    Vec2 operator + (const Vec2& rhs);
    Vec2 operator - (const Vec2& rhs);
    Vec2 operator * (float factor);
    void operator += (const Vec2& rhs);
    void operator *= (float factor);
    float dist(const Vec2& v);
    float length();
    Vec2 normalize();
};