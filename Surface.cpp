#include "Surface.hpp"

Surface::Surface(const SurfaceTemplate &NewTemplate, map<string, sf::Texture> &Textures) : Template(NewTemplate)
{
    Shape.setTexture(Textures[Template.Res]);
}

SurfaceTemplate Surface::GetTemplate()
{
    return Template;
}

void Surface::Draw(sf::RenderWindow &App)
{
    for (unsigned int y = 0; y < App.getSize().y; y += Shape.getTexture()->getSize().y)
    {
        for (unsigned int x = 0; x < App.getSize().x; x += Shape.getTexture()->getSize().x)
        {
            Shape.setPosition(x, y);
            App.draw(Shape);
        }
    }
}
