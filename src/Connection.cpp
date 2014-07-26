#include "Connection.hpp"
#include <Tank/System/Game.hpp>

void Connection::connect(std::string hostname, std::string service)
{
    tank::Game::log << "Opening connection" << std::endl;
    Resolver resolver_(*io_);
    Resolver::iterator end, endpoint = resolver_.resolve({hostname, service});
    boost::system::error_code error = boost::asio::error::host_not_found;

    while (error && endpoint != end) {
        socket_.close();
        socket_.connect(*endpoint++, error);
    }

    if (error) {
        throw boost::system::system_error(error);
    }
}

void Connection::async_read_until(
        std::string const& delim,
        std::function<void(boost::system::error_code const&,
                           size_t bytes)> handler)
{
    boost::asio::async_read_until(socket_,
                                  streamBuf_,
                                  delim,
                                  handler);
}

void Connection::async_read_some(
        std::function<void(boost::system::error_code const&,
                           size_t bytes)> handler)
{
    socket_.async_read_some(boost::asio::buffer(readBuf_, bufSize), handler);
}

void Connection::async_write(std::string str)
{
}

std::string Connection::read_some()
{
    boost::system::error_code error;

    boost::array<char, bufSize> buf;
    size_t len = socket_.read_some(boost::asio::buffer(buf), error);

    if (error) {
        throw boost::system::system_error(error);
    }

    return std::string(buf.data(), len);
}

Connection::~Connection()
{
    boost::system::error_code ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,ec);
    socket_.close(ec);
    if (ec) {
        tank::Game::log << ec << std::endl;
    }
}
