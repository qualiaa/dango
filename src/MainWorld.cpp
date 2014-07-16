#include "MainWorld.hpp"

#include <functional>
#include <boost/system/system_error.hpp>
#include <Tank/System/Game.hpp>
#include "Mutex.hpp"

MainWorld::MainWorld()
    : io_(new boost::asio::io_service)
    , c(io_)
{
    try {
        c.connect("127.1","8124");
        c.async_read_some(std::bind(&MainWorld::connectionHandler, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        board_ = makeEntity<Board>(c);

        connectionThread_ = std::thread(&MainWorld::threadFunc, this);
        connectionThread_.detach();
    }
    catch (std::exception const& e) {
        std::cerr << e.what() << std::endl;
        std::cerr << "Closing prematurely" << std::endl;
        tank::Game::stop();
    }
}

MainWorld::~MainWorld()
{
    try {
        boost::system::error_code ec;
        io_->stop();
    }
    catch (std::exception const& e) {
        std::cerr << "Exception caught while closing MainWorld: "
                        << e.what() << std::endl;
    }

    if (connectionThread_.joinable()) {
        connectionThread_.join();
    }
}

void MainWorld::threadFunc()
{
    mutex.lock();
    std::cout << "[" << std::this_thread::get_id() << "] Thread starting" << std::endl;
    mutex.unlock();

    while (true)
    try {
        boost::system::error_code ec;
        io_->run(ec);
        if (ec) {
            std::cerr << "Error: " << ec << std::endl;
        }
        break;
    }
    catch (std::exception const& e) {
        std::cerr << e.what() << std::endl;
    }

    mutex.lock();
    std::cout << "[" << std::this_thread::get_id() << "] Thread closing" << std::endl;
    mutex.unlock();

}

void MainWorld::connectionHandler(Connection *c,
                                  boost::system::error_code const& ec,
                                  size_t bytes)
{
    auto const& rb = c->readBuffer();

    if (ec) {
        std::cerr << "Error: " << ec << std::endl;
        return;
    }


    mutex.lock();
    std::cout << "Handling input: ";
    std::cout.write(rb.data(),bytes);
    std::cout << std::endl;
    mutex.unlock();

    // TODO Error checking
    switch (rb[0]) {
    case 't': //change turn
        currentTurn_ = not currentTurn_;
    case 'c': //connection
        switch (rb[1]) {
        case 'w': player_ = White; break;
        case 'b': player_ = Black; break;
        default:
            break;
        }
    case 's': //set stone
        mutex.lock();
        {
        tank::Vectoru pos {rb[1], rb[2]};
        std::cout << "\nWriting stone to " << pos << std::endl;
        switch (rb[3]) {
        case 'w': board_->setStone(pos, White); std::cout << "White"; break;
        case 'b': board_->setStone(pos, Black); std::cout << "Black"; break;
        case 'e': board_->setStone(pos, Empty); std::cout << "Empty"; break;
        default:
            std::cerr << "Error: unexpected data" << std::endl;
            break;
        }
        }
        std::cout << std::endl;
        mutex.unlock();
        break;
    default:
        std::cerr << "Error: unexpected header" << std::endl;
        break;
    }

    c->async_read_some(std::bind(&MainWorld::connectionHandler,
                                         this,
                                         std::placeholders::_1,
                                         std::placeholders::_2,
                                         std::placeholders::_3));
}
