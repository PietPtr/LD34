#ifndef LANDER_H
#define LANDER_H
#include <SFML/Graphics.hpp>
#include "SurfaceGenerator.h"
#include "Audio.h"

using namespace sf;

struct LanderSettings
{
    Vector2<double> position;
    Vector2<double> velocity;
    double speed;
    std::vector<Audio*>* sfx;
};

class Lander
{
    public:
        Lander();
        void init(LanderSettings landerSettings);
        int  update(double dt, int phase, SurfaceGenerator* surfaceGenerator);
        int  phaseMenu();
        int  phaseOrbit();
        int  phaseDeorbit(SurfaceGenerator* surfaceGenerator);
        int  phaseTouchdown();
        int  checkCollision(SurfaceGenerator* surfaceGenerator);
        void playAtmosphericSound();
        void draw(RenderWindow* window, std::vector<Texture>* textures, int phase);
        double calcGravitationForce();
        bool isSoundPlaying();
        Vector2f getPosition() { return (Vector2f)(position); }
        double getAltitude() { return altitude; }
        double getRotation() { return rotation; }
    protected:
    private:
        Vector2<double> acceleration; // m/s^2
        Vector2<double> position;
        Vector2<double> velocity;     // keep this normalized please
        Vector2<double> gravitation;
        Vector2<double> thrust;
        double speed;                 // in meters per second
        double gravitationForce;
        double rotation = 270;
        double angularMomentum = 0;
        double mass = 15200; // kg
        double dt;
        double totalTime;
        double rocketPower = 24685; //Fsum = 0 when this is ~25000
        const double maxThrust = 45000;
        double throttle = 24685/45000;
        double altitude = 0;
        double lastSoundPlayed = 0; // time

        LanderSettings settings;
        std::vector<Audio*>* sfx;
};

#endif // LANDER_H
