#ifndef INDICATOR_HPP
#define INDICATOR_HPP

#include <Tank/System/Entity.hpp>
#include <Tank/Graphics/CircleShape.hpp>
#include "Stone.hpp"

class Indicator : public tank::Entity
{
    tank::observing_ptr<tank::CircleShape> indicator_;
public:
    Indicator(tank::Vectorf pos, std::string label);

    void setColor(Stone);
};

#endif /* INDICATOR_HPP */
