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
        void resetGame();

        void loadAudio(std::vector<std::string> fileNames);
        void loadTextures(std::vector<std::string> fileNames);

        int randomint(int low, int high);
    protected:
    private:
        RenderWindow* window;
        View view;
        const double STARTZOOM = 4;
        double zoom = STARTZOOM;
        double zoomGoal = STARTZOOM;

        int windowWidth = 1280;
        int windowHeight = 720;

        Vector2f viewPos { 0, 0 };
        Vector2f viewPosOffset { 0, 0 }; //relative to lander

        Time frameTime;
        Time totalTime;
        Clock clock;
        int frame = 0;

        /*std::vector<std::string> audioFileNames { "noise.wav", "landed.wav", "landed-full.wav", "5-percent.wav",        //0 1 2 3
                                                  "altitude-velocity-light.wav", "drifting-to-the-right.wav",           //4 5
                                                  "faint-shadow.wav", "watch-your-shadow.wav", "lunar-velocity.wav",    //6 7 8
                                                  "700-feet.wav", "600-feet.wav", "540-feet.wav", "400-feet.wav",       //9 10 11 12
                                                  "350-feet.wav", "300-feet.wav", "100-feet.wav", "75-feet.wav",        //13 14 15 16
                                                  "50-feet.wav", "40-feet.wav", "houston-problem.wav",                  //17 18 19
                                                  "go-for-landing1.wav", "go-for-landing2.wav", "go-for-pdi1.wav",      //20 21 22
                                                  "go-for-pdi2.wav", "alarm.wav" };                                      //23 24*/

        std::vector<std::string> audioFileNames { "noise.wav", "landed.wav", "landed-full.wav", "5-percent.wav",        //0 1 2 3
                                                  "altitude-velocity-light.wav", "drifting-to-the-right.wav",           //4 5
                                                  "faint-shadow.wav", "watch-your-shadow.wav", "lunar-velocity.wav",
                                                  "man-attitude.wav", "kicking-up-dust.wav",     //6 7 8
                                                  "3000-feet.wav", "2000-feet.wav", "700-feet.wav", "600-feet.wav",
                                                  "540-feet.wav", "400-feet.wav", "350-feet.wav", "300-feet.wav",
                                                  "220-feet.wav", "200-feet.wav", "100-feet.wav", "75-feet.wav",        //13 14 15 16
                                                  "50-feet.wav", "40-feet.wav", "houston-problem.wav",                  //17 18 19
                                                  "go-for-landing1.wav", "go-for-landing2.wav", "go-for-pdi1.wav",      //20 21 22
                                                  "go-for-pdi2.wav", "alarm.wav" };                                      //23 24

        std::vector<std::string> textureFileNames { "lander.png", "booster.png", "menu.png", "select.png",              //0 1 2 3
                                                  "altitude.png", "altitude-LM.png", "attitude.png", "attitude-LM.png", //4 5 6 7
                                                  "thrust.png", "tutorial.png", "landed.png", "steep.png",              //8 9 10 11
                                                  "upside-down.png", "velocity.png", "gameover-menu.png",               //12 13 14
                                                  "horizontal-warning.png", "vertical-warning.png" }; //15 16

        std::vector<Audio*> sfx;
        std::vector<Texture> textures;

        //----

        Lander* lander;
        SurfaceGenerator surfaceGenerator;

        int phase = MENU;
        int lastPhase = MENU;

        int mission = 11;

        int hudtransparency = 200;

        int goTrack = 21;

        Color hudColor { 0, 200, 0, 200 };

        bool choice = false;
        bool horizontalWarning = false;
        bool verticalWarning = false;
        bool lastHorizontalWarning = false;
        bool lastVerticalWarning = false;

};
