#ifndef RUNWAY_H
#define RUNWAY_H

#include <SFML/Graphics.hpp>
#include <map>
#include <vector>

using namespace std;

struct RunwayTemplate
{
    string Name;
    string Res;
    sf::Vector2f Center;
    float Radius;
    float LandAngle;
    float Length;
    bool Directional;
    vector<string> Surfaces;
};

class Runway
{
    public:
        friend class Scenery;
        Runway(const RunwayTemplate &NewTemplate, map<string, sf::Texture> &Textures, sf::Vector2f Pos, float Rot);
        RunwayTemplate GetTemplate();

        bool OnMe(sf::Vector2f Pos);

        bool Colliding(const Runway &Other);

        sf::Vector2f GetPos();
        float GetAngle();
        float GetLength();

        sf::Sprite GetShape();

        void Draw(sf::RenderWindow &App);
    protected:
    private:
        const RunwayTemplate &Template;
        sf::Sprite Shape;

        float Radius;
};

#endif // RUNWAY_H
