#include "Board.hpp"

#include <cmath>
#include <Tank/System/Mouse.hpp>
#include <Tank/Graphics/RectangleShape.hpp>
#include "Mutex.hpp"

Board::Board(Connection& c, unsigned size)
  : tank::Entity({40,40})
  , connection_(c)
  , grid_(tank::Vectoru{size,size}, Empty)
{
    using tank::RectangleShape;

    const auto pixelSize = stoneSize * size;
    auto g = makeGraphic<tank::RectangleShape>(tank::Vectoru{pixelSize,pixelSize});
    g->setFillColor({0,125,0});

    const unsigned stoneRadius = /*std::ceil*/(stoneSize/2.0);

    for (unsigned i = 0; i < size; ++i) {
        const unsigned offset = i * stoneSize + stoneRadius;
        makeGraphic<RectangleShape>(tank::Rectu{0, offset, pixelSize, 1})->setFillColor({});
        makeGraphic<RectangleShape>(tank::Rectu{offset, 0, 1, pixelSize})->setFillColor({});
    }

    cursor_ = makeGraphic<tank::CircleShape>(stoneRadius);
    cursor_->setFillColor({0,0,0,40});
    hideCursor();

    stone_[White] = tank::CircleShape(stoneRadius);
    stone_[Black] = tank::CircleShape(stoneRadius);
    stone_[Empty] = tank::CircleShape(0);
    stone_[Black].setFillColor({});
}

void Board::onAdded()
{
    using M = tank::Mouse;
    using B = M::Button;
    connect(M::InEntity(*this), &Board::mouseOver);
    connect(M::InEntity(*this) && M::ButtonPress(B::Left), &Board::onClick);
    connect(M::InEntity(*this) && M::ButtonRelease(B::Left), &Board::onRelease);
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

    tank::Vector<char> tilePos = {
        static_cast<char>(std::floor((mPos.x) / stoneSize)),
        static_cast<char>(std::floor((mPos.y) / stoneSize))
    };

    char player = currentPlayer_ == Black ? 'b' : 'w';
    std::string s;
    s.push_back('s');
    s.push_back(tilePos.x);
    s.push_back(tilePos.y);
    s.push_back(player);
    connection_.write(s);
    /*
    mutex.lock();
    setStone(tilePos, static_cast<Stone>(currentPlayer));
    mutex.unlock();
    */

    currentPlayer_ = not currentPlayer_;

    uint8_t c = currentPlayer_ * 255;
    cursor_->setFillColor({c,c,c,40});
}

void Board::onClick()
{
    uint8_t c = currentPlayer_ * 255;
    cursor_->setFillColor({c,c,c,100});
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
    if (pos.x > grid_.getWidth() or pos.y > grid_.getHeight()) {
        throw std::invalid_argument("Stone placed outside of board");
    }
    grid_[pos] = s;
}

void Board::draw(tank::Camera const& camera)
{
    tank::Entity::draw(camera);

    tank::Vectoru pos;
    const auto ss = stoneSize;

    mutex.lock();
    for (pos.y = 0; pos.y < grid_.getHeight(); ++pos.y) {
        for (pos.x = 0; pos.x < grid_.getWidth(); ++pos.x) {
            //draw the appropriate stone flyweight at pos
            stone_[grid_[pos]].draw(pos*ss + getPos(), 0, {}, camera);
        }
    }
    mutex.unlock();
}

void Board::hideCursor()
{
    cursor_->setPos({-stoneSize, -stoneSize});
}
