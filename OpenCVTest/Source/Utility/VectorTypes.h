//
// Copyright (c) 2019 Jimmy Lord http://www.flatheadgames.com
//
#ifndef __VectorTypes_H__
#define __VectorTypes_H__

typedef Vector2 vec2;
typedef Vector3 vec3;
typedef Vector4 vec4;
typedef Vector2Int ivec2;
typedef Vector3Int ivec3;
typedef Vector4Int ivec4;

class ColorRGBf
{
public:
    float r;
    float g;
    float b;

    ColorRGBf() { r = g = b = 0; }
    ColorRGBf(float nr, float ng, float nb) { r = nr; g = ng; b = nb; }
    ColorRGBf(unsigned char nr, unsigned char ng, unsigned char nb) { r = nr/255.0f; g = ng/255.0f; b = nb/255.0f; }
    ColorRGBf(cv::Vec3b in) { r = in[2]/255.0f; g = in[1]/255.0f; b = in[0]/255.0f; }
    inline void Set(float nr, float ng, float nb) { r = nr, g = ng, b = nb; }
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

#endif //__VectorTypes_H__
