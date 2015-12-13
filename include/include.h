#include <cstdlib>
#include <iostream>
#include <SFML/Graphics.hpp>

#pragma once

using namespace sf;

const double G = 6.674e-11;
const double MOONMASS = 7.342e22;
const double MOONRADIUS = 1737e3;
const double PI = 3.141592654;

Vector2<double> normalize(Vector2<double> vect)
{
    double length = sqrt(pow(vect.x, 2) + pow(vect.y, 2));
    return Vector2<double>(vect.x / length, vect.y / length);
}

void printVector(Vector2<double> vect, std::string name)
{
    std::cout << name << ": X: " << vect.x << ", Y: " << vect.y << "\n";
}

enum Textures
{
    LANDER
};
