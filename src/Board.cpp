#include "Board.hpp"

#include <iostream>
#include <cmath>
#include <Tank/System/Mouse.hpp>
#include <Tank/Graphics/RectangleShape.hpp>

Board::Board(unsigned size)
  : tank::Entity({10,10})
  , grid_(tank::Vectoru{size,size}, Empty)
  //, size_(size)
{
    const auto pixelSize = stoneSize * size;
    auto g = makeGraphic<tank::RectangleShape>(tank::Vectoru{pixelSize,pixelSize});
    g->setFillColor({0,125,0});

    const unsigned stoneRadius = std::ceil(stoneSize/2.0);
    cursor_ = makeGraphic<tank::CircleShape>(stoneRadius);
    cursor_->setOrigin({stoneRadius,stoneRadius});
}

void Board::onAdded()
{
    using M = tank::Mouse;
    using B = M::Button;
    connect(M::InEntity(*this), &Board::mouseOver);
    connect(M::InEntity(*this) && M::ButtonPress(B::Left), &Board::onClick);
}

void Board::mouseOver()
{
    using M = tank::Mouse;
    auto mPos = M::getPos();

    tank::Vectoru tilePos = { 
        (mPos.x / stoneSize),
        (mPos.y / stoneSize)
    };

    const auto ss = stoneSize;
    cursor_->setPos(tilePos * ss);

    isIn_ = true;

    std::cout << tilePos << std::endl;
}

void Board::onClick()
{
    std::cout << "Mouse click" << std::endl;
}

void Board::update()
{
    if (wasIn_ and not isIn_) {
        hideCursor();
    }
    wasIn_ = isIn_;
    isIn_ = false;
}

void Board::hideCursor()
{
    cursor_->setPos({-stoneSize, -stoneSize});
}
