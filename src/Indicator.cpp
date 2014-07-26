#include "Indicator.hpp"

#include <Tank/Graphics/Text.hpp>
#include "StartWorld.hpp"

Indicator::Indicator(tank::Vectorf pos, std::string label)
    : Entity(pos)
{
    auto text = makeGraphic<tank::Text>(StartWorld::font);
    text->setText(label);
    text->setFontSize(24);
    text->setOrigin(tank::Vectori(text->getSize() / 2));
    indicator_ = makeGraphic<tank::CircleShape>(40);
    indicator_->setOrigin(tank::Vectori(indicator_->getSize() / 2));
    indicator_->setPos({0, 75});
    indicator_->setOutlineThickness(2);

    setColor(Black);
}

void Indicator::setColor(Stone turn)
{
    if (turn == Black) {
        indicator_->setFillColor({0,0,0});
        indicator_->setOutlineColor({0,0,0,0});
    } else if (turn == White) {
        indicator_->setFillColor({255,255,255});
        indicator_->setOutlineColor({0,0,0,0});
    } else {
        indicator_->setFillColor({0,0,0,0});
        indicator_->setOutlineColor({0,0,0,255});
    }
}
