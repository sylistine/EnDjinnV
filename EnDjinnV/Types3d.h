#pragma once

#define TEMPLATE_T template<typename T>

namespace Djn
{
    TEMPLATE_T
    struct tuple3
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
        // ctors
        tuple3(T x, T y, T z) : x(x), y(y), z(z) {}
        tuple3(T x, T y) : tuple3(x, y, 0) {}
        tuple3() : tuple3(0, 0, 0) {}
        tuple3(const tuple2<T>& other) : tuple3(other.x, other.y, 0) {}
        const tuple3& operator=(const tuple2<T>& rhs)
        {
            x = rhs.x;
            y = rhs.y;
            z = 0;
        }
        // Arithmetic Operators
        inline const tuple3 operator+(const tuple3& rhs)
        {
            return tuple3(x + rhs.x, y + rhs.y, z + rhs.z);
        }
        inline const tuple3 operator-(const tuple3& rhs)
        {
            return tuple3(x - rhs.x, y - rhs.y, z - rhs.z);
        }
        inline const tuple3 operator*(const T& rhs)
        {
            return tuple3(x * rhs, y * rhs, z * rhs);
        }
        inline const tuple3 operator/(const T& rhs)
        {
            return tuple3(x / rhs, y / rhs, z / rhs);
        }
        // Assignment Operators
        const tuple3& operator+=(const tuple3& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            return *this;
        }
        const tuple3& operator-=(const tuple3& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            return *this;
        }
        const tuple3& operator*=(const T& rhs)
        {
            x *= rhs;
            y *= rhs;
            z *= rhs;
            return *this;
        }
        const tuple3& operator/=(const T& rhs)
        {
            x /= rhs;
            y /= rhs;
            z /= rhs;
            return *this;
        }
    };

    // etc.
    TEMPLATE_T
    inline bool operator==(const tuple3<T>& lhs, const tuple3<T>& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
    }
    TEMPLATE_T
    inline bool operator!=(const tuple3<T>& lhs, const tuple3<T>& rhs)
    {
        return !(lhs == rhs);
    }

    typedef tuple3<double> double3;
    typedef tuple3<float> float3;
    typedef tuple3<int> int3;
    typedef tuple3<unsigned int> uint3;
}
