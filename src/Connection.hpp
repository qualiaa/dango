#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <iostream>
#include <array>
#include <boost/asio.hpp>
#include <boost/array.hpp>

class Connection
{
    using Resolver = boost::asio::ip::tcp::resolver;
    using Socket = boost::asio::ip::tcp::socket;

    boost::asio::io_service io_;
    Socket socket_;

public:
    Connection()
        : socket_(io_)
    {
    }

    Connection(std::string hostname, std::string service)
        : Connection()
    {
        connect(hostname, service);
    }

    void connect(std::string hostname, std::string service)
    {
        std::cout << "Opening connection" << std::endl;
        Resolver resolver_(io_);
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

    void write(std::string str)
    {
        boost::system::error_code error;

        boost::asio::write(socket_,
                boost::asio::buffer(str, str.length()));

        if (error) {
            throw boost::system::system_error(error);
        }

        // Read response
        std::cout << read_some();
    }

    std::string read_some()
    {
        boost::system::error_code error;

        boost::array<char, 128> buf;
        size_t len = socket_.read_some(boost::asio::buffer(buf), error);

        if (error) {
            throw boost::system::system_error(error);
        }

        return std::string(buf.data(), len);
    }

    virtual ~Connection()
    {
        std::cout << "Closed connection" << std::endl;
    }
};

#endif /* CONNECTION_HPP */
