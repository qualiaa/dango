#include <Tank/System/Game.hpp>
#include "StartWorld.hpp"


int main()
{
    using namespace tank;
    Game::initialize({640,480});
    Game::window()->setCaption("dango");
    Game::window()->setBackgroundColor({0,29,0});
    Game::makeWorld<StartWorld>();
    Game::run();
}
