#ifndef STARTWORLD_HPP
#define STARTWORLD_HPP

#include <Tank/System/World.hpp>
#include <Tank/System/Game.hpp>
#include "MainWorld.hpp"

class StartWorld : public tank::World
{
public:
    virtual void update() override
    {
        tank::Game::log << "calling popWorld" << std::endl;
        tank::Game::popWorld();
        tank::Game::makeWorld<MainWorld>();
    }
};

#endif
