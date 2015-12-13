#include <SFML/Graphics.hpp>
#include <iostream>
#include "Audio.h"
#include "SurfaceGenerator.h"
#include "Lander.h"

using namespace sf;

enum Phase
{
    MENU,
    ORBIT,
    DEORBIT,
    LANDED
};

enum Sfx
{
    NOISE,
    LANDED_PART,
    LANDED_FULL,
};

class Game
{
    public:
        Game(RenderWindow* window);
        void initialize();
        void update();
        void draw();
        bool isWindowOpen();

        void loadAudio(std::vector<std::string> fileNames);
        void loadTextures(std::vector<std::string> fileNames);

        int randint(int low, int high, int seed);
        int randint(int low, int high);
    protected:
    private:
        RenderWindow* window;
        View view;
        double zoom = 20;

        int windowWidth = 1280;
        int windowHeight = 720;

        Vector2f viewPos { 0, 0 };

        Time frameTime;
        Time totalTime;
        Clock clock;
        int frame = 0;

        std::vector<std::string> audioFileNames { "noise.wav", "landed.wav", "landed-full.wav", "5-percent.wav",
                                                  "altitude-velocity-light.wav", "drifting-to-the-right.wav",
                                                  "faint-shadow.wav", "watch-your-shadow.wav", "lunar-velocity.wav",
                                                  "700-feet.wav", "600-feet.wav", "540-feet.wav", "400-feet.wav",
                                                  "350-feet.wav", "300-feet.wav", "100-feet.wav", "75-feet.wav",
                                                  "50-feet.wav", "40-feet.wav", "houston-problem.wav" };
        std::vector<std::string> textureFileNames { "lander.png", "booster.png", "menu.png", "select.png" };

        std::vector<Audio*> sfx;
        std::vector<Texture> textures;

        //----

        Lander lander;
        SurfaceGenerator surfaceGenerator;

        int phase = MENU;

        int mission = 11;
};
