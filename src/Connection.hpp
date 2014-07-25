#ifndef CONNECTION_HPP
#define CONNECTION_HPP

#include <array>
#include <boost/asio.hpp>
#include <boost/array.hpp>

class Connection
{
public:
    static constexpr unsigned bufSize = 256;

private:
    using Resolver = boost::asio::ip::tcp::resolver;
    using Socket = boost::asio::ip::tcp::socket;

    std::shared_ptr<boost::asio::io_service> io_;
    Socket socket_;
    boost::array<char, bufSize> readBuf_;
    boost::asio::streambuf streamBuf_;

public:
    Connection(std::shared_ptr<boost::asio::io_service> io)
        : io_(io)
        , socket_(*io)
    {
    }

    Connection(Connection&& c)
        : io_(c.io_)
        , socket_(std::move(c.socket_))
    {
        c.io_ = nullptr;
        c.socket_ = Socket(*io_);
    }

    Connection(Connection const&) = delete;
    Connection& operator=(Connection const&) = delete;
    virtual ~Connection();

    void connect(std::string hostname, std::string service);
    template<typename T>
    void write(T const& data, size_t bytes);
    void async_write(std::string str);
    std::string read_some();
    void async_read_until(std::string const& delim,
                          std::function<void(boost::system::error_code const&,
                                             size_t bytes)>);
    void async_read_some(std::function<void(boost::system::error_code const&,
                                            size_t bytes)>);

    boost::array<char, bufSize> const& readBuffer() { return readBuf_; }
    boost::asio::streambuf* streamBuffer() { return &streamBuf_; }
};

template <typename T>
void Connection::write(T const& data, size_t bytes)
{
    boost::asio::write(socket_,
            boost::asio::buffer(data, bytes));
}

#endif /* CONNECTION_HPP */
