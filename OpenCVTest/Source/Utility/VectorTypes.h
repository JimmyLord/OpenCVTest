//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __VectorTypes_H__
#define __VectorTypes_H__

const float FEQUALEPSILON = 0.00001f;

inline bool fequal(const float a, const float b, const float epsilon = FEQUALEPSILON)
{
    return fabs(a - b) <= epsilon;
}

inline bool fnotequal(const float a, const float b, const float epsilon = FEQUALEPSILON)
{
    return fabs(a - b) > epsilon;
}

inline bool fequal(const double a, const double b, const double epsilon = FEQUALEPSILON)
{
    return fabs(a - b) <= epsilon;
}

inline bool fnotequal(const double a, const double b, const double epsilon = FEQUALEPSILON)
{
    return fabs(a - b) > epsilon;
}

class ColorRGBf
{
public:
    float r;
    float g;
    float b;

    ColorRGBf() { r = g = b = 0; }
    ColorRGBf(float nr, float ng, float nb) { r = nr; g = ng; b = nb; }
    ColorRGBf(cv::Vec3b in) { r = in[2]/255.0f; g = in[1]/255.0f; b = in[0]/255.0f; }
    inline void FromVec3b(cv::Vec3b in) { r = in[2]/255.0f; g = in[1]/255.0f; b = in[0]/255.0f; }
    inline cv::Vec3b AsVec3b() { return cv::Vec3b( (unsigned char)(b*255), (unsigned char)(g*255), (unsigned char)(r*255) ); }

    inline float DistanceFrom(ColorRGBf o) { return sqrtf( (r-o.r)*(r-o.r) + (g-o.g)*(g-o.g) + (b-o.b)*(b-o.b) ); }

    inline bool operator ==(const ColorRGBf& o) const { return fequal(this->r, o.r) && fequal(this->g, o.g) && fequal(this->b, o.b); }
    inline bool operator !=(const ColorRGBf& o) const { return !fequal(this->r, o.r) || !fequal(this->g, o.g) || !fequal(this->b, o.b); }

    inline ColorRGBf operator -() const { return ColorRGBf(-this->r, -this->g, -this->b); }
    inline ColorRGBf operator *(const float o) const { return ColorRGBf(this->r * o, this->g * o, this->b * o); }
    inline ColorRGBf operator /(const float o) const { return ColorRGBf(this->r / o, this->g / o, this->b / o); }
    inline ColorRGBf operator +(const float o) const { return ColorRGBf(this->r + o, this->g + o, this->b + o); }
    inline ColorRGBf operator -(const float o) const { return ColorRGBf(this->r - o, this->g - o, this->b - o); }
    inline ColorRGBf operator *(const ColorRGBf& o) const { return ColorRGBf(this->r * o.r, this->g * o.g, this->b * o.b); }
    inline ColorRGBf operator /(const ColorRGBf& o) const { return ColorRGBf(this->r / o.r, this->g / o.g, this->b / o.b); }
    inline ColorRGBf operator +(const ColorRGBf& o) const { return ColorRGBf(this->r + o.r, this->g + o.g, this->b + o.b); }
    inline ColorRGBf operator -(const ColorRGBf& o) const { return ColorRGBf(this->r - o.r, this->g - o.g, this->b - o.b); }

    inline ColorRGBf operator *=(const float o) { this->r *= o; this->g *= o; this->b *= o; return *this; }
    inline ColorRGBf operator /=(const float o) { this->r /= o; this->g /= o; this->b /= o; return *this; }
    inline ColorRGBf operator +=(const float o) { this->r += o; this->g += o; this->b += o; return *this; }
    inline ColorRGBf operator -=(const float o) { this->r -= o; this->g -= o; this->b -= o; return *this; }
    inline ColorRGBf operator *=(const ColorRGBf& o) { this->r *= o.r; this->g *= o.g; this->b *= o.b; return *this; }
    inline ColorRGBf operator /=(const ColorRGBf& o) { this->r /= o.r; this->g /= o.g; this->b /= o.b; return *this; }
    inline ColorRGBf operator +=(const ColorRGBf& o) { this->r += o.r; this->g += o.g; this->b += o.b; return *this; }
    inline ColorRGBf operator -=(const ColorRGBf& o) { this->r -= o.r; this->g -= o.g; this->b -= o.b; return *this; }
};

