#ifndef BUTTON_HPP
#define BUTTON_HPP

#include <functional>
#include <Tank/System/Entity.hpp>
#include <Tank/Graphics/Text.hpp>
#include <Tank/Graphics/RectangleShape.hpp>

class Button : public tank::Entity
{
    bool hasFocus_ {false};
    tank::observing_ptr<tank::RectangleShape> box_;
    tank::observing_ptr<tank::Text> label_;
    std::function<void()> callback_;

    tank::Color normal {150,150,150};
    tank::Color focus {100,100,100};
    tank::Color hover {200,200,200};
    tank::Color active {255,255,255};

public:
    virtual void onAdded() override;
    virtual void update() override;
    //virtual void draw(tank::Camera const&) override;

    void setCallback(std::function<void()> c) { callback_ = c; }
    void setLabel(std::string label);
    std::string getLabel() const { return label_->getText(); }
    void setFontSize(unsigned);
    unsigned getFontSize() const { return label_->getFontSize(); }

    void onRelease();
    void onClick();

    void gainFocus();
    void loseFocus();
    bool hasFocus() const { return hasFocus_; }

    Button(tank::Vectorf,
           tank::Vectoru size,
           std::string label = "",
           std::function<void()> callback = []{});
};

#endif /* BUTTON_HPP */
