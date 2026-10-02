#include "RigidBody2D.hpp"
#include <cassert>

void RigidBody2D::integrate(float dt) noexcept {
    assert(dt > 0.0f && "O passo de tempo dt deve ser positivo");
    velocity += acceleration * dt;
    position += velocity * dt;
}