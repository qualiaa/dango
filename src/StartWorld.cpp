#include "StartWorld.hpp"

#include <Tank/System/Game.hpp>
#include <Tank/System/Keyboard.hpp>
#include <Tank/Graphics/Text.hpp>
#include "MainWorld.hpp"

tank::Font StartWorld::titleFont;
tank::Font StartWorld::font;

StartWorld::StartWorld()
{
    // Load fonts
    titleFont.loadFromFile("res/Anonymous Pro.ttf");
    font.loadFromFile("res/Consola.ttf");

    // Make title text
    auto title = makeEntity<tank::Entity>()->makeGraphic<tank::Text>(titleFont);
    title->setColor({255,255,255});
    title->setText("dango");
    title->setFontSize(72);
    title->setOrigin(title->getSize() / 2);
    title->setPos(tank::Vectorf(tank::Game::window()->getSize().x / 2, 50));

    std::function<void()> callback = std::bind(&StartWorld::startGame,this);
    // Make input boxes
    hostname_ = makeEntity<Input>(tank::Vectorf{250,300},
                                  tank::Vectoru{200,16},
                                  "Hostname:",
                                  callback);
    port_ = makeEntity<Input>(tank::Vectorf{250,330},
                              tank::Vectoru{200,16},
                              "Port:",
                              callback);

    button_ = makeEntity<Button>(tank::Vectorf{250,360},
                                 tank::Vectoru{200,30},
                                 "Connect",
                                 callback);
    // Set default text
    hostname_->setText("127.1");
    port_->setText("8037");

    // Key bindings
    using K = tank::Keyboard;
    using Key = tank::Key;
    connect(K::KeyPress(Key::Tab) && []{return not K::control();}, [this](){ switchFocus(1);} );
    connect(K::KeyPress(Key::Tab) && K::Control, [this](){ switchFocus(menuItems - 1);});
    //connect(K::KeyPress(Key::Return), &StartWorld::switchFocus);

    // Give focus to hostname
    menuItem_ = 0;
    hostname_->gainFocus();
}

void StartWorld::switchFocus(unsigned shift)
{
    menuItem_ += shift;
    menuItem_ %= menuItems;

    if (menuItem_ == 0) {
        hostname_->gainFocus();
        port_->loseFocus();
        button_ -> loseFocus();
    } else if (menuItem_ == 1) {
        hostname_->loseFocus();
        port_->gainFocus();
        button_ -> loseFocus();
    } else {
        hostname_->loseFocus();
        port_->loseFocus();
        button_ -> gainFocus();
    }
}

void StartWorld::startGame()
{
    tank::Game::makeWorld<MainWorld>(hostname_->getText(), port_->getText());
}
