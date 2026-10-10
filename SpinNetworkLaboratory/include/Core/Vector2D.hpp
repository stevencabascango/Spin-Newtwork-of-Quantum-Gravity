#pragma once
#include <cmath>
namespace snl {
struct Vec2 {
    double x=0,y=0;
    Vec2& operator+=(Vec2 v){x+=v.x;y+=v.y;return *this;}
    Vec2& operator-=(Vec2 v){x-=v.x;y-=v.y;return *this;}
};
inline Vec2 operator+(Vec2 a,Vec2 b){return a+=b;}
inline Vec2 operator-(Vec2 a,Vec2 b){return a-=b;}
inline Vec2 operator*(Vec2 a,double s){return {a.x*s,a.y*s};}
inline Vec2 operator/(Vec2 a,double s){return {a.x/s,a.y/s};}
inline double length(Vec2 v){return std::hypot(v.x,v.y);}
inline bool finite(Vec2 v){return std::isfinite(v.x)&&std::isfinite(v.y);}
}
