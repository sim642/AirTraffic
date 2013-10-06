#include "GraphUtil.hpp"
#include "Math.hpp"
#include <algorithm>

sf::ConvexShape Line(const sf::Vector2f &Start, const sf::Vector2f &End, float Thickness, const sf::Color &Color, float Outline, const sf::Color &OutlineColor)
{
    sf::Vector2f Normal(Start.y - End.y, End.x - Start.x);
    float Length = Distance(sf::Vector2f(0.f, 0.f), Normal);
    if (Length != 0.f)
        Normal /= Length;

    Normal *= Thickness / 2;


    sf::ConvexShape Line;

    Line.setPointCount(4);
    Line.setPoint(0, Start - Normal);
    Line.setPoint(1, End - Normal);
    Line.setPoint(2, End + Normal);
    Line.setPoint(3, Start + Normal);
    Line.setFillColor(Color);
    Line.setOutlineThickness(Outline);
    Line.setOutlineColor(OutlineColor);

    return Line;
}

sf::CircleShape Circle(const sf::Vector2f &Center, float Radius, const sf::Color &Color, float Outline, const sf::Color &OutlineColor)
{
    sf::CircleShape Circle;
    Circle.setOrigin(Radius, Radius);
    Circle.setPosition(Center);
    Circle.setRadius(Radius);
    Circle.setFillColor(Color);
    Circle.setOutlineThickness(Outline);
    Circle.setOutlineColor(OutlineColor);
    return Circle;
}

sf::RectangleShape Rectangle(float Left, float Top, float Width, float Height, const sf::Color &Color, float Outline, const sf::Color &OutlineColor)
{
    sf::RectangleShape Rectangle;
    Rectangle.setPosition(Left, Top);
    Rectangle.setSize(sf::Vector2f(Width, Height));
    Rectangle.setFillColor(Color);
    Rectangle.setOutlineThickness(Outline);
    Rectangle.setOutlineColor(OutlineColor);
    return Rectangle;
}

class BottomLeftCompare
{
public:
    bool operator() (const sf::Vector2f &Lhs, const sf::Vector2f &Rhs) const
    {
        if (Lhs.y == Rhs.y)
            return Lhs.x < Rhs.x;
        else
            return Lhs.y > Rhs.y;
    }
} BottomLeftComp;

class AngleCompare
{
public:
    AngleCompare(const sf::Vector2f &NewP) : P(NewP)
    {

    }

    bool operator() (const sf::Vector2f &Lhs, const sf::Vector2f &Rhs) const
    {
        return (-DotProduct(sf::Vector2f(1.f, 0.f), Lhs - P) / Magnitude(Lhs - P)) < (-DotProduct(sf::Vector2f(1.f, 0.f), Rhs - P) / Magnitude(Rhs - P));
    }
private:
    sf::Vector2f P;
};

float CCW(const sf::Vector2f &p1, const sf::Vector2f &p2, const sf::Vector2f &p3)
{
    return CrossProduct(p2 - p1, p3 - p1);
}

sf::ConvexShape ConvexHull(vector<sf::Vector2f> Points)
{
    iter_swap(Points.begin(), min_element(Points.begin(), Points.end(), BottomLeftComp));

    AngleCompare AngleComp(Points.front());
    sort(Points.begin() + 1, Points.end(), AngleComp);

    int m = 0;
    for (unsigned int i = 1; i < Points.size(); i++)
    {
        while (CCW(Points[m == 0 ? Points.size() - 1 : m - 1], Points[m], Points[i]) >= 0.f)
        {
            if (m > 0)
                m--;
            else if (i == Points.size() - 1)
                break;
            else
                i++;
        }

        m++;
        swap(Points[m], Points[i]);
    }

    sf::ConvexShape Shape(m + 1);
    for (unsigned int i = 0; i < Shape.getPointCount(); i++)
        Shape.setPoint(i, Points[i]);

    return Shape;
}

sf::Texture PerlinNoise()
{
    const int Size2 = 800;
    const float Persistence = 0.5f;

    vector< vector<float> > F(Size2, vector<float>(Size2, 0.f));

    for (int i = 4; i <= 7; i++)
    {
        int Freq = pow(2, i);
        float Amplitude = pow(Persistence, log(Size2) / log(2) - i);

        int Size = Size2 / Freq;
        vector< vector<sf::Vector2f> > G(Size + 1, vector<sf::Vector2f>(Size + 1));
        for (int y = 0; y < Size + 1; y++)
        {
            for (int x = 0; x < Size + 1; x++)
                G[y][x] = PolarToRect(sf::Vector2f(1.f, Random(0.f, 360.f)));
        }

        for (int y = 0; y < Size2; y++)
        {
            for (int x = 0; x < Size2; x++)
            {
                sf::Vector2f p(x, y);
                p *= float(Size) / Size2;
                sf::Vector2i p0(p);
                sf::Vector2i p1 = p0 + sf::Vector2i(1, 1);

                sf::Vector2f pp0 = p - sf::Vector2f(p0);
                sf::Vector2f pp1 = p - sf::Vector2f(p1);

                float s = DotProduct(G[p0.y][p0.x], pp0);
                float t = DotProduct(G[p0.y][p1.x], sf::Vector2f(pp1.x, pp0.y));
                float u = DotProduct(G[p1.y][p0.x], sf::Vector2f(pp0.x, pp1.y));
                float v = DotProduct(G[p1.y][p1.x], pp1);

                sf::Vector2f S;
                S.x = 3 * pow(pp0.x, 2) - 2 * pow(pp0.x, 3);
                float a = s + S.x * (t - s);
                float b = u + S.x * (v - u);
                S.y = 3 * pow(pp0.y, 2) - 2 * pow(pp0.y, 3);
                float z = a + S.y * (b - a);

                F[y][x] += z * Amplitude;
            }
        }
    }

    float min, max;
    for (int y = 0; y < Size2; y++)
    {
        for (int x = 0; x < Size2; x++)
        {
            if (y == 0 && x == 0)
                min = max = F[y][x];
            else
            {
                if (F[y][x] < min)
                    min = F[y][x];

                if (F[y][x] > max)
                    max = F[y][x];
            }
        }
    }

    sf::Image Img;
    Img.create(Size2, Size2, sf::Color::Red);
    for (int y = 0; y < Size2; y++)
    {
        for (int x = 0; x < Size2; x++)
        {
            int val = Map(F[y][x], min, max, 165, 255);
            Img.setPixel(x, y, sf::Color(val, val, val));
        }
    }

    sf::Texture Tex;
    Tex.loadFromImage(Img);
    return Tex;
}
