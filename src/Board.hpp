#include <Tank/System/Entity.hpp>
#include <Tank/Utility/CollisionGrid.hpp>

class Board : public tank::Entity
{
    enum Stone {
        White,
        Black,
        Empty
    };

    tank::CollisionGrid grid_;
    //unsigned size_;

public:
    static constexpr unsigned stoneSize = 20;

    virtual void onAdded() override;
    void mouseOver();
    void onClick();
    Board(unsigned size = 19);
};
