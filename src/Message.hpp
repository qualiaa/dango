#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <vector>
#include <algorithm>

class Message
{
public:
    static constexpr char SET     = 's';
    static constexpr char TURN    = 't';
    static constexpr char PLAYER  = 'p';
    static constexpr char BOARD   = 'b';
    static constexpr char SCORE   = 'c';

    Message() = default;
    Message(std::vector<char>&& data);
    char header;
    std::vector<char> data;
};

Message::Message(std::vector<char>&& data)
{
    header = data[0];
    // cut off header and delimiter
    std::move(data.begin() + 1, data.end() - 2, std::back_inserter(this->data));
}

#endif /* MESSAGE_HPP */
