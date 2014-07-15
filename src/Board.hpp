#ifndef BOARD_HPP
#define BOARD_HPP

#include <Tank/System/Entity.hpp>
#include <array>
#include <Tank/Utility/Grid.hpp>
#include <Tank/Graphics/CircleShape.hpp>

class Board : public tank::Entity
{
private:
    enum Stone {
        Black,
        White,
        Empty
    };

    tank::Grid<Stone> grid_;
    tank::observing_ptr<tank::CircleShape> cursor_;
    std::array<tank::CircleShape, 3> stone_;
    bool currentPlayer {Black};

    bool isIn_ {false}, wasIn_ {false};

    void hideCursor();

public:
    static constexpr unsigned stoneSize = 19;

    virtual void onAdded() override;
    virtual void update() override;
    virtual void draw(tank::Camera const&) override;

    void mouseOver();
    void onClick();
    void onRelease();
    Board(unsigned size = 19);
};

#endif /* BOARD_HPP */
