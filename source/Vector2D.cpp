#include <Vector2D.hpp>
#include <cmath>

Vector2D Vector2D::operator+(const Vector2D& rhs) const noexcept
{
    return Vector2D(x + rhs.x, y + rhs.y);
}

Vector2D Vector2D::operator-(const Vector2D& rhs) const noexcept
{
    return Vector2D(x - rhs.x, y - rhs.y);
}

Vector2D Vector2D::operator*(float scalar) const noexcept
{
    return Vector2D(x * scalar, y * scalar);
}

Vector2D operator*(float scalar, const  Vector2D& vec) noexcept
{
    return vec * scalar;
}

