#include "Aircraft.hpp"

//Aircraft::Aircraft(vector<sf::Image> &Images, sf::Vector2f Pos, float Rot) : Land(0), Turning(0.f), Turn(2.f), FlyIn(true)
Aircraft::Aircraft(vector<sf::Image> &Images, sf::Vector2f Pos, float Rot) : Turn(2.f), Land(0), Turning(0.f), FlyIn(true)
{
    const sf::Image &Image = Images[rand() % Images.size()];
    Shape.SetImage(Image);
    Shape.SetCenter(Image.GetWidth() / 2, Image.GetHeight() / 2);
    Shape.SetPosition(Pos);
    Shape.SetRotation(Rot);

    //Radius = sqrt(pow(Image.GetWidth(), 2) + pow(Image.GetHeight(), 2)) / 2.f / 1.5f;
    Radius = sqrt(pow(Image.GetWidth(), 2) + pow(Image.GetHeight(), 2)) / 2.f;
    Speed = 2.5f * Image.GetWidth();
}

sf::Vector2f Aircraft::GetPos()
{
    return Shape.GetPosition();
}

Path& Aircraft::GetPath()
{
    return P;
}

bool Aircraft::OnMe(sf::Vector2f Pos)
{
    const sf::Vector2f &Me = Shape.GetPosition();
    return pow(Pos.x - Me.x, 2) + pow(Pos.y - Me.y, 2) < pow(Radius, 2);
}

bool Aircraft::Colliding(const Aircraft &Other)
{
    const sf::Vector2f &Me = Shape.GetPosition();
    const sf::Vector2f &Pos = Other.Shape.GetPosition();
    return pow(Pos.x - Me.x, 2) + pow(Pos.y - Me.y, 2) < pow(Radius + Other.Radius, 2);
}

bool Aircraft::Colliding(const Explosion &Exp)
{
    const sf::Vector2f &Me = Shape.GetPosition();
    const sf::Vector2f &Pos = Exp.Shape.GetPosition();
    return pow(Pos.x - Me.x, 2) + pow(Pos.y - Me.y, 2) < pow((Radius + Exp.Radius) / 2.f, 2);
}

void Aircraft::SetRunway(Runway *NewLand)
{
    Land = NewLand;
}

bool Aircraft::Step(float FT)
{
    P.Highlight = static_cast<bool>(Land);

    const sf::Vector2f &Me = Shape.GetPosition();

    if (P.NumPoints() > 0)
    {
        FlyIn = false;
        const sf::Vector2f &To = P[0];

        if (pow(To.x - Me.x, 2) + pow(To.y - Me.y, 2) < pow(5, 2))
            P.RemovePoint(0);

        float Target = AngleFix(RadToDeg(atan2(To.y - Me.y, To.x - Me.x)));
        float Diff = AngleDiff(Shape.GetRotation(), Target);

        Shape.SetRotation(Target);
    }
    else if (FlyIn)
    {
        if (Me.x > 100 && Me.x < 700 && Me.y > 100 && Me.y < 500)
        {
            FlyIn = false;
            Turning = 0.2f;
        }
        sf::Vector2f To(400.f, 300.f);
        Shape.SetRotation(AngleFix(RadToDeg(atan2(To.y - Me.y, To.x - Me.x))));
    }
    else
    {
        FlyIn = false;
        float Angle = Shape.GetRotation(), AddAngle;
        if ((Me.x < 100 && Angle > 180) ||
            (Me.x > 700 && Angle < 180) ||
            (Me.y < 100 && (Angle > 90 && Angle < 270)) ||
            (Me.y > 500 && (Angle > 270 || Angle < 90)))
        {
            AddAngle = Turn;
            Turning = Turn / 10;
        }
        else if ((Me.x < 100 && Angle <= 180) ||
                 (Me.x > 700 && Angle >= 180) ||
                 (Me.y < 100 && (Angle <= 90 || Angle >= 270)) ||
                 (Me.y > 500 && (Angle <= 270 && Angle >= 90)))
        {
            AddAngle = -Turn;
            Turning = -Turn / 10;
        }
        else
        {
            AddAngle = Turning;
        }

        Angle += AddAngle;
        if (Angle < 0.f)
            Angle += 360.f;
        Shape.SetRotation(Angle);
    }

    Shape.Move(sf::Vector2f(cos(DegToRad(AngleFix(Shape.GetRotation()))), sin(DegToRad(AngleFix(Shape.GetRotation())))) * FT * Speed);

    return P.NumPoints() == 0 && (Land ? Land->OnMe(Me) : false);
}

void Aircraft::Draw(sf::RenderWindow& App)
{
    App.Draw(Shape);
    //App.Draw(sf::Shape::Circle(Shape.GetPosition(), 15.f, sf::Color::Magenta));
}
