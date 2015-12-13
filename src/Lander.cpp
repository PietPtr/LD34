#include "include.h"
#include "Lander.h"
#include <iostream>
#include <SFML/Graphics.hpp>

using namespace sf;

int randint(int low, int high, int seed);

Lander::Lander()
{
    std::cout << "Lander object created...\n";
}

void Lander::init(LanderSettings settings)
{
    this->settings = settings;
    position = settings.position;
    velocity = settings.velocity;
    speed = settings.speed;
    acceleration = Vector2<double>(0, 0);
    sfx = settings.sfx;
    std::cout << "Lander initialized...\n";
}

int Lander::update(double dt, int phase, SurfaceGenerator* surfaceGenerator)
{
    this->dt = dt;
    this->totalTime += dt;

    if (phase == 0)
        return phaseMenu();
    else if (phase == 1)
        return phaseOrbit();
    else if (phase == 2)
        return phaseDeorbit(surfaceGenerator);
    return -1;
}

int Lander::phaseMenu()
{
    phaseOrbit();
    return 0;
}

int Lander::phaseOrbit()
{
    acceleration = Vector2<double>(0, 0);
    velocity = Vector2<double>(1, 0);

    position.x = position.x + velocity.x * speed * dt;
    position.y = position.y + velocity.y * speed * dt;

    /*if ((*sfx).at(2)->sound.getStatus() == 0)
    {
        std::cout << "playing sound 2" << "\n";
        (*sfx).at(2)->play();
    }*/
    //std::cout << (*sfx).at(2)->sound.getStatus() << "\n";

    return 1;
}

int Lander::phaseDeorbit(SurfaceGenerator* surfaceGenerator)
{
    if (checkCollision(surfaceGenerator) == 3)
        return 3;

    // Forces

    gravitationForce = calcGravitationForce();
    gravitation = normalize(Vector2<double>(0, 1)) * gravitationForce;

    thrust.x = cos(rotation * (PI/180) - 0.5 * PI);
    thrust.y = sin(rotation * (PI/180) - 0.5 * PI);

    Vector2<double> Fsum = gravitation + thrust * rocketPower;

    // Acceleration

    acceleration = Fsum / mass;

    double accLength = sqrt(pow(acceleration.x, 2) + pow(acceleration.y, 2));
    Vector2<double> normalizedAcceleration = Vector2<double>(acceleration.x / accLength, acceleration.y / accLength);

    speed += accLength * dt;

    Vector2<double> velocityAndAcceleration = velocity * speed + acceleration;
    velocity = normalize(velocityAndAcceleration);

    // Position

    position.x = position.x + velocity.x * speed * dt;
    position.y = position.y + velocity.y * speed * dt;

    // Rotation

    if (Keyboard::isKeyPressed(Keyboard::Left)  || Keyboard::isKeyPressed(Keyboard::A))
        angularMomentum -= 4;
    if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
        angularMomentum += 4;

    rotation += angularMomentum * dt;

    // Remaining updates

    rocketPower = maxThrust * throttle;

    altitude = -position.y - surfaceGenerator->getHeight(position.x);

    double previousThrottle = throttle;
    if (altitude < 4000)
        throttle = 2.0;
    if (altitude < 2000)
        throttle = 3.0;
    if (altitude < 1000)
        throttle = 5.0;
    if (altitude < 500)
        throttle = 10.0;
    if (altitude < 100 && altitude > 0)
        throttle = altitude / 100;
    if (altitude < 0)
        throttle = 0;
    if (altitude >= 4000)
        throttle = 1.0;

    //std::cout << previousThrottle << " " << throttle << "\n";

    if ((totalTime - lastSoundPlayed > randint(2, 5, lastSoundPlayed)) && totalTime - lastSoundPlayed > 2 && !isSoundPlaying())
        playAtmosphericSound();

    //std::cout << totalTime - lastSoundPlayed << "\n";

    return 2;
}

void Lander::playAtmosphericSound()
{
    if (altitude > 8000)
    {
        (*sfx).at(3 + randint(0, 5, altitude))->play();
        std::cout << "playing sound " << 3 + randint(0, 5, altitude) << "\n";
        //(*sfx).at(6)->play();
    }
    else if (altitude < 8000)
    {
        (*sfx).at(9 + randint(0, 2, altitude))->play();
    }
    else if (altitude < 5000)
    {
        (*sfx).at(12 + randint(0, 6, altitude))->play();
    }

    lastSoundPlayed = totalTime;
}

