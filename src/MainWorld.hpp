#ifndef MAINWORLD_HPP
#define MAINWORLD_HPP

#include <Tank/System/World.hpp>
#include "Connection.hpp"

class MainWorld : public tank::World
{
    Connection c;

public:
    MainWorld();
};

#endif /* MAINWORLD_HPP */
