#include "Input.hpp"

#include <Tank/System/Game.hpp>
#include <Tank/System/Keyboard.hpp>
#include <Tank/System/Mouse.hpp>
#include "StartWorld.hpp"

Input::Input(tank::Vectorf pos,
             tank::Vectoru size,
             std::string label,
             std::function<void()> callback)
    : tank::Entity(pos)
    , callback_ {callback}
{
    box_ = makeGraphic<tank::RectangleShape>(size);
    text_ = makeGraphic<tank::Text>(StartWorld::font);
    label_ = makeGraphic<tank::Text>(StartWorld::titleFont);

    text_->setColor({});
    text_->setOrigin({-5, 2});
    box_->setOutlineColor({200,200,200});
    setFontSize(size.y - 2);
    setLabel(label);
}

void Input::setText(std::string text)
{
    text_->setText(text);
    input_ = text;
}

void Input::setLabel(std::string label)
{
    label_->setText(label);
    label_->setOrigin({label_->getSize().x + 5, 2});
}

void Input::setFontSize(unsigned s)
{
    label_->setFontSize(s);
    text_->setFontSize(s);
}

void Input::onAdded()
{
    using K = tank::Keyboard;
    using Key = tank::Key;
    using M = tank::Mouse;
    using B = M::Button;

    auto haveFocus = [&]() { return hasFocus_; };
    //auto noFocus = [&]() { return not hasFocus_; };
    connect(K::KeyPress() && haveFocus, &Input::handleInput);
    connect(K::KeyPress(Key::Return) && haveFocus,
            [this]{ if (callback_) { callback_(); }});
    connect(M::ButtonRelease(B::Left), &Input::checkFocus);
}

void Input::update()
{
    using M = tank::Mouse;
    if (M::isInEntity(*this)) {
        box_->setFillColor(hover);
    }
    else {
        if (hasFocus_) {
            box_->setFillColor(focus);
        } else {
            box_->setFillColor(normal);
        }
    }
}

//void Input::mouseOut()
//{
    //box_->setFillColor({200,200,200});
//}

void Input::handleInput()
{
    // FIXME: This assumes ASCII -> is not portable
    char c;
    while (tank::Game::keystream >> c) {
        try {
            if (c < 0x20 or c >= 0x7F) { // Outside display char range
                switch (c) {
                    case 0x15: // CTRL + w //TODO: "delete word"
                    case 0x17: // CTRL + u
                        input_.clear();
                        break;
                    case 0x1B: // ESC
                        loseFocus();
                        break;
                    case 0x08: // BS / CTRL + h
                        if (tank::Keyboard::control() and
                            tank::Keyboard::isKeyDown(tank::Key::BackSpace)) {
                            input_.clear(); // TODO: "delete word"
                            break;
                        }
                    case 0x7F: // DEL
                        input_.pop_back();
                        break;
                    default:
                        std::cout << static_cast<unsigned>(c) << std::endl;
                        break;
                }
            } else {
                input_.push_back(c);
            }
        }
        catch (std::out_of_range const& e) { }
    }

    text_->setText(input_);
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
    if (hasFocus_) return;

    tank::Game::keystream.str("");
    tank::Game::keystream.clear();
    hasFocus_ = true;
    box_->setFillColor(focus);
    box_->setOutlineThickness(1);
}

void Input::loseFocus()
{
    if (not hasFocus_) return;

    tank::Game::keystream.str("");
    tank::Game::keystream.clear();
    hasFocus_ = false;;
    box_->setFillColor(normal);
    box_->setOutlineThickness(0);
}
