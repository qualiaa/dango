#include "MainWorld.hpp"

#include <functional>
#include <istream>
#include <vector>
#include <boost/system/system_error.hpp>
#include <Tank/System/Game.hpp>
#include "Mutex.hpp"

const std::string MainWorld::messageDelim {"\r\n"};

MainWorld::MainWorld()
    : io_(new boost::asio::io_service)
    , c(io_)
{
    try {
        c.connect("127.1","8124");
        c.async_read_until(messageDelim,
                           std::bind(&MainWorld::connectionHandler,
                                     this,
                                     &c,
                                     std::placeholders::_1,
                                     std::placeholders::_2));
        board_ = makeEntity<Board>(c);

        connectionThread_ = std::thread(&MainWorld::threadFunc, this);
        connectionThread_.detach();
    }
    catch (std::exception const& e) {
        std::cerr << "Exception while creating MainWorld: ";
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
        std::cerr << "Exception while closing MainWorld: "
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

    while (true) {
        try {
            mutex.lock();
            std::cerr << "[" << std::this_thread::get_id() << "] ";
            std::cerr << "Doing work" << std::endl;
            mutex.unlock();
            boost::system::error_code ec;
            io_->run(ec);
            if (ec) {
                mutex.lock();
                std::cerr << "[" << std::this_thread::get_id() << "] ";
                std::cerr << "Error: " << ec << std::endl;
                mutex.unlock();
            }
            break;
        }
        catch (std::exception const& e) {
            std::cerr << "[" << std::this_thread::get_id() << "] ";
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }

    mutex.lock();
    std::cout << "[" << std::this_thread::get_id() << "] Thread closing" << std::endl;
    mutex.unlock();

}

void MainWorld::connectionHandler(Connection *c,
                                  boost::system::error_code const& ec,
                                  size_t bytes)
{
    if (!ec) {
        mutex.lock();

        std::istream is {c->streamBuffer()};
        std::vector<uint8_t> data(bytes);
        is.read(reinterpret_cast<char*>(&data[0]), bytes);
        for (auto byte : data) {
            std::cout << std::hex << static_cast<unsigned>(byte) << " ";
        }
        std::cout << std::dec <<  std::endl;

        std::cout << "Handling input of size " << bytes << std::endl;

        // TODO Error checking
        switch (data[0]) {
        case 't': //change turn
            std::cout << "Switching turn" << std::endl;
            currentTurn_ = not currentTurn_;
            break;
        case 'c': //connection response
            std::cout << "Connection accepted" << std::endl;
            switch (data[1]) {
            case 'w': player_ = White; break;
            case 'b': player_ = Black; break;
            default:
                break;
            }
        case 's': {//set stone
            if (bytes - 2 < sizeof(char)*2 + sizeof(tank::Vectoru)) {
                std::cerr << "SET: unexpected number of bytes" << std::endl;
            }
            tank::Vectoru pos;
            std::memcpy(&pos, &data[2], sizeof(pos));
            try {
                switch (data[1]) {
                case 'w': board_->setStone(pos, White); break;
                case 'b': board_->setStone(pos, Black); break;
                case 'e': board_->setStone(pos, Empty); break;
                default:
                    std::cerr << "SET: unexpected data" << std::endl;
                    break;
                }
            }
            catch (std::exception const& e) {
                std::cerr << "Exception: " << e.what() << std::endl;
            }
            break;
        }
        default:
            std::cerr << "Error: unexpected header: "
                            << static_cast<unsigned>(data[0]) << std::endl;
            break;
        }

        mutex.unlock();
    } else {
        std::cerr << "Error: " << ec << std::endl;
    }

    /*
    auto rb = c->readBuffer();

    if (ec) {
        std::cerr << "Error: " << ec << std::endl;
        return;
    }


    mutex.lock();
    std::cout << "Handling input of size " << bytes << std::endl;

    // TODO Error checking
    switch (rb[0]) {
    case 't': //change turn
        std::cout << "Switching turn" << std::endl;
        currentTurn_ = not currentTurn_;
        break;
    case 'c': //connection response
        std::cout << "Connection accepted" << std::endl;
        switch (rb[1]) {
        case 'w': player_ = White; break;
        case 'b': player_ = Black; break;
        default:
            break;
        }
    case 's': //set stone
        {
        tank::Vectoru pos {rb[1], rb[2]};
        std::cout << "Stone write" << pos << std::endl;
        switch (rb[3]) {
        case 'w': board_->setStone(pos, White); break;
        case 'b': board_->setStone(pos, Black); break;
        case 'e': board_->setStone(pos, Empty); break;
        default:
            std::cerr << "Error: unexpected data" << std::endl;
            break;
        }
        }
        std::cout << std::endl;
        break;
    default:
        std::cerr << "Error: unexpected header" << std::endl;
        break;
    }

    mutex.unlock();
    */

    c->async_read_until(messageDelim,
                        std::bind(&MainWorld::connectionHandler,
                                  this,
                                  c,
                                  std::placeholders::_1,
                                  std::placeholders::_2));
}
