#include "Score.hpp"

#include "StartWorld.hpp"

Score::Score(tank::Vectori pos, std::string label)
    : Entity(pos)
{
    using Pos = tank::Vectorf;

    auto labelText = makeGraphic<tank::Text>(StartWorld::font);
    labelText->setText(label);
    labelText->setFontSize(24);
    scoreText_ = makeGraphic<tank::Text>(StartWorld::font);
    scoreText_->setFontSize(24);
    scoreText_->setPos(labelText->getPos() + Pos{labelText->getSize().x + 10,0});

    setScore(0);
}

void Score::setScore(unsigned score)
{
    score_ = score;
}

void Score::draw(tank::Camera const& cam)
{
    scoreText_->setText(std::to_string(score_));
    Entity::draw(cam);
}
