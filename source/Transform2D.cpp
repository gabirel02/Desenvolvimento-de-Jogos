#include "Transform2D.hpp"
#include <cmath>

Transform2D Transform2D::translation(float tx, float ty) noexcept {
    Transform2D t;
    t.m[2][0] = tx;
    t.m[2][1] = ty;
    return t;
}

Transform2D Transform2D::scale(float sx, float sy) noexcept {
    Transform2D t;
    t.m[0][0] = sx;
    t.m[1][1] = sy;
    return t;
}

Vector2D Transform2D::transform_point(const Vector2D& point) const noexcept {
    Vector2D v;
    v.x = point.x * m[0][0] + point.y * m[1][0] + m[2][0];
    v.y = point.x * m[0][1] + point.y * m[1][1] + m[2][1];
    return v;
}

Vector2D Transform2D::transform_vector(const Vector2D& direction) const noexcept {
    Vector2D v;
    v.x = direction.x * m[0][0] + direction.y * m[1][0];
    v.y = direction.x * m[0][1] + direction.y * m[1][1];
    return v;
}

Transform2D Transform2D::rotation(float angle_rad) noexcept {
    Transform2D t;
    float c = std::cos(angle_rad);
    float s = std::sin(angle_rad);
    t.m[0][0] = c;
    t.m[0][1] = s;
    t.m[1][0] = -s;
    t.m[1][1] = c;
    return t;
}

Transform2D Transform2D::operator*(const Transform2D& rhs) const noexcept {
    Transform2D t;
    float soma = 0.0f;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            for (int w = 0; w < 3; w++) {
                soma += m[i][w] * rhs.m[w][j];
            }
            t.m[i][j] = soma;
            soma = 0.0f;
        }
    }
    return t;
}

Transform2D& Transform2D::operator*=(const Transform2D& rhs) noexcept {
    *this = *this * rhs;
    return *this;
}