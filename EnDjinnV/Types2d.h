#pragma once

#define TEMPLATE_T template<typename T>

namespace Djn
{
    TEMPLATE_T struct tuple2
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
        // ctors.
        tuple2(T x, T y) : x(x), y(y) {}
        tuple2() : tuple2(0, 0) {}
        // Arithmetic operators.
        inline const tuple2 operator+(const tuple2& rhs)
        {
            return tuple2(x + rhs.x, y + rhs.y);
        }
        inline const tuple2 operator-(const tuple2& rhs)
        {
            return tuple2(x - rhs.x, y - rhs.y);
        }
        inline const tuple2 operator*(const T& rhs)
        {
            return tuple2(x * rhs, y * rhs);
        }
        inline const tuple2 operator/(const T& rhs)
        {
            return tuple2(x / rhs, y / rhs);
        }
        // Assignment operators.
        const tuple2& operator+=(const tuple2& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            return *this;
        }
        const tuple2& operator-=(const tuple2& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            return *this;
        }
        const tuple2& operator*=(T rhs)
        {
            x *= rhs;
            y *= rhs;
            return *this;
        }
        const tuple2& operator/=(T rhs)
        {
            x /= rhs;
            y /= rhs;
            return *this;
        }
    };

    // etc.
    TEMPLATE_T
    inline bool operator==(const tuple2<T>& lhs, const tuple2<T>& rhs)
    {
        return lhs.x == rhs.x && lhs.y == rhs.y;
    }
    TEMPLATE_T
    inline bool operator!=(const tuple2<T>& lhs, const tuple2<T>& rhs)
    {
        return !(lhs == rhs);
    }

    typedef tuple2<double> double2;
    typedef tuple2<float> float2;
    typedef tuple2<int> int2;
    typedef tuple2<unsigned int> uint2;
}
