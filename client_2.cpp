#include <iostream>
#include <vector>
#include <cmath>
#include <SDL2/SDL.h>

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

// RigidBody class representing a physical object
class RigidBody {
public:
    Vector2 position;    // Position of the body
    Vector2 velocity;    // Velocity of the body
    Vector2 force;       // Applied force
    float mass;          // Mass of the body
    Vector2 size;        // Size of the body (width, height)
    float friction;      // Friction coefficient

    RigidBody(float x, float y, float mass, float width, float height, float friction = 0.1f)
        : position(x, y), velocity(0, 0), force(0, 0), mass(mass), size(width, height), friction(friction) {}

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

            // Apply friction
            velocity = velocity * (1.0f - friction);
            force = Vector2(0, 0); // Reset force after applying
        }
    }
};

// Simplified AABB collision detection
bool checkCollision(const RigidBody& a, const RigidBody& b) {
    return (a.position.x < b.position.x + b.size.x &&
            a.position.x + a.size.x > b.position.x &&
            a.position.y < b.position.y + b.size.y &&
            a.position.y + a.size.y > b.position.y);
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
                if (checkCollision(bodies[i], bodies[j])) {
                    std::cout << "Collision detected between body " << i << " and body " << j << std::endl;
                    // Consider adding collision response here
                }
            }
        }
    }
};

SDL_Window* initSDL(int width, int height) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return nullptr;
    }

    SDL_Window* window = SDL_CreateWindow("Physics Engine Client", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return nullptr;
    }

    return window;
}

int main() {
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    const float DT = 0.016f; // 60 FPS

    SDL_Window* window = initSDL(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!window) {
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cerr << "Renderer could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return -1;
    }

    PhysicsEngine engine;
    RigidBody player(100, 100, 1.0f, 50.0f, 50.0f); // Player initialization with size
    engine.addBody(player);

    bool running = true;
    SDL_Event event;
    
    while (running) {
        while (SDL_PollEvent(&event) != 0) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        // Handle input
        const Uint8* state = SDL_GetKeyboardState(NULL);
        Vector2 force(0, 0);
        if (state[SDL_SCANCODE_LEFT]) force = Vector2(-10, 0);
        if (state[SDL_SCANCODE_RIGHT]) force = Vector2(10, 0);
        if (state[SDL_SCANCODE_UP]) force = Vector2(0, -10);
        if (state[SDL_SCANCODE_DOWN]) force = Vector2(0, 10);
        player.applyForce(force);

        // Update physics
        engine.update(DT);

        // Render
        SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
        SDL_RenderClear(renderer);

        SDL_Rect fillRect = { static_cast<int>(player.position.x), static_cast<int>(player.position.y), static_cast<int>(player.size.x), static_cast<int>(player.size.y) };
        SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
        SDL_RenderFillRect(renderer, &fillRect);

        SDL_RenderPresent(renderer);

        SDL_Delay(static_cast<Uint32>(DT * 1000.0f)); // Delay for frame timing
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
