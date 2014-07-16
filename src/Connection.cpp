#include "Connection.hpp"
#include <iostream>

void Connection::connect(std::string hostname, std::string service)
{
    std::cout << "Opening connection" << std::endl;
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

    // Read welcome message
    std::cout << read_some();
}

void Connection::async_read_some(
        std::function<void(Connection *c,
                           boost::system::error_code const&,
                           size_t bytes)> handler)
{
    socket_.async_read_some(boost::asio::buffer(readBuf_, bufSize), 
                            std::bind(handler, 
                                      this, 
                                      std::placeholders::_1, 
                                      std::placeholders::_2));
}

void Connection::write(std::string str)
{
    boost::asio::write(socket_,
            boost::asio::buffer(str, str.length()));
}

std::string Connection::read_some()
{
    boost::system::error_code error;

    boost::array<char, 128> buf;
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
    std::cout << "Closed connection" << std::endl;
}