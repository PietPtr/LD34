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

enum DeathCause
{
    ALIVE,
    STEEP,
    UPSIDE_DOWN,
};

class Lander
{
    public:
        Lander();
        void init(LanderSettings landerSettings);
        int  update(double dt, double totalTime, int phase, SurfaceGenerator* surfaceGenerator);
        int  phaseMenu();
        int  phaseOrbit();
        int  phaseDeorbit(SurfaceGenerator* surfaceGenerator);
        int  phaseTouchdown(SurfaceGenerator* surfaceGenerator);
        int  checkLanded(SurfaceGenerator* surfaceGenerator);
        void playAtmosphericSound();
        void draw(RenderWindow* window, std::vector<Texture>* textures, int phase);
        double checkCollision(Vector2i hitpoint, SurfaceGenerator* surfaceGenerator);
        double calcGravitationForce();
        bool isSoundPlaying();
        Vector2f getPosition() { return (Vector2f)(position); }
        double getAltitude() { return altitude; }
        double getRotation() { return rotation; }
        double getThrottle() { return throttle; }
        Vector2<double> getTouchdownPos() { return touchdownPos; }
        DeathCause getDeathCause() { return deathCause; }
    protected:
    private:
        Vector2<double> acceleration; // m/s^2
        Vector2<double> position;
        Vector2<double> velocity;     // keep this normalized please
        Vector2<double> gravitation;
        Vector2<double> thrust;
        Vector2i hitpoints[2];
        double speed;                 // in meters per second
        double gravitationForce;
        double rotation = 225;
        double angularMomentum = 0;
        double mass = 15200; // kg
        double dt;
        double totalTime;
        double rocketPower = 24685; //Fsum = 0 when this is ~25000
        const double maxThrust = 45000;
        double throttle = 24685/45000;
        double altitude = 0;
        double lastSoundPlayed = 0; // time
        double touchdownAngle = 0;
        double touchDownTime = 0;
        int touchdownStrut = -1;
        Vector2<double> touchdownPos;
        double stableRotation = 0;
        int phase;
        int heightSound = 0;
        DeathCause deathCause = ALIVE;
        double belowSurface = -1;

        LanderSettings settings;
        std::vector<Audio*>* sfx;
};

#endif // LANDER_H
