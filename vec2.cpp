#include "vec2.h"
#include <math.h>

Vec2::Vec2() {}
Vec2::Vec2(float xin, float yin)
    : x(xin), y(yin)
{}

Vec2 Vec2::operator+ (const Vec2& rhs) {
    return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator- (const Vec2& rhs) {
    return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator* (float factor) {
    return Vec2(x * factor, y * factor);
}

void Vec2::operator+= (const Vec2& rhs) {
    x += rhs.x;
    y += rhs.y;
}

void Vec2::operator*= (float factor) {
    x *= factor;
    y *= factor;
}

float Vec2::dist(const Vec2& v)
{
    return sqrtf((v.x-x)*(v.x-x)+(v.y-y)*(v.y-y));
}

float Vec2::length() {
    return sqrtf(x*x+y*y);
}

Vec2 Vec2::normalize() {
    float len = length();
    return Vec2(x/len, y/len);
}