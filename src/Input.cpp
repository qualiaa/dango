#include "Input.hpp"

#include <Tank/System/Keyboard.hpp>
#include <Tank/System/Mouse.hpp>
#include "StartWorld.hpp"

Input::Input(tank::Vectorf pos, tank::Vectoru size, std::string label)
    : tank::Entity(pos)
{
    box_ = makeGraphic<tank::RectangleShape>(size);
    text_ = makeGraphic<tank::Text>(StartWorld::font);
    label_ = makeGraphic<tank::Text>(StartWorld::font);

    box_->setOutlineColor({200,200,200});
    setFontSize(size.y);
    setLabel(label);
}

void Input::setLabel(std::string label)
{
    label_->setText(label);
    label_->setOrigin({label_->getSize().x, 0});
}

void Input::setFontSize(unsigned s)
{
    label_->setFontSize(s);
    text_->setFontSize(s);
}

unsigned Input::getFontSize()
{
    return label_->getFontSize();
}

void Input::onAdded()
{
    using K = tank::Keyboard;
    using M = tank::Mouse;
    using B = M::Button;

    auto haveFocus = [&]() { return hasFocus_; };
    //auto noFocus = [&]() { return not hasFocus_; };
    connect(K::KeyPress() && haveFocus, &Input::handleInput);
    connect(M::ButtonRelease(B::Left), &Input::checkFocus);
}

void Input::update()
{
    using M = tank::Mouse;
    if (not hasFocus_) {
        if (M::isInEntity(*this)) {
            box_->setFillColor(hover);
        }
        else {
            box_->setFillColor(normal);
        }
    }
}

void Input::mouseOver()
{
    box_->setFillColor(hover);
}

//void Input::mouseOut()
//{
    //box_->setFillColor({200,200,200});
//}

void Input::onRelease()
{
    hasFocus_ = true;
}

void Input::handleInput()
{
    std::string s;
    tank::Game::keystream >> s;
    std::cout << s << std::endl;
    tank::Game::keystream.str("");
    tank::Game::keystream.clear();
}

void Input::checkFocus()
{
    using M = tank::Mouse;
    if (M::isInEntity(*this)) {
        gainFocus();
    } else {
        loseFocus();
    }
}

void Input::gainFocus()
{
    tank::Game::keystream.str("");
    tank::Game::keystream.clear();
    hasFocus_ = true;
    box_->setFillColor(focus);
    box_->setOutlineThickness(1);
}

void Input::loseFocus()
{
    hasFocus_ = false;;
    box_->setFillColor(normal);
    box_->setOutlineThickness(0);
}
