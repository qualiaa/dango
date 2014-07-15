#include "MainWorld.hpp"

#include <boost/system/system_error.hpp>
#include "Board.hpp"

MainWorld::MainWorld()
{
    makeEntity<Board>();
    try {
        c.connect("127.1","8124");
    }
    catch (boost::system::system_error const& e) {
        std::cerr << e.what() << std::endl;
    }
}
