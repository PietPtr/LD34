#ifndef SURFACEGENERATOR_H
#define SURFACEGENERATOR_H
#include <SFML/Graphics.hpp>

using namespace sf;

class SurfaceGenerator
{
    public:
        SurfaceGenerator();
        void init(int seed);
        void drawSurface(RenderWindow* window, Vector2<double> position);
        void midPointDisplacement(double* values, int length, int seed, int skip);
        double getHeight(double position);
        double getBaseHeight() { return baseHeight; }
        double getSurfaceHeight() { return surfaceHeight; }
        double surface[4097];
    protected:
    private:
        int metersPerValue = 16;
        double surfaceHeight = 4000;
        double baseHeight = 20000;
        int length = 4097;
};

#endif // SURFACEGENERATOR_H
