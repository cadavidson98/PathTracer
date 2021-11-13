#include "vec.h"
#include <cmath>

Vec2::Vec2(float _x, float _y) : x(_x), y(_y) {

}

float Vec2::Dot(Vec2 rhs) const {
    return x * rhs.x + y * rhs.y;
}

float Vec2::Magnitude() const {
    return sqrt(x * x + y * y);
}

float Vec2::MagnitudeSqr() const {
    return x * x + y * y;
}

void Vec2::Normalize() {
    float mag = Magnitude();
    x /= mag;
    y /= mag;
}

Vec2 Vec2::UnitVec() const {
    Vec2 cpy = *this;
    cpy.Normalize();
    return cpy;
}

Vec3::Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {

}

Vec3::Vec3(Vec2 _xy, float _z) : x(_xy.x), y(_xy.y), z(_z) {

}

float Vec3::Dot(Vec3 rhs) const {
    return x * rhs.x + y * rhs.y + z * rhs.z;
}

Vec3 Vec3::Cross(Vec3 rhs) const {
    return Vec3(y * rhs.z - z * rhs.y,
                - x * rhs.z + rhs.x * z,
                + x * rhs.y - y * rhs.x);
}

float Vec3::Magnitude() const {
    return sqrt(x * x + y * y + z * z);
}

float Vec3::MagnitudeSqr() const {
    return x * x + y * y + z * z;
}

void Vec3::Normalize() {
    float mag = Magnitude();
    x /= mag;
    y /= mag;
    z /= mag;
}

Vec3 Vec3::UnitVec() const {
    Vec3 cpy = *this;
    cpy.Normalize();
    return cpy;
}

Vec4::Vec4(float _x, float _y, float _z, float _w) : x(_x), y(_y), z(_z), w(_w) {

}

Vec4::Vec4(Vec2 _xy, float _z, float _w) : x(_xy.x), y(_xy.y), z(_z), w(_w) {

}

Vec4::Vec4(Vec3 _xyz, float _w) : x(_xyz.x), y(_xyz.y), z(_xyz.z), w(_w) {

}

float Vec4::Dot(Vec4 _rhs) {
    return x * _rhs.x + y * _rhs.y + z * _rhs.z + w * _rhs.w;
}

Vec4 Vec4::Cross(Vec4 _rhs) {
    return Vec4();
}

float Vec4::Magnitude() {
    return sqrt(x * x + y * y + z * z + w * w);
}

float Vec4::MagnitudeSqr() {
    return x * x + y * y + z * z + w * w;
}

void Vec4::Normalize() {
    float magnitude = Magnitude();
    x /= magnitude;
    y /= magnitude;
    z /= magnitude;
    w /= magnitude;
}