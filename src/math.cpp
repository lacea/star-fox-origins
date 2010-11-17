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

#include "math.h"

//TODO: Replace entirely
////////////////////
//  a3dssMatrix3
////////////////////

a3dssMatrix3::a3dssMatrix3(){
    m11 = m12 = m13 = 
    m21 = m22 = m23 = 
    m31 = m32 = m33 = 0.0f;
}
a3dssMatrix3::a3dssMatrix3(float _m11, float _m12, float _m13,
                           float _m21, float _m22, float _m23,
                           float _m31, float _m32, float _m33){
    m11 = _m11; m12 = _m12; m13 = _m13;
    m21 = _m21; m22 = _m22; m23 = _m23;
    m31 = _m31; m32 = _m32; m33 = _m33;
}

a3dssMatrix3::a3dssMatrix3(float ax, float ay, float az){
    float rad = 3.14159265358979323846264338327950f / 180.0f;
    float angle[3] = {ax*rad, ay*rad, az*rad};
    float A       = cosf(angle[0]);
    float B       = sinf(angle[0]);
    float C       = cosf(angle[1]);
    float D       = sinf(angle[1]);
    float E       = cosf(angle[2]);
    float F       = sinf(angle[2]);
    float AD      =   A * D;
    float BD      =   B * D;
    m11  =   C * E;
    m12  =  -C * F;
    m13  =   D;
    m21  =  BD * E + A * F;
    m22  = -BD * F + A * E;
    m23  =  -B * C;
    m31  = -AD * E + B * F;
    m32  =  AD * F + B * E;
    m33  =   A * C;
}

a3dssMatrix3& a3dssMatrix3::fromEuler(float ax, float ay, float az){
    float rad = 3.14159265358979323846264338327950f / 180.0f;
    float angle[3] = {ax*rad, ay*rad, az*rad};
    float A       = cosf(angle[0]);
    float B       = sinf(angle[0]);
    float C       = cosf(angle[1]);
    float D       = sinf(angle[1]);
    float E       = cosf(angle[2]);
    float F       = sinf(angle[2]);
    float AD      =   A * D;
    float BD      =   B * D;
    m11  =   C * E;
    m12  =  -C * F;
    m13  =   D;
    m21  =  BD * E + A * F;
    m22  = -BD * F + A * E;
    m23  =  -B * C;
    m31  = -AD * E + B * F;
    m32  =  AD * F + B * E;
    m33 =   A * C;
    return *this;
}


///////////////////////////////////////////////////////////////////////////////
//  a3dssVector3 operations
///////////////////////////////////////////////////////////////////////////////
a3dssVector3 operator * (const a3dssMatrix3& m, const a3dssVector3& v){
    return a3dssVector3(m.m11 * v.x + m.m12 * v.y + m.m13 * v.z,
                   m.m21 * v.x + m.m22 * v.y + m.m23 * v.z,
                   m.m31 * v.x + m.m32 * v.y + m.m33 * v.z);
}

a3dssVector3 operator * (const a3dssVector3& v, const a3dssMatrix3& m){
    return a3dssVector3(m.m11 * v.x + m.m21 * v.y + m.m31 * v.z,
                   m.m12 * v.x + m.m22 * v.y + m.m32 * v.z,
                   m.m13 * v.x + m.m23 * v.y + m.m33 * v.z);
}

