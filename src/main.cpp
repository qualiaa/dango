#include <Tank/System/Game.hpp>
#include "StartWorld.hpp"


int main()
{
    using namespace tank;
    Game::initialize({640,480});
    Game::makeWorld<StartWorld>();
    Game::run();
}
