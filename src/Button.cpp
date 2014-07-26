#include "Button.hpp"

#include <Tank/System/Keyboard.hpp>
#include <Tank/System/Mouse.hpp>
#include "StartWorld.hpp"

Button::Button(tank::Vectorf pos,
               tank::Vectoru size,
               std::string label,
               std::function<void()> callback)
    : tank::Entity(pos)
    , callback_ {callback}
{
    box_ = makeGraphic<tank::RectangleShape>(size);
    label_ = makeGraphic<tank::Text>(StartWorld::font);
    label_->setColor({});

    box_->setOutlineColor({200,200,200});
    setFontSize(18);
    setLabel(label);
}

void Button::setLabel(std::string label)
{
    label_->setText(label);
    label_->setPos(tank::Vectori(box_->getSize() / 2));
    label_->setOrigin(tank::Vectori((label_->getSize() / 2) + tank::Vectorf{0, 4}));
}

void Button::setFontSize(unsigned s)
{
    label_->setFontSize(s);
}

void Button::onAdded()
{
    using K = tank::Keyboard;
    using Key = tank::Key;
    using M = tank::Mouse;
    using B = M::Button;

    auto haveFocus = [&]() { return hasFocus_; };
    connect(K::KeyPress(Key::Return) && haveFocus, &Button::onRelease);
    connect(M::ButtonRelease(B::Left) && M::InEntity(*this), &Button::onRelease);
    connect(M::ButtonPress(B::Left) && M::InEntity(*this), &Button::onClick);
}

void Button::update()
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

void Button::onClick()
{
    box_->setFillColor(active);
}

void Button::onRelease()
{
    gainFocus();
    if (callback_) callback_();
}

void Button::gainFocus()
{
    if (hasFocus_) return;

    hasFocus_ = true;
    box_->setFillColor(focus);
    box_->setOutlineThickness(1);
}

void Button::loseFocus()
{
    if (not hasFocus_) return;

    hasFocus_ = false;;
    box_->setFillColor(normal);
    box_->setOutlineThickness(0);
}
