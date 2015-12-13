#include <SFML/Graphics.hpp>
//#include "include.h"
#include "Game.h"
#include "Lander.h"

using namespace sf;

Game::Game(RenderWindow* _window)
{
    window = _window;
}

void Game::initialize()
{
    loadAudio(audioFileNames);
    loadTextures(textureFileNames);

    LanderSettings landerSettings;
    landerSettings.position = Vector2<double>(16 * 2048, -32000);
    landerSettings.velocity = Vector2<double>(1, 0);
    landerSettings.speed = 1150;
    landerSettings.sfx = &sfx;
    std::cout << &sfx << " &sfx\n";
    std::cout << landerSettings.sfx << " landerSettings.sfx\n";
    lander.init(landerSettings);

    mission = 11;//randint(11, 17);
    std::cout << "The mission is Apollo " << mission << "\n";

    surfaceGenerator.init(mission);

    sfx[NOISE]->loop(true);
    sfx[NOISE]->play();

}

void Game::update()
{
    Event event;
    while (window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
            window->close();
        if (event.type == Event::KeyPressed)
        {
            if (event.key.code == Keyboard::Escape)
                window->close();
            if (event.key.code == Keyboard::Num1)
                phase = MENU;
            if (event.key.code == Keyboard::Num2)
            {
                phase = ORBIT;
            }
            if (event.key.code == Keyboard::Num3)
                phase = DEORBIT;
            if (event.key.code == Keyboard::Num4)
                phase = LANDED;
            if (event.key.code == Keyboard::Space)
                sfx.at(0)->play();
            if (event.key.code == Keyboard::Up)
                choice = !choice;
            if (event.key.code == Keyboard::Down)
                choice = !choice;
            if (event.key.code == Keyboard::Return)
            {
                if (choice)
                {
                    window->close();
                }
                else
                {
                    phase = 1;
                }
            }
        }
        if (event.type == Event::Resized)
        {
            windowWidth = event.size.width;
            windowHeight = event.size.height;
        }
        if (event.type == Event::MouseWheelMoved)
        {
            if (event.mouseWheel.delta > 0)
                zoom /= 1.1;
            else
                zoom *= 1.1;
        }
    }

    //Manual view control
    double viewMoveSpeed = 20;
    if (Keyboard::isKeyPressed(Keyboard::Home))
        viewPos.y -= viewMoveSpeed;
    if (Keyboard::isKeyPressed(Keyboard::End))
        viewPos.y += viewMoveSpeed;
    if (Keyboard::isKeyPressed(Keyboard::Delete))
        viewPos.x -= viewMoveSpeed;
    if (Keyboard::isKeyPressed(Keyboard::PageDown))
        viewPos.x += viewMoveSpeed;

    frameTime = clock.restart();
    totalTime += frameTime;

    double dt = frameTime.asSeconds() * 1;

    int updateReturn = lander.update(dt, phase, &surfaceGenerator);
    if (updateReturn != -1)
        phase = updateReturn;
    //if (updateReturn == 3)

    viewPos = lander.getPosition();

    int maxOffset = 5000;
    //double viewLanderOffset = maxOffset + surfaceGenerator.getHeight(lander.getPosition().x) / lander.getPosition().y * maxOffset;
    double viewLanderOffset = maxOffset + 20000 / lander.getPosition().y * maxOffset;
    viewLanderOffset = (viewLanderOffset > maxOffset ? maxOffset : viewLanderOffset);

    if (phase == MENU)
    {
        viewPos.x = viewPos.x + viewPosOffset.x * zoom;
        viewPos.y = viewPos.y + viewPosOffset.y * zoom;
    }
    else if (phase == ORBIT)
    {
        if (lastPhase == MENU)
            sfx.at(20 + randint(0, 1) * 2 + 1)->play();

        viewPos.x = viewPos.x + viewPosOffset.x * zoom;
        viewPos.y = viewPos.y + viewPosOffset.y * zoom;

        double transitionx = 500 * dt / 14;
        double transitiony = 300 * dt / 14;
        viewPosOffset.x = viewPosOffset.x - transitionx < 0 ? 0 : viewPosOffset.x - transitionx;
        viewPosOffset.y = viewPosOffset.y - transitiony < 0 ? 0 : viewPosOffset.y - transitiony;

        if (viewPosOffset.x == 0 && viewPosOffset.y == 0)
            phase = DEORBIT;
    }
    else if (phase == LANDED || phase == DEORBIT)
    {
        viewPos = lander.getPosition();
    }

    if (abs(zoom - zoomGoal) < 0.05)
        zoom = zoomGoal;
    if (zoom < zoomGoal)
        zoom += 0.05;
    if (zoom > zoomGoal)
        zoom -= 0.05;

    lastPhase = phase;

    frame++;
}

