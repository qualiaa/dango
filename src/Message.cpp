#include "Message.hpp"

const std::string Message::delimiter {"\r\n"};

Message::Message(std::vector<char>&& data)
{
    header = data[0];
    // cut off header and delimiter
    std::move(data.begin() + 1, data.end() - 2, std::back_inserter(this->data));
}
