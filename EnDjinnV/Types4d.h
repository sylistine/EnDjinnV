#pragma once

#define TEMPLATE_T template<typename T>

namespace Djn
{
    TEMPLATE_T
    struct tuple4
    {
        union
        {
            T x;
            T r;
        };
        union
        {
            T y;
            T g;
        };
        union
        {
            T z;
            T b;
        };
        union
        {
            T w;
            T a;
        };
        // ctor
        tuple4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
        tuple4(T x, T y, T z) : tuple4(x, y, z, 0) {}
        tuple4(T x, T y) : tuple4(x, y, 0, 0) {}
        tuple4() : tuple4(0, 0, 0, 0) {}
        tuple4(const tuple3<T>& other) :
            tuple4(other.x, other.y, other.z, 0) {}
        tuple4(const tuple2<T>& other) :
            tuple4(other.x, other.y, 0, 0)
        {}
        // Copy assignment operators.
        const tuple4& operator=(const tuple4<T>& rhs)
        {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
            w = rhs.w;
        }
        const tuple4& operator=(const tuple3<T>& rhs)
        {
            x = rhs.x;
            y = rhs.y;
            z = rhs.z;
            w = 0;
        }
        const tuple4& operator=(const tuple2<T>& rhs)
        {
            x = rhs.x;
            y = rhs.y;
            z = 0;
            w = 0;
        }
        // Arithmetic operators.
        inline const tuple4 operator+(const tuple4& rhs)
        {
            return tuple4(x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w);
        }
        inline const tuple4 operator-(const tuple4& rhs)
        {
            return tuple4(x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w);
        }
        inline const tuple4 operator*(const T& rhs)
        {
            return tuple4(x * rhs, y * rhs, z * rhs, w * rhs);
        }
        inline const tuple4 operator/(const T& rhs)
        {
            return tuple4(x / rhs, y / rhs, z / rhs, w / rhs);
        }
        // Assignment operators.
        const tuple4& operator+=(const tuple4& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            w += rhs.w;
            return *this;
        }
        const tuple4& operator-=(const tuple4& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            w -= rhs.w;
            return *this;
        }
        const tuple4& operator*=(const T& rhs)
        {
            x *= rhs;
            y *= rhs;
            z *= rhs;
            w *= rhs;
            return *this;
        }
        const tuple4& operator/=(const T& rhs)
        {
            x /= rhs;
            y /= rhs;
            z /= rhs;
            w /= rhs;
            return *this;
        }
    };

    // etc.
    TEMPLATE_T
    inline bool operator==(const tuple4<T>& lhs, const tuple4<T>& rhs)
    {
        return
            lhs.x == rhs.x &&
            lhs.y == rhs.y &&
            lhs.z == rhs.z &&
            lhs.w == rhs.w;
    }
    TEMPLATE_T
    inline bool operator!=(const tuple4<T>& lhs, const tuple4<T>& rhs)
    {
        return !(lhs == rhs);
    }

    typedef tuple4<double> double4;
    typedef tuple4<float> float4;
    typedef tuple4<int> int4;
    typedef tuple4<unsigned int> uint4;
}
