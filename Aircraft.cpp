#include "Aircraft.hpp"
#include "Math.hpp"
#include <iostream>

Aircraft::Aircraft(const AircraftTemplate &NewTemplate, map<string, sf::Texture> &Textures, map<string, sf::SoundBuffer> &Sounds, sf::Vector2f Pos, float Rot) : Template(NewTemplate), Land(0), State(FlyingIn), Direction(Aircraft::In)
{
    Setup(Textures, Sounds, Pos, Rot);
    FlySound.Play();
}

Aircraft::Aircraft(const AircraftTemplate &NewTemplate, map<string, sf::Texture> &Textures, map<string, sf::SoundBuffer> &Sounds, sf::Vector2f Pos, float Rot, Runway *NewRunway, OutDirections NewOutDirection) : Template(NewTemplate), Land(NewRunway), State(TakingOff), Direction(Aircraft::Out), OutDirection(NewOutDirection)
{
    Setup(Textures, Sounds, Pos, Rot);
    TakeoffSound.Play();
}

Aircraft::Aircraft(const AircraftTemplate &NewTemplate, map<string, sf::Texture> &Textures, map<string, sf::SoundBuffer> &Sounds, sf::Vector2f Pos, float Rot, Runway *NewRunway, States NewState, OutDirections NewOutDirection) : Template(NewTemplate), Land(NewRunway), State(NewState), Direction(Aircraft::Out), OutDirection(NewOutDirection)
{
    Setup(Textures, Sounds, Pos, Rot);
}

Aircraft::Aircraft(const AircraftTemplate &NewTemplate, map<string, sf::Texture> &Textures, map<string, sf::SoundBuffer> &Sounds, sf::Vector2f Pos, float Rot, Runway *NewRunway, States NewState, sf::Vector2f NewLandPoint) : Template(NewTemplate), Land(NewRunway), State(NewState), Direction(Aircraft::In), LandPoint(NewLandPoint)
{
    Setup(Textures, Sounds, Pos, Rot);
}

void Aircraft::Setup(map<string, sf::Texture> &Textures, map<string, sf::SoundBuffer> &Sounds, sf::Vector2f Pos, float Rot)
{
    const sf::Texture &Texture = Textures[Template.Res];
    if (Template.FrameSize.x >= 0 || Template.FrameSize.y >= 0)
    {
        Shape = AnimSprite(Texture, Template.FrameSize, Template.FrameRate);
        Shape.SetOrigin(Template.FrameSize.x / 2.f, Template.FrameSize.y / 2.f);
    }
    else
    {
        Shape = AnimSprite(Texture, sf::Vector2i(Texture.GetWidth(), Texture.GetHeight()), 0.f);
        Shape.SetOrigin(Texture.GetWidth() / 2, Texture.GetWidth() / 2);
    }
    Shape.SetPosition(Pos);
    Shape.SetRotation(Rot);

    TakeoffSound.SetBuffer(Sounds[Template.TakeoffRes]);
    TakeoffSound.SetAttenuation(0.01f);
    FlySound.SetBuffer(Sounds[Template.FlyRes]);
    FlySound.SetLoop(true);
    FlySound.SetAttenuation(0.01f);
    LandingSound.SetBuffer(Sounds[Template.LandingRes]);
    LandingSound.SetAttenuation(0.01f);

    Radius = Template.Radius;
    Speed = Template.Speed;
    Turn = Template.Turn;
}

const AircraftTemplate& Aircraft::GetTemplate() const
{
    return Template;
}

sf::Vector2f Aircraft::GetPos() const
{
    return Shape.GetPosition();
}

float Aircraft::GetAngle() const
{
    return Shape.GetRotation();
}

Runway* const Aircraft::GetLand() const
{
    return Land;
}

float Aircraft::GetRadius() const
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

Aircraft::States Aircraft::GetState()
{
    return State;
}

Aircraft::Directions Aircraft::GetDirection()
{
    return Direction;
}

Aircraft::OutDirections Aircraft::GetOutDirection()
{
    return OutDirection;
}

sf::Vector2f Aircraft::GetLandPoint()
{
    return LandPoint;
}

