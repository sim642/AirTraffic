#ifndef PATH_H
#define PATH_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <algorithm>
#include "Math.hpp"
#include "GraphUtil.hpp"

using namespace std;

class Path
{
    public:
        Path();

        int AddPoint(sf::Vector2f Point);
        bool TryAddPoint(sf::Vector2f Point);

        float EndAngle();

        void RemovePoint(int i);
        void Clear();
        int NumPoints();

        sf::Vector2f operator[](int i);

        void Draw(sf::RenderWindow &App);

        bool Highlight;
    private:
        double tangent(double p0, double p1, double p2, double shit);
        double f(double p0, double p1, double m0, double m1, double t);

        vector<sf::Vector2f> Points;
        int interp;
};

#endif // PATH_H