void Game::draw()
{
    window->clear();

    view.setSize(Vector2f(windowWidth, windowHeight));
    view.setCenter(viewPos);
    view.zoom(zoom);
    window->setView(view);

    //Vector2<double> surfacePosition = Vector2<double>(lander.getPosition().x - windowWidth / 2, lander.getPosition().y + windowHeight / 2);
    Vector2<double> surfacePosition;
    surfacePosition.x = (int)(lander.getPosition().x / 65563.0) * 65563.0;

    surfacePosition.y = 0;
    //std::cout << surfacePosition.x << " " << surfacePosition.y << "\n";
    surfaceGenerator.drawSurface(window, surfacePosition);

    if (phase == MENU)
    {
        Sprite menuSprite;
        menuSprite.setTexture(textures.at(2));
        menuSprite.setPosition(viewPos);
        menuSprite.setScale(zoom, zoom);
        menuSprite.setOrigin(128, 128);
        menuSprite.setColor(Color(0, 200, 0));
        window->draw(menuSprite);

        Sprite selectSprite;
        selectSprite.setTexture(textures.at(3));
        selectSprite.setPosition(viewPos + Vector2f(-128 * zoom, (-128 + choice * 96) * zoom));
        selectSprite.setScale(zoom, zoom);
        selectSprite.setOrigin(64, 0);
        selectSprite.setColor(Color(0, 200, 0));
        window->draw(selectSprite);

        zoomGoal = zoom;
    }

    if (phase == DEORBIT || phase == ORBIT || phase == LANDED)
    {
        int maxZoom = 30; int minZoom =  4;
        int maxAlt = 3e4; int minAlt = 2e4;
        zoomGoal = ((-lander.getPosition().y - minAlt) / (maxAlt - minAlt)) * (maxZoom - minZoom);
        zoomGoal = zoomGoal < minZoom ? minZoom : zoomGoal;
        zoomGoal = zoomGoal > maxZoom ? maxZoom : zoomGoal;

        //metres
        Sprite altitudeMeter;
        altitudeMeter.setTexture(textures.at(4));
        altitudeMeter.setPosition(viewPos + Vector2f(windowWidth / 2.0, -windowHeight / 2.0) * (float)zoom);
        altitudeMeter.setScale(zoom, zoom);
        altitudeMeter.setColor(Color(0, 200, 0, hudtransparency));
        altitudeMeter.setOrigin(Vector2f(50, -2));
        window->draw(altitudeMeter);

        Sprite attitudeMeter;
        attitudeMeter.setTexture(textures.at(6));
        attitudeMeter.setPosition(viewPos + Vector2f(-windowWidth / 2.0, -windowHeight / 2.0) * (float)zoom);
        attitudeMeter.setScale(zoom, zoom);
        attitudeMeter.setColor(Color(0, 200, 0, hudtransparency));
        attitudeMeter.setOrigin(Vector2f(-2, -2));
        window->draw(attitudeMeter);

        Sprite attitudeLM;
        attitudeLM.setTexture(textures.at(7));
        attitudeLM.setPosition(viewPos + Vector2f(-windowWidth / 2.0 + 80, -windowHeight / 2.0 + 80) * (float)zoom);
        attitudeLM.setScale(zoom, zoom);
        attitudeLM.setColor(Color(0, 200, 0, hudtransparency));
        attitudeLM.setOrigin(Vector2f(80, 80));
        attitudeLM.setRotation(lander.getRotation());
        window->draw(attitudeLM);
    }

    if (phase == DEORBIT)
    {
        Sprite altitudeLM;
        double lmOffset = -170 + (lander.getAltitude() / 15000) * 533;
        lmOffset = lmOffset < -170 ? -170 : lmOffset;
        lmOffset = lmOffset > 363 ? 363 : lmOffset;
        altitudeLM.setTexture(textures.at(5));
        altitudeLM.setPosition(viewPos + Vector2f(windowWidth / 2.0, -lmOffset) * (float)zoom);
        altitudeLM.setScale(zoom, zoom);
        altitudeLM.setColor(Color(0, 200, 0, hudtransparency));
        altitudeLM.setOrigin(46, 0);
        window->draw(altitudeLM);
    }

    lander.draw(window, &textures, phase);

    window->display();
}

bool Game::isWindowOpen()
{
    return window->isOpen();
}

void Game::loadAudio(std::vector<std::string> audioFileNames)
{
    std::cout << "Loading audio..." << audioFileNames.size() << "\n";
    for (int i = 0; i < audioFileNames.size(); i++)
    {
        sfx.push_back(new Audio());
        sfx.back()->init("audio/" + audioFileNames[i]);
        std::cout << "Loaded audio " << "audio/" + audioFileNames[i] << "\n";
    }
}

void Game::loadTextures(std::vector<std::string> textureFileNames)
{
    for (int i = 0; i < textureFileNames.size(); i++)
    {
        //textures.push_back()
        Texture texture;
        if (!texture.loadFromFile("textures/" + textureFileNames.at(i)))
            window->close();
        else
            std::cout << "Loaded texture " << "textures/" + textureFileNames.at(i) << "\n";
        textures.push_back(texture);
    }
}

int Game::randint(int low, int high)
{
    int value = rand() % (high + 1 - low) + low;
    srand(totalTime.asMicroseconds() * value * rand());

    return value;
}
