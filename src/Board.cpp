#include "Board.hpp"

#include <cmath>
#include <Tank/System/Mouse.hpp>
#include <Tank/Graphics/RectangleShape.hpp>
#include "Message.hpp"

Board::Board(Connection& c, unsigned size)
  : tank::Entity({border,border})
  , connection_(c)
  , board_(tank::Vectoru{size,size}, Empty)
  , marks_(tank::Vectoru{size,size}, false)
{
    using tank::RectangleShape;

    // Create board rectangle
    const auto pixelSize = stoneSize * size;
    auto g = makeGraphic<tank::RectangleShape>(tank::Vectoru{pixelSize,pixelSize});
    g->setFillColor({0,125,0});

    const unsigned stoneRadius = /*std::ceil*/(stoneSize/2.0);

    // Create board lines
    for (unsigned i = 0; i < size; ++i) {
        const unsigned offset = i * stoneSize + stoneRadius;
        makeGraphic<RectangleShape>(tank::Rectu{0, offset, pixelSize, 1})
            ->setFillColor({});
        makeGraphic<RectangleShape>(tank::Rectu{offset, 0, 1, pixelSize})
            ->setFillColor({});
    }

    // Create hoshi
    tank::CircleShape hoshi(3);
    hoshi.setOrigin(hoshi.getSize() / 2);
    hoshi.setFillColor({});
    unsigned reducedSize = size - 6;
    if (size % 2 == 1 and reducedSize <= size) {
        for (unsigned i = 0; i < 3; ++i) {
            unsigned x = (i * std::floor(reducedSize / 2)) + 3;
            for (unsigned j = 0; j < 3; ++j) {
                unsigned y = (j * std::floor(reducedSize / 2)) + 3;
                auto ss = stoneSize;
                makeGraphic<tank::CircleShape>(hoshi)
                    ->setPos({x*ss + stoneRadius, y*ss + stoneRadius});
            }
        }
    }

    // Create cursor
    cursor_ = makeGraphic<tank::CircleShape>(stoneRadius);
    hideCursor();

    // Create mark and stone flyweights
    mark_.setSize({stoneRadius,stoneRadius});
    mark_.setFillColor({127,127,127});
    mark_.setPos(tank::Vectori(mark_.getSize() / 2));

    stoneFlyweights_[White] = tank::CircleShape(stoneRadius);
    stoneFlyweights_[Black] = tank::CircleShape(stoneRadius);
    stoneFlyweights_[Empty] = tank::CircleShape(0);
    stoneFlyweights_[Black].setFillColor({});
}

void Board::onAdded()
{
    using M = tank::Mouse;
    using B = M::Button;
    connect(M::InEntity(*this), &Board::mouseOver);
    connect(M::InEntity(*this) && M::ButtonPress(B::Left), &Board::onClick);
    connect(M::InEntity(*this) && M::ButtonRelease(), &Board::onRelease);
}

void Board::mouseOver()
{
    using M = tank::Mouse;
    auto mPos = M::getPos() - getPos();

    tank::Vectoru tilePos = {
        static_cast<unsigned>(std::floor((mPos.x) / stoneSize)),
        static_cast<unsigned>(std::floor((mPos.y) / stoneSize))
    };

    const auto ss = stoneSize;
    cursor_->setPos(tilePos * ss);

    isIn_ = true;
}

void Board::onRelease()
{
    using M = tank::Mouse;
    auto mPos = M::getPos() - getPos();

    tank::Vector<unsigned> tilePos = {
        static_cast<unsigned>(std::floor((mPos.x) / stoneSize)),
        static_cast<unsigned>(std::floor((mPos.y) / stoneSize))
    };

    if (M::isButtonReleased(M::Button::Left)) {
        placeStone(tilePos);
    } else if (M::isButtonReleased(M::Button::Right)) {
        placeMark(tilePos);
    }

    auto c = cursor_->getFillColor();
    c.a = 40;
    cursor_->setFillColor(c);
}

void Board::placeStone(tank::Vectoru pos)
{
    // Set up message
    boost::array<char, sizeof(pos) + 1> data;
    data[0] = Message::SET;
    std::memcpy(&data[1],  &pos, sizeof(pos));
    // Send message
    connection_.write(data, data.size());
}

void Board::placeMark(tank::Vectoru pos)
{
    // Set up message
    boost::array<char, sizeof(pos) + 1> data;
    data[0] = Message::MARK;
    std::memcpy(&data[1],  &pos, sizeof(pos));
    // Send message
    connection_.write(data, data.size());
}

void Board::onClick()
{
    auto c = cursor_->getFillColor();
    c.a = 100;
    cursor_->setFillColor(c);
}

void Board::update()
{
    if (wasIn_ and not isIn_) {
        hideCursor();
    }
    wasIn_ = isIn_;
    isIn_ = false;
}

void Board::setStone(tank::Vectoru pos, Stone s)
{
    if (pos.x > board_.getWidth() or pos.y > board_.getHeight()) {
        throw std::invalid_argument("Stone placed outside of board");
    }
    board_[pos] = s;
}

void Board::setMark(tank::Vectoru pos, bool mark) {
    if (pos.x > board_.getWidth() or pos.y > board_.getHeight()) {
        throw std::invalid_argument("Mark placed outside of board");
    }

    marks_[pos] = mark;
}

void Board::draw(tank::Camera const& camera)
{
    tank::Entity::draw(camera);

    tank::Vectoru pos;
    const auto ss = stoneSize;

    for (pos.y = 0; pos.y < board_.getHeight(); ++pos.y) {
        for (pos.x = 0; pos.x < board_.getWidth(); ++pos.x) {
            // draw the appropriate stone flyweight at pos
            stoneFlyweights_[board_[pos]].draw(pos*ss + getPos(), 0, {}, camera);
            if (marks_[pos]) {
                // mark any marked stones
                mark_.draw(pos*ss + getPos(), 0, {}, camera);
            }
        }
    }
}

void Board::setCursor(Stone s)
{
    cursor_->setVisible(true);
    if (s == Black) {
        cursor_->setFillColor({0,0,0,40});
    } else if (s == White) {
        cursor_->setFillColor({255,255,255, 40});
    } else {
        cursor_->setVisible(false);
    }
}

void Board::hideCursor()
{
    cursor_->setPos({-stoneSize, -stoneSize});
}
