#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <vector>
#include <algorithm>

class Message
{
public:
    static constexpr char SET    = 's'; // Set a stone
    static constexpr char TURN   = 't'; // Set the turn
    static constexpr char PLAYER = 'p'; // Set client to player
    static constexpr char BOARD  = 'b'; // Set board size
    static constexpr char SCORE  = 'c'; // Set scores
    static constexpr char RESET  = 'r'; // Restart Game
    static const std::string delimiter;


    Message() = default;
    Message(std::vector<char>&& data);
    char header;
    std::vector<char> data;
};

#endif /* MESSAGE_HPP */
