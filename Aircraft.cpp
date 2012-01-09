#include "Aircraft.hpp"

Aircraft::Aircraft(AircraftTemplate NewTemplate, map<string, sf::Texture> &Textures, sf::Vector2f Pos, float Rot, Runway *NewRunway) : Template(NewTemplate), Land(NewRunway), Turning(0.f)
{
    const sf::Texture &Texture = Textures[Template.Res];
    Shape.SetTexture(Texture);
    Shape.SetOrigin(Texture.GetWidth() / 2, Texture.GetHeight() / 2);
    Shape.SetPosition(Pos);
    Shape.SetRotation(Rot);

    Radius = Template.Radius;
    Speed = Template.Speed;
    Turn = Template.Turn;

    if (NewRunway != 0)
    {
        State = TakingOff;
        Direction = Out;
    }
    else
    {
        State = FlyingIn;
        Direction = In;
    }
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
    return !OnRunway() && State != FlyingOut;
}

bool Aircraft::OnRunway() const
{
    return State == Landing || State == TakingOff;
}

Aircraft::Directions Aircraft::GetDirection()
{
    return Direction;
}

bool Aircraft::Colliding(const Aircraft &Other)
{
    const sf::Vector2f &Me = Shape.GetPosition();
    const sf::Vector2f &Pos = Other.Shape.GetPosition();
    return OnRunway() == Other.OnRunway() &&
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

    const sf::Vector2f &Me = Shape.GetPosition();

    switch (State)
    {
        case FlyingIn:
        {
            sf::Vector2f To(400.f, 300.f);
            Shape.SetRotation(RadToDeg(atan2(To.y - Me.y, To.x - Me.x)));

            if (P.NumPoints() > 0)
            {
                State = FlyingPath;
            }
            else if (Me.x > 100 && Me.x < 700 && Me.y > 100 && Me.y < 500)
            {
                State = FlyingFree;
                Turning = 0.2f;
            }
            break;
        }
        case FlyingOut:
        {
            sf::Vector2f From(400.f, 300.f);
            Shape.SetRotation(RadToDeg(atan2(Me.y - From.y, Me.x - From.x)));

            if (Me.x < 0 || Me.x > 800 || Me.y < 0 || Me.y > 600)
            {
                Die = true;
            }
            break;
        }
        case FlyingFree:
        {
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

            if (P.NumPoints() > 0)
            {
                State = FlyingPath;
            }
            else if (Direction == Out &&
                     (Me.x < 50 || Me.x > 750 || Me.y < 50 || Me.y > 550))
            {
                State = FlyingOut;
            }
            break;
        }
        case FlyingPath:
        {
            const sf::Vector2f To = P[0]; // might crash

            if (InRange(Me, To, 5))
                P.RemovePoint(0);

            float Target = RadToDeg(atan2(To.y - Me.y, To.x - Me.x));

            Shape.SetRotation(Target);

            if (Land &&
                P.NumPoints() == 0 &&
                Land->OnMe(Me) &&
                abs(AngleDiff(GetAngle(), Land->GetAngle())) <= Land->GetTemplate().LandAngle)
            {
                State = Landing;
                LandPoint = Me;
            }
            else if (P.NumPoints() == 0)
            {
                State = FlyingFree;
            }
            break;
        }
        case Landing:
        {
            sf::Vector2f Runway = LandPoint;
            float Dist = Distance(Me, Runway);

            Speed = wr::Map(Dist, 0.f, Land->GetLength() * 1.1f, Template.Speed, 0.f);

            if (Land->GetTemplate().Directional)
            {
                Shape.SetRotation(Land->GetAngle());
            }

            float Scale = wr::Map(Dist, 0.f, Land->GetLength() * 1.1f, 1.f, 0.65f);
            Shape.SetScale(Scale, Scale);
            Radius = Template.Radius * Scale;

            if (Dist > Land->GetLength())
            {
                Die = true;
            }
            break;
        }
        case TakingOff:
        {
            sf::Vector2f Runway = Land->GetPos();
            float Dist = Distance(Me, Runway);

            Speed = wr::Map(Dist, 0.f, Land->GetLength() * 1.1f, 10.f, Template.Speed);

            if (Land->GetTemplate().Directional)
            {
                Shape.SetRotation(Land->GetAngle());
            }

            float Scale = wr::Map(Dist, 0.f, Land->GetLength() * 1.1f, 0.65f, 1.f);
            Shape.SetScale(Scale, Scale);
            Radius = Template.Radius * Scale;

            if (Dist > Land->GetLength())
            {
                State = FlyingFree;
                Land = 0;
            }
            break;
        }
    }

    Shape.Move(sf::Vector2f(cos(DegToRad(Shape.GetRotation())), sin(DegToRad(Shape.GetRotation()))) * FT * Speed);

    return Die;
}

void Aircraft::Draw(sf::RenderWindow& App)
{
    App.Draw(Shape);
}
