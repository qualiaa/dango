#include <Tank/System/Entity.hpp>
#include <Tank/Utility/Grid.hpp>
#include <Tank/Graphics/CircleShape.hpp>

class Board : public tank::Entity
{
private:
    enum Stone {
        White,
        Black,
        Empty
    };

    tank::Grid<Stone> grid_;
    tank::observing_ptr<tank::CircleShape> cursor_;
    bool isIn_ {false}, wasIn_ {false};

    void hideCursor();
    //unsigned size_;

public:
    static constexpr unsigned stoneSize = 19;

    virtual void onAdded() override;
    virtual void update() override;
    void mouseOver();
    void onClick();
    Board(unsigned size = 19);
};
