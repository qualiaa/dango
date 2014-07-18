#include "StartWorld.hpp"

#include <Tank/Graphics/Text.hpp>
#include "Input.hpp"

tank::Font StartWorld::titleFont;
tank::Font StartWorld::font;

StartWorld::StartWorld()
{
    titleFont.loadFromFile("res/Anonymous Pro.ttf");
    font.loadFromFile("res/Consola.ttf");
    auto title = makeEntity<tank::Entity>()->makeGraphic<tank::Text>(titleFont);
    title->setColor({255,255,255});
    title->setText("dango");
    title->setFontSize(72);
    title->setOrigin(title->getSize() / 2);
    title->setPos(tank::Vectorf(tank::Game::window()->getSize().x / 2, 50));

    makeEntity<Input>(tank::Vectorf{300,400}, tank::Vectoru{200,20}, "TestInput");
}
