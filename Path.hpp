#ifndef PATH_H
#define PATH_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "Math.hpp"

using namespace std;

class Path
{
    public:
        Path();

        int AddPoint(sf::Vector2f);
        bool TryAddPoint(sf::Vector2f);

        float EndAngle();

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
