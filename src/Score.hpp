#ifndef SCORE_HPP
#define SCORE_HPP

#include <Tank/System/Entity.hpp>
#include <Tank/Graphics/Text.hpp>

class Score : public tank::Entity
{
    tank::observing_ptr<tank::Text> scoreText_;
    unsigned score_;
public:
    Score(tank::Vectori pos, std::string label);

    void setScore(unsigned);

    virtual void draw(tank::Camera const&) override;
};

#endif /* SCORE_HPP */
