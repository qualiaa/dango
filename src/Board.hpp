#ifndef BOARD_HPP
#define BOARD_HPP

#include <array>
#include <Tank/System/Entity.hpp>
#include <Tank/Utility/Grid.hpp>
#include <Tank/Graphics/CircleShape.hpp>
#include <Tank/Graphics/RectangleShape.hpp>
#include "Connection.hpp"
#include "Stone.hpp"

class Board : public tank::Entity
{
public:
    static constexpr unsigned border = 40;
    static constexpr unsigned stoneSize = 19;

private:
    Connection& connection_;

    tank::Grid<Stone> board_;
    tank::Grid<bool> marks_;
    tank::observing_ptr<tank::CircleShape> cursor_;
    std::array<tank::CircleShape, 3> stoneFlyweights_;
    tank::RectangleShape mark_;

    bool isIn_ {false}, wasIn_ {false};

public:
    virtual void onAdded() override;
    virtual void update() override;
    virtual void draw(tank::Camera const&) override;

    void setStone(tank::Vectoru, Stone);
    void setMark(tank::Vectoru, bool mark);
    void setCursor(Stone);

    Board(Connection& c, unsigned size = 19);

private:
    void hideCursor();

    void mouseOver();
    void onClick();
    void onRelease();

    void placeStone(tank::Vectoru pos);
    void placeMark(tank::Vectoru pos);
};

#endif /* BOARD_HPP */
