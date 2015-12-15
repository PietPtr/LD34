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

int Lander::update(double dt, double totalTime, int phase, SurfaceGenerator* surfaceGenerator)
{
    this->dt = dt;
    this->totalTime = totalTime;
    this->phase = phase;

    altitude = -position.y - surfaceGenerator->getHeight(position.x);

    if (phase == 0)
        return phaseMenu();
    else if (phase == 1)
        return phaseOrbit();
    else if (phase == 2)
        return phaseDeorbit(surfaceGenerator);
    else if (phase == 3)
        return phaseTouchdown(surfaceGenerator);
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
    Vector2<double> orbitVelocity = Vector2<double>(1150, 0);

    position.x = position.x + orbitVelocity.x * dt;
    position.y = position.y + orbitVelocity.y * dt;

    if (Keyboard::isKeyPressed(Keyboard::Left)  || Keyboard::isKeyPressed(Keyboard::A))
        angularMomentum -= 4;
    if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
        angularMomentum += 4;

    rotation += angularMomentum * dt;

    return 1;
}

int Lander::phaseDeorbit(SurfaceGenerator* surfaceGenerator)
{
    if (checkLanded(surfaceGenerator) == 3)
        return 3;

    // Forces

    //gravitationForce = calcGravitationForce();
    gravitation = normalize(Vector2<double>(0, 1)) * calcGravitationForce(); // better safe than sorry
                                                                             // - marien, 2015

    //std::cout << "gravitation " << calcGravitationForce() << "\n";

    rocketPower = sqrt(pow(velocity.x, 2) + pow(velocity.y, 2)) * (mass / 25);
    thrust.x = cos(rotation * (PI/180) - 0.5 * PI) * rocketPower;
    thrust.y = sin(rotation * (PI/180) - 0.5 * PI) * rocketPower;

    Vector2<double> Fsum = gravitation + thrust;

    // Acceleration

    acceleration = Fsum / mass;
    //std::cout << "Fsum/mass " << Fsum.x / mass << ", " << Fsum.y / mass << "\n";

    //double accLength = sqrt(pow(acceleration.x, 2) + pow(acceleration.y, 2));
    //Vector2<double> normalizedAcceleration = Vector2<double>(acceleration.x / accLength, acceleration.y / accLength);

    //speed = speed + accLength * dt;
    /*velocity.x = velocity.y + acceleration.x * dt;
    velocity.y = velocity.y + acceleration.y * dt;*/
    velocity += acceleration * dt;

    //Vector2<double> velocityAndAcceleration = velocity * speed + normalizedAcceleration * accLength;
    //velocity = normalize(velocityAndAcceleration);

    // Position

    position += velocity * dt;

    //position.x = position.x + velocity.x * speed * dt;
    //position.y = position.y + velocity.y * speed * dt;

    // Rotation

    if (Keyboard::isKeyPressed(Keyboard::Left)  || Keyboard::isKeyPressed(Keyboard::A))
        angularMomentum -= 4;
    if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
        angularMomentum += 4;

    rotation += angularMomentum * dt;

    // Remaining updates


    altitude = -position.y - surfaceGenerator->getHeight(position.x);

    /*double previousThrottle = throttle;
    if (altitude < 4000)
        throttle = 2.0;
    if (altitude < 2000)
        throttle = 3.0;
    if (altitude < 1000)
        throttle = 3.0;
    if (altitude < 500)
        throttle = 5.0;
    if (altitude < 100 && altitude > 0)
        throttle = altitude / 100;
    if (altitude < 0)
        throttle = 0;
    if (altitude >= 4000)
        throttle = 1.0;*/

    //std::cout << previousThrottle << " " << throttle << "\n";

    if ((totalTime - lastSoundPlayed > randint(2, 5, lastSoundPlayed)) && totalTime - lastSoundPlayed > 2)
        playAtmosphericSound();

    //std::cout << "Altitude: " << altitude << "\n";

    //std::cout << totalTime - lastSoundPlayed << "\n";

    return 2;
}

