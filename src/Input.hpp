#ifndef INPUT_HPP
#define INPUT_HPP

#include <Tank/System/Entity.hpp>
#include <Tank/Graphics/Text.hpp>
#include <Tank/Graphics/RectangleShape.hpp>

class Input : public tank::Entity
{
    bool hasFocus_ {false};
    tank::observing_ptr<tank::RectangleShape> box_;
    tank::observing_ptr<tank::Text> text_;
    tank::observing_ptr<tank::Text> label_;

    tank::Color normal {150,150,150};
    tank::Color focus {100,100,100};
    tank::Color hover {200,200,200};

public:
    virtual void onAdded() override;
    virtual void update() override;
    //virtual void draw(tank::Camera const&) override;

    void setLabel(std::string label);
    void setFontSize(unsigned);
    unsigned getFontSize();

    void mouseOver();
    void onClick();
    void onRelease();

    void gainFocus();
    void loseFocus();

    Input(tank::Vectorf, tank::Vectoru size, std::string label = "");

private:
    void checkFocus();
    void handleInput();
};

#endif /* INPUT_HPP */
