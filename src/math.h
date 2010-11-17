/*  Copyright (C) 2010 Michael Wells (dancinninjac@gmail.com)

    This file is part of StarFox Origins.

    StarFox Origins is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    StarFox Origins is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with StarFox Origins.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef __SF_MATH__
#define __SF_MATH__

#include <cmath>

class a3dssVector3;
class a3dssMatrix3;

class a3dssMatrix3{
    public:
    a3dssMatrix3();
    a3dssMatrix3(float ax, float ay, float az);
    a3dssMatrix3(float _m11, float _m12, float _m13,
              float _m21, float _m22, float _m23,
              float _m31, float _m32, float _m33);
    inline float& operator[] (int ind) {   return m[ind]; }
    a3dssMatrix3& fromEuler(float ax, float ay, float az);
    
    //~a3dssMatrix3();
    
    union{
        float m[9];
        struct {
            float m11,m12,m13,m21,m22,m23,m31,m32,m33;
        };
    };
};

class a3dssVector3{

public:
    a3dssVector3():x(0), y(0), z(0){};
    a3dssVector3(float nx, float ny, float nz):x(nx), y(ny), z(nz){};
    a3dssVector3(const a3dssVector3 &other):x(other.x), y(other.y), z(other.z){};
    //~a3dssVector3();
    
    void set(const float x, const float y, const float z){
        this->x = x;
        this->y = y;
        this->z = z;
    }

    void set(const a3dssVector3 &v){
        this->x = v.x;
        this->y = v.y;
        this->z = v.z;
    }

    a3dssVector3& normalize(){
        float l = x*x + y*y + z*z;
        if (l!=0.0f){
            l = 1.0/sqrt(l);
            x *= l; y *= l; z *= l;
        }
        return *this;
    }
    
    float lengthSqr() const {
        return x*x + y*y + z*z;
    }
    
    float length() const {
        return sqrtf(x*x + y*y + z*z);
    }
    
    // Returns distance from another point.
    float getDistanceFrom(const a3dssVector3 other) const {
        float vx = x - other.x; float vy = y - other.y; float vz = z - other.z;
        return sqrtf(vx*vx + vy*vy + vz*vz);
    }
    
    // Returns squared distance from another point. 
    float getSqrDistanceFrom(const a3dssVector3 other) const {
        float vx = x - other.x; float vy = y - other.y; float vz = z - other.z;
        return (vx*vx + vy*vy + vz*vz);
    }
    
    // Returns if this vector interpreted as a point is on a line between two other points.
    // It is assumed that the point is on the line.
    bool isBetweenPoints(const a3dssVector3& begin, const a3dssVector3& end) const {
        float f = (end - begin).lengthSqr();
        return getSqrDistanceFrom(begin) < f && 
               getSqrDistanceFrom(end) < f;
    }
    
    a3dssVector3 crossProduct(const a3dssVector3 &v){
        return a3dssVector3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
    
    float dotProduct(const a3dssVector3 &v) const {
        return x*v.x+y*v.y+z*v.z;
    }
    
    //element by element accessor
    inline float& operator[] (int ind) {   return v[ind]; }
    
    //negative
    inline const a3dssVector3 operator - () const {return a3dssVector3(-x,-y,-z);}
    
    // scalar Operators
    // WARNING: No division by zero checked!!!
    inline const a3dssVector3 operator + (const float& s) const {return a3dssVector3(x+s,y+s,z+s);}
    inline const a3dssVector3 operator - (const float& s) const {return a3dssVector3(x-s,y-s,z-s);}
    inline const a3dssVector3 operator * (const float& s) const {return a3dssVector3(x*s,y*s,z*s);}
    inline const a3dssVector3 operator / (const float& s) const {return a3dssVector3(x/s,y/s,z/s);} //No division by 0 checked!!!
    a3dssVector3& operator *= (const float& s) {x*=s; y*=s; z*=s; return *this;}
    a3dssVector3& operator /= (const float& s) {x/=s; y/=s; z/=s; return *this;}   //No division by 0 checked!!!
    //Vector operators
    inline const a3dssVector3 operator + (const a3dssVector3& v) const {return a3dssVector3(x+v.x,y+v.y,z+v.z);}
    inline const a3dssVector3 operator - (const a3dssVector3& v) const {return a3dssVector3(x-v.x,y-v.y,z-v.z);}
    a3dssVector3& operator += (const a3dssVector3& v) {x+=v.x; y+=v.y; z+=v.z; return *this;}
    a3dssVector3& operator -= (const a3dssVector3& v) {x-=v.x; y-=v.y; z-=v.z; return *this;}
    //matrix operators
    friend a3dssVector3 operator * (const a3dssMatrix3& m, const a3dssVector3& v);
    friend a3dssVector3 operator * (const a3dssVector3& v, const a3dssMatrix3& m);
    
    
    union{
        float v[3];
        struct{
            float x,y,z;
        };
    };
};

//this class is useful for sphere collision detection
class Sphere{

public:
    Sphere():x(0),y(0),z(){};
    Sphere(float X, float Y, float Z, float R):x(X),y(Y),z(Z),r(R){};
    // Returns distance from another sphere.
    float getDistanceFrom(const Sphere other) const {
        float sx = x - other.x; float sy = y - other.y; float sz = z - other.z;
        return sqrtf(sx*sx + sy*sy + sz*sz);
    }
    
    // Returns squared distance from another sphere. 
    float getSqrDistanceFrom(const Sphere other) const {
        float sx = x - other.x; float sy = y - other.y; float sz = z - other.z;
        return (sx*sx + sy*sy + sz*sz);
    }
    
    // Check if this sphere collides with other using squared distances
    bool checkCollisionFast(const Sphere other){
        float sx = x - other.x; float sy = y - other.y; float sz = z - other.z;
        float radius = r+other.r;
        float dist = sx*sx + sy*sy + sz*sz;
        if (dist<(radius*radius))
            return true;
        else
            return false;
    }
    
    // Check if this sphere collides with other
    bool checkCollision(const Sphere other) {
        float sx = x - other.x; float sy = y - other.y; float sz = z - other.z;
        float dist = sqrtf(sx*sx + sy*sy + sz*sz);
        if (dist<(r+other.r))
            return true;
        else
            return false;
    }
    
    float x,y,z,r;
};

#endif  //__MATH_H__
