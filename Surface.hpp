#ifndef SURFACE_H
#define SURFACE_H

#include <SFML/Graphics.hpp>
#include <string>

using namespace std;

struct SurfaceTemplate
{
    string Name;
    string Res;
};

class Surface
{
    public:
        Surface(SurfaceTemplate NewTemplate, map<string, sf::Texture> &Textures);
        SurfaceTemplate GetTemplate();

        void Draw(sf::RenderWindow &App);
    private:
        const SurfaceTemplate Template;
        sf::Sprite Shape;
};

#endif // SURFACE_H
