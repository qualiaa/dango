#ifndef MAINWORLD_HPP
#define MAINWORLD_HPP

#include <thread>
#include <boost/asio.hpp>
#include <Tank/System/World.hpp>
#include "Connection.hpp"
#include "Board.hpp"

class MainWorld : public tank::World
{
    std::shared_ptr<boost::asio::io_service> io_;
    Connection c;
    std::thread connectionThread_;

    tank::observing_ptr<Board> board_;
    bool currentTurn_ {false};
    Stone player_ {Empty};

public:
    MainWorld();
    virtual ~MainWorld();

    void threadFunc();
    void connectionHandler(Connection *c,
                           boost::system::error_code const&,
                           size_t bytes);
};

#endif /* MAINWORLD_HPP */
