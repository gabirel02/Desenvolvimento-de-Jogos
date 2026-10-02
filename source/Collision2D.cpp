#include "Collision2D.hpp"

AABB Collision2D::bounds(const Vector2D& position) const noexcept {
    AABB caixa;
    caixa.min = position - halfExtents;
    caixa.max = position + halfExtents;
    return caixa;
}

bool AABB::intersects(const AABB& other) const noexcept {
    bool sobrepoe_x = (min.x <= other.max.x) && (other.min.x <= max.x);
    bool sobrepoe_y = (min.y <= other.max.y) && (other.min.y <= max.y);
    return sobrepoe_x && sobrepoe_y;
}