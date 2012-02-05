#ifndef ANIMSPRITE_H
#define ANIMSPRITE_H

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>

class AnimSprite : public sf::Sprite
{
    public:
        AnimSprite(); // doesn't work properly
        AnimSprite(const sf::Texture &texture, const sf::Vector2i &frameSize, const float &frameRate);

        void SetFrameSize(const sf::Vector2i &frameSize);
        const sf::Vector2i& GetFrameSize() const;

        void SetFrameRate(const float &frameRate);
        const float GetFrameRate() const;

        void SetFrame(const unsigned int &frame);
        const unsigned int& GetFrame() const;

        const unsigned int GetFrameCount() const;

        void ResetTime();

        void Update(const float &FT);
        void UpdateRect();
    private:
        sf::Vector2i myFrameSize;
        sf::Vector2i myFrameCount;
        float myFrameTime; // unsure about framerate 0.f
        unsigned int myFrame;

        float myTime;
};

#endif // ANIMSPRITE_H
