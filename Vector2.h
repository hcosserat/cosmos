#ifndef VECTOR2_H
#define VECTOR2_H

#include <iostream>

template<typename T>
class Vector2 {
public:
    T x;
    T y;

    // Constructors
    Vector2() : x(0), y(0) {
    }

    Vector2(T x, T y) : x(x), y(y) {
    }

    // Addition
    Vector2<T> operator+(const Vector2<T> &other) const {
        return Vector2<T>(x + other.x, y + other.y);
    }

    Vector2<T> &operator+=(const Vector2<T> &other) {
        x += other.x;
        y += other.y;
        return *this;
    }

    // Subtraction
    Vector2<T> operator-(const Vector2<T> &other) const {
        return Vector2<T>(x - other.x, y - other.y);
    }

    Vector2<T> &operator-=(const Vector2<T> &other) {
        x -= other.x;
        y -= other.y;
        return *this;
    }

    Vector2 operator-() const {
        return Vector2(-x, -x);
    }

    // Scalar multiplication
    Vector2<T> operator*(T scalar) const {
        return Vector2<T>(x * scalar, y * scalar);
    }

    Vector2<T> &operator*=(T scalar) {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    // Division by scalar
    Vector2<T> operator/(T scalar) const {
        return Vector2<T>(x / scalar, y / scalar);
    }

    Vector2<T> &operator/=(T scalar) {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    // Array-like access (for compatibility with previous code)
    T operator[](int index) const {
        if (index == 0) return x;
        if (index == 1) return y;
        throw std::out_of_range("Vector2 index out of range");
    }

    T &operator[](int index) {
        if (index == 0) return x;
        if (index == 1) return y;
        throw std::out_of_range("Vector2 index out of range");
    }

    T dist2(const Vector2<T> *other) const {
        const T dx = other->x - x;
        const T dy = other->y - y;
        return dx * dx + dy * dy;
    }
};

// Scalar multiplication (scalar on left side)
template<typename T>
Vector2<T> operator*(T scalar, const Vector2<T> &vec) {
    return Vector2<T>(scalar * vec.x, scalar * vec.y);
}

// Print operator
template<typename T>
std::ostream &operator<<(std::ostream &os, const Vector2<T> &vec) {
    os << "(" << vec.x << ", " << vec.y << ")";
    return os;
}

#endif // VECTOR2_H
