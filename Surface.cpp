#include "Surface.hpp"

Surface::Surface(const SurfaceTemplate &NewTemplate, map<string, sf::Texture> &Textures) : Template(NewTemplate)
{
    Shape.SetTexture(Textures[Template.Res]);
}

SurfaceTemplate Surface::GetTemplate()
{
    return Template;
}

void Surface::Draw(sf::RenderWindow &App)
{
    for (unsigned int y = 0; y < App.GetHeight(); y += Shape.GetTexture()->GetHeight())
    {
        for (unsigned int x = 0; x < App.GetWidth(); x += Shape.GetTexture()->GetWidth())
        {
            Shape.SetPosition(x, y);
            App.Draw(Shape);
        }
    }
}
