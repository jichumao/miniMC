#ifndef NOISE_H
#define NOISE_H

#include <glm_includes.h>

using namespace std;
//  3D
glm::vec3 random3(const glm::vec3 &gridPoint);
glm::vec3 custom_pow(const glm::vec3 &v, float exponent);
float surflet(glm::vec3 p, glm::vec3 gridPoint);
float perlinNoise3D(const glm::vec3 &p);
//  2D
glm::vec2 random2(const glm::vec2 &gridPoint);
glm::vec2 custom_pow(const glm::vec2 &v, float exponent);
float surflet2D(glm::vec2 P, glm::vec2 gridPoint);
float perlinNoise2D(const glm::vec2 &p);

float perlinNoise(float x, float z);

#endif // NOISE_H
