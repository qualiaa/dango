#include "MainWorld.hpp"

#include <functional>
#include <istream>
#include <vector>
#include <boost/system/system_error.hpp>
#include <Tank/System/Game.hpp>
#include <Tank/System/Keyboard.hpp>
#include "Message.hpp"

MainWorld::MainWorld(std::shared_ptr<boost::asio::io_service> io, Connection&& c)
    : io_(io)
    , connection_(std::move(c))
{
    using Pos = tank::Vectorf;
    turnIndicator_ = makeEntity<Indicator>(Pos{500, 60}, "Current Turn");
    playerIndicator_ = makeEntity<Indicator>(Pos{500, 200}, "You Are");

    blackScoreDisplay = makeEntity<Score>(Pos{425, 340}, "Black:");
    whiteScoreDisplay = makeEntity<Score>(Pos{425, 375}, "White:");

    // Pass button
    button_ = makeEntity<Button>(Pos{425, 410},
                                 tank::Vectoru{100,30},
                                 "Pass",
                                 [this]{
                                     // Set up message
                                     boost::array<char, 1> data;
                                     data[0] = Message::TURN;
                                     // Send message
                                     connection_.write(data, data.size());
                                 });

    // Start io event listener
    connection_.async_read_until(Message::delimiter,
                                 std::bind(&MainWorld::connectionHandler,
                                           this,
                                           &connection_,
                                           std::placeholders::_1,
                                           std::placeholders::_2));

    // Split IO thread
    connectionThread_ = std::thread(&MainWorld::threadFunc, this);
    connectionThread_.detach();

    // Register input handlers
    using K = tank::Keyboard;
    using Key = tank::Key;
    connect(K::KeyRelease(Key::Escape), tank::Game::popWorld);
    connect(K::KeyDown(Key::LShift) && K::KeyDown(Key::K),
                                 [this]{
                                     // Set up message
                                     boost::array<char, 1> data;
                                     data[0] = Message::KICK_ALL;
                                     // Send message
                                     connection_.write(data, data.size());
                                 });
    connect(K::KeyDown(Key::LControl) && K::KeyPress(Key::Z),
                                 [this]{
                                     // Set up message
                                     boost::array<char, 1> data;
                                     data[0] = Message::UNDO;
                                     // Send message
                                     connection_.write(data, data.size());
                                 });
    connect(K::KeyDown(Key::K),
            [this]{
                // Set up message
                boost::array<char, 1> data;
                data[0] = Message::KILL;
                // Send message
                connection_.write(data, data.size());
            });
}

void MainWorld::draw()
{
    mutex_.lock();
    tank::World::draw();
    mutex_.unlock();
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
                mutex_.lock();
                std::cerr << "[" << std::this_thread::get_id() << "] ";
                std::cerr << "Error: " << ec << std::endl;
                mutex_.unlock();
            }
            break;
        }
        catch (std::exception const& e) {
            std::cerr << "[" << std::this_thread::get_id() << "] ";
            std::cerr << "Exception: " << e.what() << std::endl;
        }
    }

    mutex_.lock();
    std::cout << "[" << std::this_thread::get_id() << "] Thread closing" << std::endl;
    mutex_.unlock();
}

void MainWorld::connectionHandler(Connection *connection,
                                  boost::system::error_code const& ec,
                                  size_t bytes)
{
    if (!ec) {
        mutex_.lock();

        // Create an instream from the connection stream buffer
        std::istream instream {connection->streamBuffer()};
        // Create a vector to store data of size `bytes`
        std::vector<char> data(bytes);
        // Read the data into the vector
        instream.read(&data[0], bytes);
        // print data
        /*
        std::cout << "Handling " << bytes << " bytes: < " << std::hex;
        for (auto byte : data) {
            std::cout << static_cast<unsigned>(byte) << " ";
        }
        std::cout << ">" << std::dec <<  std::endl;
        */

        Message message{std::move(data)};

        // TODO: Error checking
        if (message.header == Message::PLAYER) {
            switch (message.player.color) {
                case 'w': player_ = White; break;
                case 'b': player_ = Black; break;
                default: player_ = Empty; break;
            }
            playerIndicator_->setColor(player_);
            board_->setCursor(player_);
        } else if (message.header == Message::BOARD) {
            if (board_) {
                board_->remove();
            }
            board_ = makeEntity<Board>(*connection, message.board.size);
            board_->setCursor(player_);
        } else if (message.header == Message::TURN) {
            switch (message.turn.color) {
                case 'w': turnIndicator_->setColor(White); break;
                case 'b': turnIndicator_->setColor(Black); break;
                default: break;
            }
        } else if (message.header == Message::SET) {
            try {
                switch (message.set.color) {
                    case 'w': board_->setStone(message.set.pos, White); break;
                    case 'b': board_->setStone(message.set.pos, Black); break;
                    case 'e': board_->setStone(message.set.pos, Empty); break;
                    default:
                        std::cerr << "SET: unexpected data" << std::endl;
                        break;
                }
            }
            catch (std::exception const& e) {
                std::cerr << "Exception: " << e.what() << std::endl;
            }
        } else if (message.header == Message::MARK) {
            try {
                board_->setMark(message.mark.pos, message.mark.mark);
            }
            catch (std::exception const& e) {
                std::cerr << "Exception: " << e.what() << std::endl;
            }
        } else if (message.header == Message::SCORE) {
            blackScoreDisplay->setScore(message.score.black);
            whiteScoreDisplay->setScore(message.score.white);
        } else if (message.header == Message::END) {
            turnIndicator_->setColor(Empty);
            //button_->setLabel("New Game");
            button_->setCallback([this]{
                                     // Set up message
                                     boost::array<char, 1> data;
                                     data[0] = Message::RESET;
                                     // Send message
                                     connection_.write(data, data.size());
                                 });
        } else if (message.header == Message::RESET) {
            //button_->setLabel("Pass");
            button_->setCallback([this]{
                                     // Set up message
                                     boost::array<char, 1> data;
                                     data[0] = Message::TURN;
                                     // Send message
                                     connection_.write(data, data.size());
                                 });
        } else {
            std::cerr << "Error: unexpected header: "
                            << message.header << std::endl;
        }

        mutex_.unlock();
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