int Lander::phaseTouchdown(SurfaceGenerator* surfaceGenerator)
{
    //std::cout << (int)rotation % 360 << " " << touchdownSlope << "\n";

    /*if (rotation - (360 - touchdownSlope) < 1)
        rotation = 360 - touchdownSlope;
    if (rotation < 360 - touchdownSlope)
        rotation += 1;
    if (rotation > 360 - touchdownSlope)
        rotation -= 1;*/

    if (deathCause == ALIVE)
    {
        double cs = cos(rotation * (PI/180));
        double sn = sin(rotation * (PI/180));
        double tcs = cos(touchdownAngle * (PI/180));
        double tsn = sin(touchdownAngle * (PI/180));
        if (touchdownStrut == 0)
        {
            hitpoints[1].x = -120 * tcs - 126 * tsn + (240 * cs - 0 * sn);
            hitpoints[1].y = -120 * tsn + 126 * tcs + (240 * sn + 0 * cs);
        }
        if (touchdownStrut == 1)
        {
            hitpoints[0].x = 120 * tcs - 126 * tsn + (-240 * cs - 0 * sn);
            hitpoints[0].y = 120 * tsn + 126 * tcs + (-240 * sn + 0 * cs);
        }

        if (touchdownStrut == 0 && checkCollision(hitpoints[1], surfaceGenerator) == -1)
            rotation += fabs(angularMomentum * dt);
        if (touchdownStrut == 1 && checkCollision(hitpoints[0], surfaceGenerator) == -1)
            rotation -= fabs(angularMomentum * dt);
    }


    return 3;
}

