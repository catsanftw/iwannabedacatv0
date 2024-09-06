#include <iostream>
#include <vector>
#include <cmath>

// Vector2 class for 2D physics
class Vector2 {
public:
    float x, y;

    Vector2(float x = 0, float y = 0) : x(x), y(y) {}

    // Add two vectors
    Vector2 operator+(const Vector2& other) const {
        return Vector2(x + other.x, y + other.y);
    }

    // Subtract two vectors
    Vector2 operator-(const Vector2& other) const {
        return Vector2(x - other.x, y - other.y);
    }

    // Multiply vector by a scalar
    Vector2 operator*(float scalar) const {
        return Vector2(x * scalar, y * scalar);
    }

    // Dot product of two vectors
    float dot(const Vector2& other) const {
        return x * other.x + y * other.y;
    }

    // Compute magnitude of vector
    float magnitude() const {
        return std::sqrt(x * x + y * y);
    }

    // Normalize the vector
    Vector2 normalized() const {
        float mag = magnitude();
        return (mag > 0) ? (*this * (1.0f / mag)) : Vector2(0, 0);
    }
};

// RigidBody class representing a physics object
class RigidBody {
public:
    Vector2 position;    // Position of the body
    Vector2 velocity;    // Velocity of the body
    Vector2 force;       // Applied force
    float mass;          // Mass of the body

    RigidBody(float x, float y, float mass) : position(x, y), velocity(0, 0), force(0, 0), mass(mass) {}

    // Apply a force to the body
    void applyForce(const Vector2& f) {
        force = force + f;
    }

    // Update the body based on physics
    void update(float dt) {
        if (mass > 0) {
            Vector2 acceleration = force * (1.0f / mass);
            velocity = velocity + acceleration * dt;
            position = position + velocity * dt;
            force = Vector2(0, 0); // Reset force after applying
        }
    }
};

// Collision detection between two bodies (simple AABB collision)
bool checkCollision(const RigidBody& a, const RigidBody& b, float widthA, float heightA, float widthB, float heightB) {
    return (a.position.x < b.position.x + widthB &&
            a.position.x + widthA > b.position.x &&
            a.position.y < b.position.y + heightB &&
            a.position.y + heightA > b.position.y);
}

// Physics Engine class
class PhysicsEngine {
public:
    std::vector<RigidBody> bodies;

    // Add a rigid body to the engine
    void addBody(const RigidBody& body) {
        bodies.push_back(body);
    }

    // Update all bodies in the engine
    void update(float dt) {
        for (auto& body : bodies) {
            body.update(dt);
        }

        // Simple collision detection
        for (size_t i = 0; i < bodies.size(); ++i) {
            for (size_t j = i + 1; j < bodies.size(); ++j) {
                if (checkCollision(bodies[i], bodies[j], 1.0f, 1.0f, 1.0f, 1.0f)) {
                    std::cout << "Collision detected between body " << i << " and body " << j << std::endl;
                }
            }
        }
    }
};

int main() {
    PhysicsEngine engine;

    // Create and add bodies to the engine
    RigidBody body1(0, 0, 1.0f);
    RigidBody body2(0.5f, 0.5f, 1.0f);
    engine.addBody(body1);
    engine.addBody(body2);

    // Apply forces
    body1.applyForce(Vector2(10, 0));
    body2.applyForce(Vector2(-10, 0));

    // Update engine
    engine.update(0.1f);

    return 0;
}
