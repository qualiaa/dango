#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <vector>
#include <algorithm>
#include <Tank/Utility/Vector.hpp>

class Message
{
public:
    static constexpr char BOARD  = 'b'; // Set board size
    static constexpr char SCORE  = 'c'; // Set scores
    static constexpr char END    = 'e'; // Game Ended
    static constexpr char KILL   = 'k'; // Kick all marked stones
    static constexpr char MARK   = 'm'; // Mark a stone
    static constexpr char PLAYER = 'p'; // Set client to player
    static constexpr char RESET  = 'r'; // Restart Game
    static constexpr char SET    = 's'; // Set a stone
    static constexpr char TURN   = 't'; // Set the turn / pass
    static constexpr char UNDO   = 'u'; // Undo
    static constexpr char KICK_ALL = '\t'; // Kick all
    static const std::string delimiter;

    struct Set
    {
        tank::Vector<uint32_t> pos;
        char color;
    };

    struct Mark
    {
        tank::Vector<uint32_t> pos;
        char mark;
    };

    struct Turn
    {
        char color;
    };

    struct Player
    {
        char color;
    };

    struct Board
    {
        uint32_t size;
    };

    struct Score
    {
        uint32_t black;
        uint32_t white;
    };

    struct Empty
    {
        char nil;
    };


    Message() = default;
    Message(std::vector<char>&& data);
    char header;
    std::vector<char> data;
    union {
        Set set;
        Mark mark;
        Turn turn;
        Player player;
        Board board;
        Score score;
        Empty empty;
    };
};

#endif /* MESSAGE_HPP */