inline ColorRGBf operator *(float scalar, const ColorRGBf& vector) { return ColorRGBf(scalar * vector.r, scalar * vector.g, scalar * vector.b); }
inline ColorRGBf operator /(float scalar, const ColorRGBf& vector) { return ColorRGBf(scalar / vector.r, scalar / vector.g, scalar / vector.b); }
inline ColorRGBf operator +(float scalar, const ColorRGBf& vector) { return ColorRGBf(scalar + vector.r, scalar + vector.g, scalar + vector.b); }
inline ColorRGBf operator -(float scalar, const ColorRGBf& vector) { return ColorRGBf(scalar - vector.r, scalar - vector.g, scalar - vector.b); }

class vec2
{
public:
    float x;
    float y;

public:
    vec2() {}
    vec2(float nxy) { x = nxy; y = nxy; }
    vec2(float nx, float ny) { x = nx; y = ny; }
    vec2(int nx, int ny) { x = (float)nx; y = (float)ny; }
    //virtual ~vec2() {}

    static const vec2 Right() { return vec2( 1.0f, 0.0f ); }
    static const vec2 Up() { return vec2( 0.0f, 1.0f ); }

    inline void Set(float nx, float ny) { x = nx; y = ny; }
    inline float LengthSquared() const { return x*x + y*y; }
    inline float Length() const { return sqrtf(x*x + y*y); }
    inline float DistanceFrom(vec2 o) { return sqrtf( (x-o.x)*(x-o.x) + (y-o.y)*(y-o.y) ); }

    inline vec2 GetNormalized() const { float len = Length(); if( fequal(len,0) ) return vec2(x,y); len = 1.0f/len; return vec2(x*len, y*len); }
    inline vec2 Normalize() { float len = Length(); if( !fequal(len,0) ) { x /= len; y /= len; } return *this; }
    inline float Dot(const vec2 &o) const { return x*o.x + y*o.y; }
    inline vec2 Add(const vec2& o) const { return vec2(this->x + o.x, this->y + o.y); }
    inline vec2 Sub(const vec2& o) const { return vec2(this->x - o.x, this->y - o.y); }
    inline vec2 Scale(const float o) const { return vec2(this->x * o, this->y * o); }

    inline vec2 WithX(float x) const { return vec2( x, this->y ); }
    inline vec2 WithY(float y) const { return vec2( this->x, y ); }

    inline bool operator ==(const vec2& o) const { return fequal(this->x, o.x) && fequal(this->y, o.y); }
    inline bool operator !=(const vec2& o) const { return !fequal(this->x, o.x) || !fequal(this->y, o.y); }

    inline vec2 operator -() const { return vec2(-this->x, -this->y); }
    inline vec2 operator *(const float o) const { return vec2(this->x * o, this->y * o); }
    inline vec2 operator /(const float o) const { return vec2(this->x / o, this->y / o); }
    inline vec2 operator +(const float o) const { return vec2(this->x + o, this->y + o); }
    inline vec2 operator -(const float o) const { return vec2(this->x - o, this->y - o); }
    inline vec2 operator *(const vec2& o) const { return vec2(this->x * o.x, this->y * o.y); }
    inline vec2 operator /(const vec2& o) const { return vec2(this->x / o.x, this->y / o.y); }
    inline vec2 operator +(const vec2& o) const { return vec2(this->x + o.x, this->y + o.y); }
    inline vec2 operator -(const vec2& o) const { return vec2(this->x - o.x, this->y - o.y); }

    inline vec2 operator *=(const float o) { this->x *= o; this->y *= o; return *this; }
    inline vec2 operator /=(const float o) { this->x /= o; this->y /= o; return *this; }
    inline vec2 operator +=(const float o) { this->x += o; this->y += o; return *this; }
    inline vec2 operator -=(const float o) { this->x -= o; this->y -= o; return *this; }
    inline vec2 operator *=(const vec2& o) { this->x *= o.x; this->y *= o.y; return *this; }
    inline vec2 operator /=(const vec2& o) { this->x /= o.x; this->y /= o.y; return *this; }
    inline vec2 operator +=(const vec2& o) { this->x += o.x; this->y += o.y; return *this; }
    inline vec2 operator -=(const vec2& o) { this->x -= o.x; this->y -= o.y; return *this; }

    float& operator[] (int i) { assert(i>=0 && i<2); return *(&x + i); }
};

inline vec2 operator *(float scalar, const vec2& vector) { return vec2(scalar * vector.x, scalar * vector.y); }
inline vec2 operator /(float scalar, const vec2& vector) { return vec2(scalar / vector.x, scalar / vector.y); }
inline vec2 operator +(float scalar, const vec2& vector) { return vec2(scalar + vector.x, scalar + vector.y); }
inline vec2 operator -(float scalar, const vec2& vector) { return vec2(scalar - vector.x, scalar - vector.y); }

#endif //__VectorTypes_H__
