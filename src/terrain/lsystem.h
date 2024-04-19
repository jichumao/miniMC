#ifndef LSYSTEM_H
#define LSYSTEM_H

#include <unordered_map>
#include <string>
#include <glm/glm.hpp>
#include "scene/chunk.h"

class LSystem {
public:
    LSystem();

    std::string generateLSystem(std::string axiom, int iterations);

private:

    std::unordered_map<char, std::string> m_rules;
};

#endif // LSYSTEM_H
