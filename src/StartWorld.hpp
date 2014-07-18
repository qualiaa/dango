#ifndef STARTWORLD_HPP
#define STARTWORLD_HPP

#include <Tank/System/World.hpp>
#include <Tank/System/Game.hpp>
#include <Tank/Graphics/Font.hpp>
#include "MainWorld.hpp"

class StartWorld : public tank::World
{
public:
    static tank::Font font;
    static tank::Font titleFont;
public:
    StartWorld();
};

#endif