bool Lander::isSoundPlaying()
{
    for (int i = 1; i < (*sfx).size(); i++)
    {
        if ((*sfx).at(i)->sound.getStatus() == 1)
            return true;
    }
    return false;
    //std::cout << std::endl;
}

int Lander::checkCollision(SurfaceGenerator* surfaceGenerator)
{
    // calculate hitpoint position
    double cs = cos(rotation * (PI/180));
    double sn = sin(rotation * (PI/180));
    Vector2i hitpoints[2];
    hitpoints[0].x = -120 * cs - 126 * sn;
    hitpoints[0].y = -120 * sn + 126 * cs;
    hitpoints[1].x =  120 * cs - 126 * sn;
    hitpoints[1].y =  120 * sn + 126 * cs;

    std::cout << "Altitude: " << altitude << "\n";

    // for
    for (int i = 0; i < 2; i++)
    {
        // find surface points under hitpoints
        int index = (int)((position.x + hitpoints[i].x) / 16.0) % 4097;
        index = index > 4097 ? 0 : index;
        double baseHeight = surfaceGenerator->getBaseHeight();
        double surfaceHeight = surfaceGenerator->getSurfaceHeight();
        double height1 = baseHeight + surfaceGenerator->surface[index] * surfaceHeight;         //maybe something is wrong here
        double height2 = baseHeight + surfaceGenerator->surface[index + 1] * surfaceHeight;

        // create line function between surface points
        double x1 = index * 16;
        double y1 = height1;
        double y2 = height2;
        double a = (abs(height1 - height2)) / 16.0;
        double b = y1 - a * x1;
        //std::cout << "y = " << a << "x + " << b << "    ";

        // check hitpoints.y < line(hitpoints.x)
        if (-position.y + hitpoints[i].y < a * (position.x + hitpoints[i].x) + b)
        {
            //std::cout << "Lunar contact!" << "\n";
            std::cout << i << ": " << -position.y + hitpoints[i].y << " < " << a * (position.x + hitpoints[i].x) + b
                      << ", pos.y: " << -position.y << ", h1: " << height1 << ", h2: " << height2 << ", get: " << surfaceGenerator->getHeight(position.x) << "\n";
            if ((int)rotation % 360 > 270 || (int)rotation % 360 < 90)
                (*sfx).at(2)->play();
            else
                (*sfx).at(19)->play();
            return 3;
        }

        // true -> set phase touchdown
    }
    return 0;
}

void Lander::draw(RenderWindow* window, std::vector<Texture>* textures, int phase)
{
    Sprite landerSprite;
    landerSprite.setTexture(textures->at(0));
    landerSprite.setOrigin(Vector2f(128, 128));
    landerSprite.setPosition(Vector2f(position.x, position.y));
    landerSprite.setRotation(rotation);

    Sprite boosterSprite;
    int boosterLength = (throttle / 1.0) * 125;
    boosterLength = -(boosterLength > 125 ? 125 : boosterLength);


    boosterSprite.setTexture(textures->at(1));
    boosterSprite.setOrigin(Vector2f(128, 0));
    boosterSprite.setPosition(Vector2f(position.x, position.y));
    boosterSprite.setPosition(Vector2f(position.x + cos(rotation * (PI/180) - 0.5 * PI) * boosterLength,
                                       position.y + sin(rotation * (PI/180) - 0.5 * PI) * boosterLength));
    boosterSprite.setRotation(rotation);

    if (phase == 2)
        window->draw(boosterSprite);

    window->draw(landerSprite);

    /*sf::Vertex line[] =
    {
        sf::Vertex(sf::Vector2f(position.x, position.y)),
        sf::Vertex(sf::Vector2f(position.x + hitpoints[1].x*rocketPower, position.y + hitpoints[1].y*rocketPower))
    };

    window->draw(line, 2, sf::Lines);*/
}

double Lander::calcGravitationForce()
{
    //Fgrav = G * (mM / r^2)

    double r = MOONRADIUS + 150; // Moon position is (0,0)

    double Fgrav = G * (this->mass * MOONMASS / pow(r, 2));

    return Fgrav;
}
