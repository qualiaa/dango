#ifndef MAINWORLD_HPP
#define MAINWORLD_HPP

#include <thread>
#include <boost/asio.hpp>
#include <Tank/System/World.hpp>
#include "Connection.hpp"
#include "Board.hpp"

class MainWorld : public tank::World
{
public:
    static const std::string messageDelim;
private:
    std::shared_ptr<boost::asio::io_service> io_;
    Connection c;
    std::string hostname_, port_;
    std::thread connectionThread_;

    tank::observing_ptr<Board> board_;
    bool currentTurn_ {false};
    Stone player_ {Empty};


public:
    MainWorld(std::string hostname, std::string port);
    virtual ~MainWorld();

    virtual void onAdded() override;

    void threadFunc();
    void connectionHandler(Connection *c,
                           boost::system::error_code const&,
                           size_t bytes);
};

#endif /* MAINWORLD_HPP */
