#include "SurfaceGenerator.h"
#include <iostream>

using namespace sf;

int randint(int low, int high, int seed);

SurfaceGenerator::SurfaceGenerator()
{

}

void SurfaceGenerator::init(int seed)
{
    std::cout << "Generating surface" << "\n";
    //surface = new double[length];

    for (int i = 0; i < length; i++)
        surface[i] = 0;
    surface[0] = 0.3;
    surface[length-1] = 0.3;

    midPointDisplacement(surface, (int)length, seed, 0);
}

void SurfaceGenerator::drawSurface(RenderWindow* window, Vector2<double> position)
{
    for (int i = -length; i < length * 2; i++)
    {
        //int index = i >= length ? i - length : i;
        int index = i < 0 ? length + i : i % 4097;

        ConvexShape surfaceSlice;
        surfaceSlice.setPointCount(4);
        surfaceSlice.setFillColor(Color(150,150,150));

        int sliceWidth = index == 4096 ? 1000 : metersPerValue;
        surfaceSlice.setPoint(0, Vector2f(0, baseHeight + surface[index] * surfaceHeight));
        surfaceSlice.setPoint(1, Vector2f(sliceWidth, baseHeight + surface[index + 1] * surfaceHeight));
        surfaceSlice.setPoint(2, Vector2f(sliceWidth, 0));
        surfaceSlice.setPoint(3, Vector2f(0, 0));

        surfaceSlice.setPosition((position.x + metersPerValue * i), position.y);
        surfaceSlice.setScale(1, -1);

        window->draw(surfaceSlice);
    }
}

double SurfaceGenerator::getHeight(double position)
{
    int index = (int)(position / 16) % 4097;
    //std::cout << baseHeight + surface[index] * surfaceHeight << "\n";
    //std::cout << index << " " << surface[index] << " " << baseHeight + surface[index + 1] * surfaceHeight << "\n";
    return baseHeight + surface[index + 1] * surfaceHeight;
}

void SurfaceGenerator::midPointDisplacement(double* values, int length, int seed, int skip)
{
    int steps = log2(length - 1);

    double randrange = 1.0;
    double roughness = 0.6;

    //length = length - pow(2, skip);
    //std::cout << length << "\n";
    length = length - 1;
    for (int i = 0; i < skip; i++)
    {
        length /= 2;
    }
    length++;

    for (int i = (length - 1) / 2; i > 0; i /= 2)
    {
        //std::cout << i << "\n";
        for (int j = i; j < length; j += 2*i)
        {
            double average = (values[j - i] + values[j + i]) / 2.0;
            values[j] = average + randint(-100 * randrange, 100 * randrange, seed + j) / 100.0;
        }
        randrange *= roughness;
    }

    double max = values[0];
    double min = values[0];

    for (int i = 0; i < length; i++)
    {
        max = std::max(max, values[i]);
        min = std::min(min, values[i]);
    }

    for (int i = 0; i < length; i++)
    {
        values[i] = (values[i] - min) / (max - min);
        /*for (int j = 0; j < values[i] * 40.0; j++)
            std::cout << "|";
        std::cout << "\n";*/
    }
}