bool Aircraft::Colliding(const Aircraft &Other) const
{
    const sf::Vector2f &Me = Shape.GetPosition();
    const sf::Vector2f &Pos = Other.Shape.GetPosition();
    return OnRunway() == Other.OnRunway() &&
           InRange(Me, Pos, (Radius + Other.Radius) / 1.3f);
}

bool Aircraft::Colliding(const Explosion &Exp) const
{
    const sf::Vector2f &Me = Shape.GetPosition();
    const sf::Vector2f &Pos = Exp.Shape.GetPosition();
    return InRange(Me, Pos, (Radius + Exp.Radius) / 2.5f);
}

void Aircraft::SetRunway(Runway *NewLand)
{
    Land = NewLand;
}

void Aircraft::Pause(bool Status)
{
    if (Status)
    {
        TakeoffSound.Pause();
        FlySound.Pause();
        LandingSound.Pause();
    }
    else
    {
        if (TakeoffSound.GetStatus() == sf::Sound::Paused)
        {
            TakeoffSound.Play();
        }

        if (FlySound.GetStatus() == sf::Sound::Paused)
        {
            FlySound.Play();
        }

        if (LandingSound.GetStatus() == sf::Sound::Paused)
        {
            LandingSound.Play();
        }
    }
}

bool Aircraft::Step(float FT, sf::Vector2f Wind)
{
    bool Die = false;

    const sf::Vector2f &Me = Shape.GetPosition();
    TakeoffSound.SetPosition(Me.x, Me.y, 0.f);
    FlySound.SetPosition(Me.x, Me.y, 0.f);
    LandingSound.SetPosition(Me.x, Me.y, 0.f);

    Shape.Update(FT);

    float Turning = 0.f;

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
                     ((OutDirection == OutUp && Me.y < 50) ||
                      (OutDirection == OutDown && Me.y > 550) ||
                      (OutDirection == OutLeft && Me.x < 50) ||
                      (OutDirection == OutRight && Me.x > 750)))
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
                FlySound.Stop();
                LandingSound.Play();
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
            sf::Vector2f Runway = Land->GetPos() + PolarToRect(sf::Vector2f(Land->GetLength() * 1.5f, Land->GetAngle()));
            float Dist = Distance(Me, LandPoint);

            Speed = Map(Dist, 0.f, Land->GetLength() * 1.1f, Template.Speed, 0.f);

            if (Land->GetTemplate().Directional)
            {
                Shape.SetRotation(Angle(Me - Runway));
            }

            float Scale = Map(Dist, 0.f, Land->GetLength() * 1.1f, 1.f, 0.65f);
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
            sf::Vector2f Runway = Land->GetPos() + PolarToRect(sf::Vector2f(Land->GetLength() * 1.5f, Land->GetAngle()));
            float Dist = Distance(Me, Land->GetPos());

            Speed = Map(Dist, 0.f, Land->GetLength() * 1.1f, 10.f, Template.Speed);

            if (Land->GetTemplate().Directional)
            {
                Shape.SetRotation(Angle(Me - Runway));
            }

            float Scale = Map(Dist, 0.f, Land->GetLength() * 1.1f, 0.65f, 1.f);
            Shape.SetScale(Scale, Scale);
            Radius = Template.Radius * Scale;

            if (Dist > Land->GetLength())
            {
                FlySound.Play();
                State = FlyingFree;
                Land = 0;
            }
            break;
        }
    }

    Shape.Move(sf::Vector2f(cos(DegToRad(Shape.GetRotation())), sin(DegToRad(Shape.GetRotation()))) * FT * Speed);
    Shape.Move(Wind * FT);

    return Die;
}

void Aircraft::Draw(sf::RenderWindow& App)
{
    Shape.SetColor(sf::Color::White);
    App.Draw(Shape);
}

void Aircraft::DrawShadow(sf::RenderWindow &App)
{
    Shape.SetColor(sf::Color(0, 0, 0, 127));

    float Scale = Map(Speed / Template.Speed, 0.f, 1.f, 1.f, 0.9f);

    sf::Transform Transform;
    Transform.Scale(sf::Vector2f(Scale, Scale), sf::Vector2f(App.GetWidth() / 2, App.GetHeight() * 0.8f));
    App.Draw(Shape, Transform);
}
