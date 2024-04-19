#include "noise.h"

//  3D
glm::vec3 random3(const glm::vec3 &gridPoint) {
    // ou'd use a predefined table of random vectors.
//    float sinX = std::sin(gridPoint.x * 127.1f + gridPoint.y * 311.7f + gridPoint.z * 74.7f) * 43758.5453123f;
//    float sinY = std::sin(gridPoint.y * 269.5f + gridPoint.x * 183.3f + gridPoint.z * 246.1f) * 19650218.357f;
//    float sinZ = std::sin(gridPoint.z * 271.9f + gridPoint.y * 325.9f + gridPoint.x * 88.7f) * 12457823.675f;
    float sinX = std::sin(gridPoint.x * 12.9898 + gridPoint.y * 78.233 + gridPoint.z * 54.578) * 43758.5453;
    float sinY = std::sin(gridPoint.y * 12.9898 + gridPoint.x * 78.233 + gridPoint.z * 54.578) * 43758.5453;
    float sinZ = std::sin(gridPoint.z * 12.9898 + gridPoint.y * 78.233 + gridPoint.x * 54.578) * 43758.5453;
    return glm::vec3(sinX - std::floor(sinX), sinY - std::floor(sinY), sinZ - std::floor(sinZ));
}

glm::vec3 custom_pow(const glm::vec3 &v, float exponent) {
    return glm::vec3(std::pow(v.x, exponent), std::pow(v.y, exponent), std::pow(v.z, exponent));
}

float surflet3D(glm::vec3 p, glm::vec3 gridPoint) {
    // Compute the distance between p and the grid point along each axis, and warp it with a
    // quintic function so we can smooth our cells
    glm::vec3 t2 = glm::abs(p - gridPoint);
    glm::vec3 t = glm::vec3(1.f) - 6.f * custom_pow(t2, 5.f) + 15.f * custom_pow(t2, 4.f) - 10.f * custom_pow(t2, 3.f);

    // Get the random vector for the grid point (assume we wrote a function random3
    // that returns a vec3 in the range [0, 1])
    glm::vec3 gradient = glm::normalize(random3(gridPoint) * 2.f - glm::vec3(1.f));
    // Get the vector from the grid point to P
    glm::vec3 diff = p - gridPoint;
    // Get the value of our height field by dotting grid->P with our gradient
    float height = glm::dot(diff, gradient);
    // Scale our height field (i.e. reduce it) by our polynomial falloff function
    return height * t.x * t.y * t.z;
}

float perlinNoise3D(const glm::vec3 &p) {
    float surfletSum = 0.f;
    for (int dx = 0; dx <= 1; ++dx) {
        for (int dy = 0; dy <= 1; ++dy) {
            for (int dz = 0; dz <= 1; ++dz) {
                surfletSum += surflet3D(p, glm::floor(p) + glm::vec3(dx, dy, dz));
            }
        }
    }
    return surfletSum;
}

//  2D
glm::vec2 random2(const glm::vec2 &gridPoint) {
    float sinX = std::sin(gridPoint.x * 12.9898f + gridPoint.y * 78.233f) * 43758.5453f;
    float sinY = std::sin(gridPoint.y * 12.9898f + gridPoint.x * 78.233f) * 43758.5453f;

    return glm::vec2(sinX - std::floor(sinX), sinY - std::floor(sinY));
}

glm::vec2 custom_pow(const glm::vec2 &v, float exponent) {
    return glm::vec2(std::pow(v.x, exponent), std::pow(v.y, exponent));
}

float surflet2D(glm::vec2 P, glm::vec2 gridPoint) {

    float distX = glm::abs(P.x - gridPoint.x);
    float distY = glm::abs(P.y - gridPoint.y);
    float tX = 1 - 6 * glm::pow(distX, 5.f) + 15 * glm::pow(distX, 4.f) - 10 * glm::pow(distX, 3.f);
    float tY = 1 - 6 * glm::pow(distY, 5.f) + 15 * glm::pow(distY, 4.f) - 10 * glm::pow(distY, 3.f);

    glm::vec2 gradient = 2.f * random2(gridPoint) - glm::vec2(1.f);

    glm::vec2 diff = P - gridPoint;

    float height = glm::dot(diff, gradient);

    return height * tX * tY;
}


float perlinNoise2D(const glm::vec2 &p) {
    float surfletSum = 0.f;
    // Iterate over the four integer corners surrounding uv
    for(int dx = 0; dx <= 1; ++dx) {
        for(int dz = 0; dz <= 1; ++dz) {
            surfletSum += surflet2D(p, glm::floor(p) + glm::vec2(dx, dz));
        }
    }
    return surfletSum;
}

float perlinNoise(float x, float z) {
    //return perlinNoise3D(glm::vec3(x, 0.0f, z));;
    return perlinNoise3D(glm::vec3(x,0.f, z));;
}
