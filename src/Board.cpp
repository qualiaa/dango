#include "Board.hpp"

#include <iostream>
#include <Tank/System/Mouse.hpp>
#include <Tank/Graphics/RectangleShape.hpp>

Board::Board(unsigned size)
  : grid_(tank::Vectoru{size,size})
  //, size_(size)
{
    const auto pixelSize = stoneSize * size;
    auto g = makeGraphic<tank::RectangleShape>(tank::Vectoru{pixelSize,pixelSize});
    g->setFillColor({0,125,0});
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
    std::cout << "Mouse over" << std::endl;
}

void Board::onClick()
{
    std::cout << "Mouse click" << std::endl;
}