void Lander::playAtmosphericSound()
{
    int altOrRand = randint(0, 1, altitude);
    if (altOrRand == 0)
    {
        (*sfx).at(3 + randint(0, 7, position.x + position.y))->play();
        //(*sfx).at(6)->play();
    }
    else if (altOrRand = 1)
    {
        (*sfx).at(11 + heightSound)->play();
        heightSound++;
    }
    heightSound = heightSound > 11 ? 0 : heightSound;

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

int Lander::checkLanded(SurfaceGenerator* surfaceGenerator)
{
    // calculate hitpoint position
    double cs = cos(rotation * (PI/180));
    double sn = sin(rotation * (PI/180));
    hitpoints[0].x = -120 * cs - 126 * sn;
    hitpoints[0].y = -120 * sn + 126 * cs;
    hitpoints[1].x =  120 * cs - 126 * sn;
    hitpoints[1].y =  120 * sn + 126 * cs;

    //std::cout << "Altitude: " << altitude << "\n";

    // for

    for (int i = 0; i < 2; i++)
    {
        belowSurface = checkCollision(hitpoints[i], surfaceGenerator);

        if (belowSurface != -1)
        {
            touchdownStrut = i;
            touchdownAngle = rotation;
            touchdownPos = position;
            touchDownTime = totalTime;

            if (fabs(angularMomentum) < 30)
                angularMomentum = 30;

            position.y -= belowSurface;

            throttle = 0;
            if (velocity.x > 150)
                deathCause = VELOCITY;
            if (velocity.y > 150)
                deathCause = VELOCITY;

            if (((int)rotation % 360 > 320 || (int)rotation % 360 < 40) && deathCause == ALIVE)
                deathCause = ALIVE;
            else if (((int)rotation % 360 > 270 || (int)rotation % 360 < 90) && deathCause == ALIVE)
                deathCause = STEEP;

            else if (deathCause == ALIVE)
                deathCause = UPSIDE_DOWN;

            if (deathCause == ALIVE)
                sfx->at(2)->play();
            else
                sfx->at(25)->play();

            std::cout << "Touchdown velocity";
            printVector(velocity, "");

            return 3;
        }

        // find surface points under hitpoints
        /*int index = (int)((position.x + hitpoints[i].x) / 16.0) % 4097;
        index = index > 4097 ? 0 : index;

        //std::cout << (int)(position.x + hitpoints[i].x) % 16 + 16 * index << " " << (position.x + hitpoints[i].x) << " " << ((int)(position.x + hitpoints[i].x) % 16 + 16 * index) - (position.x hitpoints[i].x) <<  "\n";
        double baseHeight = surfaceGenerator->getBaseHeight();
        double surfaceHeight = surfaceGenerator->getSurfaceHeight();
        //double height1 = baseHeight + surfaceGenerator->surface[index] * surfaceHeight;         //maybe something is wrong here
        //double height2 = baseHeight + surfaceGenerator->surface[index + 1] * surfaceHeight;
        double height1 = surfaceGenerator->getHeight(position.x + hitpoints[i].x);
        double height2 = surfaceGenerator->getHeight(position.x + hitpoints[i].x + 16);

        // create line function between surface points
        double x1 = (int)(position.x + hitpoints[i].x) % 16 + 16 * index;
        double y1 = height1;
        double y2 = height2;
        double a = (abs(height1 - height2)) / 16.0;
        double b = y1 - a * x1;
        //std::cout << "y = " << a << "x + " << b << "    ";

        // check hitpoints.y < line(hitpoints.x)
        if (-position.y - hitpoints[i].y < a * x1 + b)
        {
            //std::cout << "Lunar contact!" << "\n";
            /*std::cout << i << ": " << -position.y + hitpoints[i].y << " < " << a * (position.x + hitpoints[i].x) + b
                      << ", pos.y: " << -position.y << ", h1: " << height1 << ", h2: " << height2 << ", get: " << surfaceGenerator->getHeight(position.x + hitpoints[i].x)
                      << ", xpos: " << position.x << ", index: " << index << ", x1: " << x1 << ", y = " << a << "x + " << b << "\n";
            //touchdownSlope = atan(a) * (180/PI);
            /*double d = abs(touchdownSlope - (int)(rotation + 90) % 360) % 360;
            double r = d > 180 ? 360 - d : d;

            if ((int)rotation % 360 > 270 && (int)rotation % 360 < 90)
                (*sfx).at(2)->play();
            else
                (*sfx).at(19)->play();
            //std::cout << hillAngle << " degrees hill. Lander has a roll of " << (int)rotation % 360 << " degrees. The difference is " << r << "\n";

            touchdownStrut = i;

            //if (rotation != 360 - hillAngle)

            return 3;
        }*/

        // true -> set phase touchdown
    }
    return 0;
}

double Lander::checkCollision(Vector2i hitpoint, SurfaceGenerator* surfaceGenerator)
{
    Vector2<double> pos;
    /*if (phase == 3 && touchdownStrut == 0)
        pos = Vector2<double>(position.x - hitpoints[1].x, position.y + hitpoints[1].y);
    else if (phase == 3 && touchdownStrut == 1)
        pos = Vector2<double>(position.x + hitpoints[0].x, position.y + hitpoints[0].y );
    else
        pos = position;*/
    pos = position;

    // find surface points under hitpoints
    int index = (int)((position.x + hitpoint.x) / 16.0) % 4097;
    index = index > 4097 ? 0 : index;

    //std::cout << (int)(position.x + hitpoints[i].x) % 16 + 16 * index << " " << (position.x + hitpoints[i].x) << " " << ((int)(position.x + hitpoints[i].x) % 16 + 16 * index) - (position.x hitpoints[i].x) <<  "\n";
    double baseHeight = surfaceGenerator->getBaseHeight();
    double surfaceHeight = surfaceGenerator->getSurfaceHeight();
    //double height1 = baseHeight + surfaceGenerator->surface[index] * surfaceHeight;         //maybe something is wrong here
    //double height2 = baseHeight + surfaceGenerator->surface[index + 1] * surfaceHeight;
    double height1 = surfaceGenerator->getHeight(pos.x + hitpoint.x);
    double height2 = surfaceGenerator->getHeight(pos.x + hitpoint.x + 16);

    // create line function between surface points
    double x1 = (int)(pos.x + hitpoint.x) % 16 + 16 * index;
    double y1 = height1;
    double y2 = height2;
    double a = (abs(height1 - height2)) / 16.0;
    double b = y1 - a * x1;
    //std::cout << "y = " << a << "x + " << b << "    ";

    // check hitpoints.y < line(hitpoints.x)
    if (-pos.y - hitpoint.y < a * x1 + b)
    {
        /*std::cout << i << ": " << -position.y + hitpoints[i].y << " < " << a * (position.x + hitpoints[i].x) + b
                  << ", pos.y: " << -position.y << ", h1: " << height1 << ", h2: " << height2 << ", get: " << surfaceGenerator->getHeight(position.x + hitpoints[i].x)
                  << ", xpos: " << position.x << ", index: " << index << ", x1: " << x1 << ", y = " << a << "x + " << b << "\n";*/
        //touchdownSlope = atan(a) * (180/PI);
        /*double d = abs(touchdownSlope - (int)(rotation + 90) % 360) % 360;
        double r = d > 180 ? 360 - d : d;*/


        //std::cout << hillAngle << " degrees hill. Lander has a roll of " << (int)rotation % 360 << " degrees. The difference is " << r << "\n";


        //if (rotation != 360 - hillAngle)

        return (a * x1 + b) - (-pos.y - hitpoint.y);
    }
    return -1;
}

void Lander::draw(RenderWindow* window, std::vector<Texture>* textures, int phase)
{
    Sprite landerSprite;
    landerSprite.setTexture(textures->at(0));
    landerSprite.setOrigin(Vector2f(128, 128));
    landerSprite.setPosition(Vector2f(position.x, position.y));
    landerSprite.setRotation(rotation);
    if (phase == 3)
    {
        if (touchdownStrut == 0)
            landerSprite.setOrigin(Vector2f(128 - 120, 128 + 126));
        else if (touchdownStrut == 1)
            landerSprite.setOrigin(Vector2f(128 + 120, 128 + 126));
        landerSprite.setPosition(Vector2f(position.x + hitpoints[touchdownStrut].x, position.y + hitpoints[touchdownStrut].y));
    }

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

    if (deathCause == ALIVE)
    {
        window->draw(landerSprite);
    }
    else
    {
        double explodeFactor = 1 + (totalTime - touchDownTime) * 4;
        Vector2f explosionPos = Vector2f(position.x, position.y);
        int fragments = 9; // squared
        int stepSize = 256.0 / fragments;
        for (int x = 0; x < 256; x += stepSize)
        {
            for (int y = 0; y < 256; y += stepSize)
            {
                Sprite lmFragment;
                lmFragment.setTexture(textures->at(0));
                lmFragment.setTextureRect(IntRect(x, y, stepSize, stepSize));

                Vector2f fragmentOffset;
                double cs = cos(rotation * (PI/180));
                double sn = sin(rotation * (PI/180));
                fragmentOffset.x = ((x - 128) * explodeFactor) * cs - ((y - 128) * explodeFactor) * sn;
                fragmentOffset.y = ((x - 128) * explodeFactor) * sn + ((y - 128) * explodeFactor) * cs;

                lmFragment.setPosition(explosionPos + fragmentOffset);
                lmFragment.setRotation(rotation);
                window->draw(lmFragment);
            }
        }
    }


    /*sf::Vertex line[] =
    {
        sf::Vertex(sf::Vector2f(position.x, position.y)),
        sf::Vertex(sf::Vector2f(position.x + hitpoints[1].x*rocketPower, position.y + hitpoints[1].y*rocketPower))
    };

    window->draw(line, 2, sf::Lines);*/



    /*double radius = 50;
    CircleShape hitpoint(radius);
    hitpoint.setFillColor(Color(0, 200, 200, 155));
    hitpoint.setPosition(Vector2f(position.x + hitpoints[0].x, position.y + hitpoints[0].y));
    hitpoint.setOrigin(Vector2f(radius, radius));
    window->draw(hitpoint);

    CircleShape hitpoint2(radius);
    hitpoint2.setFillColor(Color(200, 0, 0, 155));
    hitpoint2.setPosition(Vector2f(position.x + hitpoints[1].x, position.y + hitpoints[1].y));
    hitpoint2.setOrigin(Vector2f(radius, radius));
    window->draw(hitpoint2);

    CircleShape pos(radius);
    pos.setFillColor(Color(0, 200, 0, 155));
    pos.setPosition(Vector2f(position.x, position.y));
    pos.setOrigin(Vector2f(radius, radius));
    window->draw(pos);*/
}

double Lander::calcGravitationForce()
{
    //Fgrav = G * (mM / r^2)

    double r = MOONRADIUS; // Moon position is (0,0)

    double Fgrav = G * (this->mass * MOONMASS / pow(r, 2));

    return Fgrav;
}
