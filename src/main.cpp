#include <Tank/System/Game.hpp>
#include "MainWorld.hpp"

int main()
{
    using namespace tank;
    Game::initialize({640,480});
    Game::makeWorld<MainWorld>();
    Game::run();
}
