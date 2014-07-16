#ifndef BOARD_HPP
#define BOARD_HPP

#include <Tank/System/Entity.hpp>
#include <array>
#include <Tank/Utility/Grid.hpp>
#include <Tank/Graphics/CircleShape.hpp>
#include "Connection.hpp"

enum Stone {
    Black,
    White,
    Empty
};

class Board : public tank::Entity
{
public:
    static constexpr unsigned stoneSize = 19;

private:
    Connection& c_;

    tank::Grid<Stone> grid_;
    tank::observing_ptr<tank::CircleShape> cursor_;
    std::array<tank::CircleShape, 3> stone_;
    bool currentPlayer_ {Black};

    bool isIn_ {false}, wasIn_ {false};

public:
    virtual void onAdded() override;
    virtual void update() override;
    virtual void draw(tank::Camera const&) override;

    void mouseOver();
    void onClick();
    void onRelease();

    void setStone(tank::Vectoru, Stone);

    Board(Connection& c, unsigned size = 19);

private:
    void hideCursor();
};

#endif /* BOARD_HPP */
