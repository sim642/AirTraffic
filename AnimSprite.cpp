#include "AnimSprite.hpp"
#include <SFML/Graphics/Texture.hpp>

AnimSprite::AnimSprite() : sf::Sprite()
{
    ResetTime();
    SetFrame(0);
}

AnimSprite::AnimSprite(const sf::Texture &texture, const sf::Vector2i &frameSize, const float &frameRate) : sf::Sprite(texture)
{
    SetFrameSize(frameSize);
    SetFrameRate(frameRate);
    ResetTime();
    SetFrame(0);

    UpdateRect();
}


void AnimSprite::SetFrameSize(const sf::Vector2i &frameSize)
{
    myFrameSize = frameSize;
    myFrameCount.x = GetTexture()->GetWidth() / myFrameSize.x;
    myFrameCount.y = GetTexture()->GetHeight() / myFrameSize.y;
}

const sf::Vector2i& AnimSprite::GetFrameSize() const
{
    return myFrameSize;
}

void AnimSprite::SetFrameRate(const float &frameRate)
{
    myFrameTime = 1.f / frameRate;
}

const float AnimSprite::GetFrameRate() const
{
    return 1.f / myFrameTime;
}

void AnimSprite::SetFrame(const unsigned int &frame)
{
    myFrame = frame;
    const unsigned int &frameCount = GetFrameCount();
    if (myFrame >= frameCount)
        myFrame = frameCount - 1;
}

const unsigned int& AnimSprite::GetFrame() const
{
    return myFrame;
}

const unsigned int AnimSprite::GetFrameCount() const
{
    return myFrameCount.x * myFrameCount.y;
}

void AnimSprite::ResetTime()
{
    myTime = 0.f;
}

void AnimSprite::Update(const float &FT)
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
        myFrame %= GetFrameCount();
        UpdateRect();
    }
}

void AnimSprite::UpdateRect()
{
    sf::IntRect rect;
    rect.Left = myFrameSize.x * (myFrame % myFrameCount.x);
    rect.Top = myFrameSize.y * (myFrame / myFrameCount.x);
    rect.Width = myFrameSize.x;
    rect.Height = myFrameSize.y;

    SetTextureRect(rect);
}
