#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <array>
#include <boost/asio.hpp>
#include <boost/array.hpp>

class Connection
{
public:
    static constexpr unsigned bufSize = 128;

    using Resolver = boost::asio::ip::tcp::resolver;
    using Socket = boost::asio::ip::tcp::socket;

    std::shared_ptr<boost::asio::io_service> io_;
    Socket socket_;
    boost::array<char, bufSize> readBuf_;

public:
    Connection(std::shared_ptr<boost::asio::io_service> io)
        : io_(io)
        , socket_(*io)
    {
    }
    virtual ~Connection();

    void connect(std::string hostname, std::string service);
    void write(std::string str);
    std::string read_some();
    void async_read_some(std::function<void(Connection* c,
                                            boost::system::error_code const&,
                                            size_t bytes)>);

    boost::array<char, bufSize> const& readBuffer() { return readBuf_; }
};

#endif /* CONNECTION_HPP */
