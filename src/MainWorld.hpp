#ifndef MAINWORLD_HPP
#define MAINWORLD_HPP

#include <thread>
#include <boost/asio.hpp>
#include <Tank/System/World.hpp>
#include "Connection.hpp"
#include "Indicator.hpp"
#include "Board.hpp"
#include "Button.hpp"
#include "Stone.hpp"
#include "Score.hpp"

class MainWorld : public tank::World
{
public:
    static const std::string messageDelim;
private:
    std::shared_ptr<boost::asio::io_service> io_;
    Connection connection_;
    std::string hostname_, port_;
    std::thread connectionThread_;

    tank::observing_ptr<Board> board_;
    tank::observing_ptr<Indicator> turnIndicator_;
    tank::observing_ptr<Indicator> playerIndicator_;
    tank::observing_ptr<Score> blackScore_;
    tank::observing_ptr<Score> whiteScore_;
    Stone player_ {Empty};

public:
    MainWorld(std::shared_ptr<boost::asio::io_service>, Connection&& c);
    virtual ~MainWorld();

    virtual void draw() override;

    void threadFunc();
    void connectionHandler(Connection *c,
                           boost::system::error_code const&,
                           size_t bytes);
};

#endif /* MAINWORLD_HPP */
