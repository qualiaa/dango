#ifndef STARTWORLD_HPP
#define STARTWORLD_HPP

#include <Tank/System/World.hpp>
#include <Tank/Graphics/Font.hpp>
#include <Tank/Graphics/Text.hpp>
#include "Input.hpp"
#include "Button.hpp"

// TODO: UI placement factory
// TODO: Abstract MenuItem class for Input, Button with gainFocus, etc.
class StartWorld : public tank::World
{
    tank::observing_ptr<Input> hostname_;
    tank::observing_ptr<Input> port_;
    tank::observing_ptr<Button> button_;
    tank::observing_ptr<tank::Text> errorText_;

    unsigned menuItem_;
    const unsigned menuItems = 3;

public:
    static tank::Font font;
    static tank::Font titleFont;
public:
    StartWorld();

    void switchFocus(unsigned);
    void startGame();
};

#endif
