#ifndef INPUT_HPP
#define INPUT_HPP

#include <functional>
#include <Tank/System/Entity.hpp>
#include <Tank/Graphics/Text.hpp>
#include <Tank/Graphics/RectangleShape.hpp>

// TODO: Template this for typed input
// TODO: Allow writing to reference
// TODO: Restricted charsets
// TODO: Cursor
class Input : public tank::Entity
{
    bool hasFocus_ {false};
    tank::observing_ptr<tank::RectangleShape> box_;
    tank::observing_ptr<tank::Text> text_;
    tank::observing_ptr<tank::Text> label_;
    std::function<void()> callback_;

    tank::Color normal {150,150,150};
    tank::Color focus {100,100,100};
    tank::Color hover {200,200,200};

    std::string input_;

public:
    virtual void onAdded() override;
    virtual void update() override;
    //virtual void draw(tank::Camera const&) override;

    void setText(std::string text);
    std::string getText() const{ return text_->getText(); }
    void setLabel(std::string label);
    std::string getLabel() const { return label_->getText(); }
    void setFontSize(unsigned);
    unsigned getFontSize() const { return label_->getFontSize(); }

    void gainFocus();
    void loseFocus();
    bool hasFocus() const { return hasFocus_; }

    Input(tank::Vectorf,
          tank::Vectoru size,
          std::string label = "",
          std::function<void()> callback = []{});

private:
    void handleInput();
    void checkFocus();
};

#endif /* INPUT_HPP */
