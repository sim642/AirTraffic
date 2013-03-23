#include "AnimSprite.hpp"
#include <SFML/Graphics/Texture.hpp>

AnimSprite::AnimSprite() : sf::Sprite()
{
    resetTime();
    setFrame(0);
}

AnimSprite::AnimSprite(const sf::Texture &texture, const sf::Vector2i &frameSize, const float &frameRate) : sf::Sprite(texture)
{
    setFrameSize(frameSize);
    setFrameRate(frameRate);
    resetTime();
    setFrame(0);

    updateRect();
}


void AnimSprite::setFrameSize(const sf::Vector2i &frameSize)
{
    myFrameSize = frameSize;
    myFrameCount.x = getTexture()->getSize().x / myFrameSize.x;
    myFrameCount.y = getTexture()->getSize().y / myFrameSize.y;
}

const sf::Vector2i& AnimSprite::getFrameSize() const
{
    return myFrameSize;
}

void AnimSprite::setFrameRate(const float &frameRate)
{
    myFrameTime = 1.f / frameRate;
}

const float AnimSprite::getFrameRate() const
{
    return 1.f / myFrameTime;
}

void AnimSprite::setFrame(const unsigned int &frame)
{
    myFrame = frame;
    const unsigned int &frameCount = getFrameCount();
    if (myFrame >= frameCount)
        myFrame = frameCount - 1;
}

const unsigned int& AnimSprite::getFrame() const
{
    return myFrame;
}

const unsigned int AnimSprite::getFrameCount() const
{
    return myFrameCount.x * myFrameCount.y;
}

void AnimSprite::resetTime()
{
    myTime = 0.f;
}

void AnimSprite::update(const float &FT)
{
    myTime += FT;

    unsigned int framesPassed = 0;
    while (myTime >= myFrameTime)
    {
        myTime -= myFrameTime;
        framesPassed++;
    }

    if (framesPassed > 0)
    {
        myFrame += framesPassed;
        myFrame %= getFrameCount();
        updateRect();
    }
}

void AnimSprite::updateRect()
{
    sf::IntRect rect;
    rect.left = myFrameSize.x * (myFrame % myFrameCount.x);
    rect.top = myFrameSize.y * (myFrame / myFrameCount.x);
    rect.width = myFrameSize.x;
    rect.height = myFrameSize.y;

    setTextureRect(rect);
}
