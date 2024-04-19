#include "lsystem.h"

LSystem::LSystem() {
    // initialize
    m_rules['X'] = "-YF+XFX+FY-";
    m_rules['Y'] = "+XF-YFY-FX+";
}

std::string LSystem::generateLSystem(std::string axiom, int iterations) {

    std::string current = axiom;
    for (int i = 0; i < iterations; ++i) {
        std::string next = "";
        for (char c : current) {
            if (m_rules.find(c) != m_rules.end()) {
                next += m_rules[c];
            } else {
                next += c;
            }
        }
        current = next;
    }
    return current;
}
