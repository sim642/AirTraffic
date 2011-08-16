#include "Aircraft.hpp"

Aircraft::Aircraft(AircraftTemplate NewTemplate, map<string, sf::Image> &Images, sf::Vector2f Pos, float Rot) : Template(NewTemplate), Land(0), Turning(0.f), FlyIn(true), FlyRunway(false)
{
    const sf::Image &Image = Images[Template.Res];
    Shape.SetImage(Image);
    Shape.SetOrigin(Image.GetWidth() / 2, Image.GetHeight() / 2);
    Shape.SetPosition(Pos);
    Shape.SetRotation(Rot);

    Radius = Template.Radius;
    Speed = Template.Speed;
    Turn = Template.Turn;
}

AircraftTemplate Aircraft::GetTemplate()
{
    return Template;
}

sf::Vector2f Aircraft::GetPos()
{
    return Shape.GetPosition();
}

float Aircraft::GetAngle()
{
    return Shape.GetRotation();
}

Runway* Aircraft::GetLand()
{
    return Land;
}

float Aircraft::GetRadius()
{
    return Radius;
}

Path& Aircraft::GetPath()
{
    return P;
}

bool Aircraft::OnMe(sf::Vector2f Pos)
{
    const sf::Vector2f &Me = Shape.GetPosition();
    return InRange(Me, Pos, Radius);
}

bool Aircraft::Pathable()
{
    return !FlyRunway;
}

bool Aircraft::OnRunway()
{
    return FlyRunway;
}

bool Aircraft::Colliding(const Aircraft &Other)
{
    const sf::Vector2f &Me = Shape.GetPosition();
    const sf::Vector2f &Pos = Other.Shape.GetPosition();
    return FlyRunway == Other.FlyRunway &&
           InRange(Me, Pos, (Radius + Other.Radius) / 1.3f);
}

bool Aircraft::Colliding(const Explosion &Exp)
{
    const sf::Vector2f &Me = Shape.GetPosition();
    const sf::Vector2f &Pos = Exp.Shape.GetPosition();
    return InRange(Me, Pos, (Radius + Exp.Radius) / 2.5f);
}

void Aircraft::SetRunway(Runway *NewLand)
{
    Land = NewLand;
}

bool Aircraft::Step(float FT)
{
    bool Die = false;
    P.Highlight = static_cast<bool>(Land);

    const sf::Vector2f &Me = Shape.GetPosition();

    if (P.NumPoints() > 0)
    {
        FlyIn = false;
        const sf::Vector2f &To = P[0];

        if (InRange(Me, To, 5))
            P.RemovePoint(0);

        float Target = RadToDeg(atan2(To.y - Me.y, To.x - Me.x));

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
        Shape.SetRotation(RadToDeg(atan2(To.y - Me.y, To.x - Me.x)));
    }
    else if (FlyRunway)
    {
        sf::Vector2f Runway = Land->GetPos();
        float Dist = Distance(Me, Runway);

        Speed = wr::Map(Dist, 0.f, Land->GetLength() * 1.1f, Template.Speed, 0.f);
        Shape.SetRotation(Land->GetAngle());

        float Scale = wr::Map(Dist, 0.f, Land->GetLength() * 1.1f, 1.f, 0.65f);
        Shape.SetScale(Scale, Scale);
        Radius = Template.Radius * Scale;

        if (Dist > Land->GetLength())
        {
            Die = true;
        }
    }
    else
    {
        FlyIn = false;
        float Angle = AngleFix(Shape.GetRotation()), AddAngle;
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
        Shape.SetRotation(AngleFix(Angle));
    }

    Shape.Move(sf::Vector2f(cos(DegToRad(Shape.GetRotation())), sin(DegToRad(Shape.GetRotation()))) * FT * Speed);

    if (!FlyRunway &&
        Land &&
        P.NumPoints() == 0 &&
        Land->OnMe(Me) &&
        abs(AngleDiff(GetAngle(), Land->GetAngle())) <= Land->GetTemplate().LandAngle)
    {
        FlyRunway = true;
    }

    return Die;
}

void Aircraft::Draw(sf::RenderWindow& App)
{
    App.Draw(Shape);
}
