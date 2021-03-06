#ifndef VEC_H
#define VEC_H

class Vec2 {
    public:

        Vec2(float x = 0, float y = 0);

        float x;
        float y;

        float Dot(Vec2 rhs);
        float Magnitude();
        float MagnitudeSqr();
        void Normalize();
        Vec2 UnitVec();
        Vec2 friend operator /(const float& lhs, const Vec2& rhs);
        Vec2 friend operator *(const float& lhs, const Vec2& rhs);
        Vec2 friend operator /(const Vec2& lhs, const float& rhs);
        Vec2 friend operator *(const Vec2& lhs, const float& rhs);
        Vec2 friend operator -(const Vec2& lhs, const Vec2& rhs);
        Vec2 friend operator +(const Vec2& lhs, const Vec2& rhs);
};

class Vec3 {
    public:

        Vec3(float x = 0, float y = 0, float z = 0);
        Vec3(Vec2 xy, float z);
        
        float x;
        float y;
        float z;

        float Dot(Vec3 rhs);
        Vec3 Cross(Vec3 rhs);
        float Magnitude();
        float MagnitudeSqr();
        void Normalize();
        Vec3 UnitVec();
        Vec3 friend operator /(const float &lhs, const Vec3 &rhs);
        Vec3 friend operator *(const float& lhs, const Vec3& rhs);
        Vec3 friend operator /(const Vec3& lhs, const float& rhs);
        Vec3 friend operator *(const Vec3& lhs, const float& rhs);
        Vec3 friend operator -(const Vec3 &lhs, const Vec3 &rhs);
        Vec3 friend operator +(const Vec3 &lhs, const Vec3 &rhs);
};

class Vec4 {
    public:
        float x;
        float y;
        float z;
        float w;

        Vec4(float x = 0, float y = 0, float z = 0, float w = 0);
        Vec4(Vec2 xy, float z, float w);
        Vec4(Vec3 xyz, float w);
        
        float Dot(Vec4 rhs);
        Vec4 Cross(Vec4 rhs);
        float Magnitude();
        float MagnitudeSqr();
        void Normalize();

        Vec4 friend operator /(float &lhs, Vec4 &rhs);
        Vec4 friend operator -(Vec4 &lhs, Vec4 &rhs);
        Vec4 friend operator +(Vec4 &lhs, Vec4 &rhs);
};

inline Vec2 operator -(const Vec2& lhs, const Vec2& rhs) {
    return Vec2(lhs.x - rhs.x, lhs.y - rhs.y);
}

inline Vec2 operator +(const Vec2& lhs, const Vec2& rhs) {
    return Vec2(lhs.x + rhs.x, lhs.y + rhs.y);
}

inline Vec2 operator /(const float& lhs, const Vec2& rhs) {
    return Vec2(lhs / rhs.x, lhs / rhs.y);
};

inline Vec2 operator *(const float& lhs, const Vec2& rhs) {
    return Vec2(lhs * rhs.x, lhs * rhs.y);
}

inline Vec2 operator /(const Vec2& lhs, const float& rhs) {
    return Vec2(lhs.x / rhs, lhs.y / rhs);
}

inline Vec2 operator *(const Vec2& lhs, const float& rhs) {
    return Vec2(lhs.x * rhs, lhs.y * rhs);
}


inline Vec3 operator -(const Vec3 &lhs, const Vec3 &rhs) {
    return Vec3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
}

inline Vec3 operator +(const Vec3 &lhs,  const Vec3 &rhs) {
    return Vec3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
}

inline Vec3 operator /(const float &lhs, const Vec3 &rhs) {
    return Vec3(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z);
};

inline Vec3 operator *(const float& lhs, const Vec3& rhs) {
    return Vec3(lhs * rhs.x, lhs * rhs.y, lhs * rhs.z);
}

inline Vec3 operator /(const Vec3& lhs, const float& rhs) {
    return Vec3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
}

inline Vec3 operator *(const Vec3& lhs, const float& rhs) {
    return Vec3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
}

inline Vec4 operator -(const Vec4 &lhs,  const Vec4 &rhs) {
    return Vec4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - lhs.w);
}

inline Vec4 operator +(const Vec4 &lhs,  const Vec4 &rhs) {
    return Vec4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + lhs.w);
}

inline Vec4 operator /(const float &lhs, const  Vec4 &rhs) {
    return Vec4(lhs / rhs.x, lhs / rhs.y, lhs / rhs.z, lhs / rhs.w);
};

#endif  // VEC_H