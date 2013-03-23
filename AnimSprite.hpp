#ifndef ANIMSPRITE_H
#define ANIMSPRITE_H

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>

class AnimSprite : public sf::Sprite
{
    public:
        AnimSprite(); // doesn't work properly
        AnimSprite(const sf::Texture &texture, const sf::Vector2i &frameSize, const float &frameRate);

        void setFrameSize(const sf::Vector2i &frameSize);
        const sf::Vector2i& getFrameSize() const;

        void setFrameRate(const float &frameRate);
        const float getFrameRate() const;

        void setFrame(const unsigned int &frame);
        const unsigned int& getFrame() const;

        const unsigned int getFrameCount() const;

        void resetTime();

        void update(const float &FT);
        void updateRect();
    private:
        sf::Vector2i myFrameSize;
        sf::Vector2i myFrameCount;
        float myFrameTime; // unsure about framerate 0.f
        unsigned int myFrame;

        float myTime;
};

#endif // ANIMSPRITE_H
