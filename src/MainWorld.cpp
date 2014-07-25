#include "MainWorld.hpp"

#include <functional>
#include <istream>
#include <vector>
#include <boost/system/system_error.hpp>
#include <Tank/System/Game.hpp>
#include "Message.hpp"
#include "Mutex.hpp"

MainWorld::MainWorld(std::string hostname, std::string port)
    : io_(new boost::asio::io_service)
    , connection_(io_)
    , hostname_(hostname)
    , port_(port)
{
    using Pos = tank::Vectorf;
    turnIndicator_ = makeEntity<Indicator>(Pos{500, 60},"Current Turn");
    playerIndicator_ = makeEntity<Indicator>(Pos{500, 200},"You Are");

    blackScore_ = makeEntity<Score>(Pos{425, 340}, "Black:");
    whiteScore_ = makeEntity<Score>(Pos{425, 375}, "White:");
}

void MainWorld::onAdded()
{
    try {
        connection_.connect(hostname_, port_);
        connection_.async_read_until(Message::delimiter,
                                     std::bind(&MainWorld::connectionHandler,
                                               this,
                                               &connection_,
                                               std::placeholders::_1,
                                               std::placeholders::_2));
        board_ = makeEntity<Board>(connection_);

        connectionThread_ = std::thread(&MainWorld::threadFunc, this);
        connectionThread_.detach();
    }
    catch (std::exception const& e) {
        std::cerr << "Exception while creating MainWorld: ";
        std::cerr << e.what() << std::endl;
        std::cerr << "Returning to menu" << std::endl;
        tank::Game::popWorld();
    }
}

void MainWorld::draw()
{
    mutex.lock();
    tank::World::draw();
    mutex.unlock();
}

MainWorld::~MainWorld()
{
    try {
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
    while (true) {
        try {
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

void MainWorld::connectionHandler(Connection *connection,
                                  boost::system::error_code const& ec,
                                  size_t bytes)
{
    if (!ec) {
        mutex.lock();

        // Create an instream from the connection stream buffer
        std::istream instream {connection->streamBuffer()};
        // Create a vector to store data of size `bytes`
        std::vector<char> data(bytes);
        // Read the data into the vector
        instream.read(&data[0], bytes);
        // print data
        std::cout << "Handling " << bytes << " bytes: < " << std::hex;
        for (auto byte : data) {
            std::cout << static_cast<unsigned>(byte) << " ";
        }
        std::cout << ">" << std::dec <<  std::endl;

        Message message{std::move(data)};

        // TODO Error checking
        // PLAYER
        if (message.header == Message::PLAYER) {
            switch (message.data[0]) {
                case 'w': player_ = White; break;
                case 'b': player_ = Black; break;
                default: player_ = Empty; break;
            }
            playerIndicator_->setColor(player_);
            board_->setCursor(player_);
        }
        // BOARD
        else if (message.header == Message::BOARD) {
            if (message.data.size() != sizeof(unsigned)) {
                std::cerr << "BOARD: unexpected number of bytes" << std::endl;
            } else {
                const unsigned size = *(&message.data[0]);
                board_->remove();
                board_ = makeEntity<Board>(*connection, size);
                board_->setCursor(player_);
            }
        }
        // TURN
        else if (message.header == Message::TURN) {
            switch (message.data[0]) {
                case 'w': turnIndicator_->setColor(White); break;
                case 'b': turnIndicator_->setColor(Black); break;
                default: break;
            }
        }
        // SET
        else if (message.header == Message::SET) {
            if (message.data.size() != sizeof(char) + sizeof(tank::Vectoru)) {
                std::cerr << "SET: unexpected number of bytes" << std::endl;
            } else {
                tank::Vectoru pos;
                std::memcpy(&pos, &message.data[1], sizeof(pos));
                try {
                    switch (message.data[0]) {
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
            }
        }
        // SCORE
        else if (message.header == Message::SCORE) {
            unsigned black, white;
            std::memcpy(&black, &message.data[0], sizeof(black));
            std::memcpy(&white, &message.data[0] + sizeof(black), sizeof(white));
            blackScore_->setScore(black);
            whiteScore_->setScore(white);
        } else {
            std::cerr << "Error: unexpected header: "
                            << message.header << std::endl;
        }

        mutex.unlock();
    } else {
        std::cerr << "Error: " << ec << std::endl;
    }

    connection->async_read_until(Message::delimiter,
                                 std::bind(&MainWorld::connectionHandler,
                                           this,
                                           connection,
                                           std::placeholders::_1,
                                           std::placeholders::_2));
}
