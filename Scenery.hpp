#ifndef SCENERY_H
#define SCENERY_H

#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include "AnimSprite.hpp"
#include "Runway.hpp"

using namespace std;

struct SceneryTemplate
{
    string Name;
    string Res;
    sf::Vector2i FrameSize;
    float FrameRate;
    bool Airport;
    float Shadow;
    vector<string> Surfaces;
};

class Scenery
{
    public:
        Scenery(const SceneryTemplate &NewTemplate, map<string, sf::Texture> &Textures, sf::Vector2f Pos, float Rot);
        SceneryTemplate GetTemplate();

        bool Colliding(const Scenery &Other);
        bool Colliding(const Runway &Other);

        sf::Vector2f GetPos();
        float GetAngle();

        const AnimSprite& GetShape();

        void Step(float FT);
        void Draw(sf::RenderWindow &App);
        void DrawShadow(sf::RenderWindow &App);
    private:
        const SceneryTemplate &Template;
        AnimSprite Shape;
};

#endif // SCENERY_H
