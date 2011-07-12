#ifndef PATH_H
#define PATH_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

using namespace std;

class Path
{
    public:
        Path();

        int AddPoint(sf::Vector2f);
        bool TryAddPoint(sf::Vector2f);

        void RemovePoint(int);
        void Clear();
        int NumPoints();

        sf::Vector2f operator[](int);

        void Draw(sf::RenderWindow&);

        bool Highlight;
    protected:
    private:
        vector<sf::Vector2f> Points;
};

#endif // PATH_H
