#ifndef STARTWORLD_HPP
#define STARTWORLD_HPP

#include <Tank/System/World.hpp>
#include <Tank/System/Game.hpp>
#include <Tank/Graphics/Font.hpp>
#include "MainWorld.hpp"
#include "Input.hpp"

// TODO: UI placement factory
class StartWorld : public tank::World
{
    tank::observing_ptr<Input> hostname_;
    tank::observing_ptr<Input> port_;

    //unsigned menuIndex_ = 0;

public:
    static tank::Font font;
    static tank::Font titleFont;
public:
    StartWorld();
};

#endif
