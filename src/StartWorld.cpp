#include "StartWorld.hpp"

#include <Tank/Graphics/Text.hpp>

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

    // Make input boxes
    hostname_ = makeEntity<Input>(tank::Vectorf{250,300},
                                  tank::Vectoru{200,16},
                                  "Hostname:");
    port_ = makeEntity<Input>(tank::Vectorf{250,330},
                              tank::Vectoru{200,16},
                              "Port:");

    // Set default text
    hostname_->setText("127.1");
    port_->setText("8037");

    // TODO: menuIndex management

    // Give focus to hostname
    hostname_->gainFocus();
}
