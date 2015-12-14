#include <SFML/Graphics.hpp>
//#include "include.h"
#include "Game.h"
#include "Lander.h"

using namespace sf;

int randint(int low, int high, int seed);

Game::Game(RenderWindow* _window)
{
    window = _window;
}

void Game::initialize()
{
    loadAudio(audioFileNames);
    loadTextures(textureFileNames);

    lander = new Lander();

    LanderSettings landerSettings;
    landerSettings.position = Vector2<double>(16 * 2048, -28000);
    landerSettings.velocity = Vector2<double>(500, 0);
    //landerSettings.speed = 1150;
    landerSettings.sfx = &sfx;
    lander->init(landerSettings);

    mission = 16;//randint(11, 17);

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
                if (phase == MENU)
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
                else if (phase == LANDED)
                {
                    if (choice)
                    {
                        window->close();
                    }
                    else
                    {
                        resetGame();
                    }
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

    frameTime = clock.restart();
    totalTime += frameTime;

    if (frame == 1)
    {
        goTrack = 27 + randomint(0, 1) * 2;
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


    double dt = frameTime.asSeconds() * 1;

    int updateReturn = lander->update(dt, totalTime.asSeconds(), phase, &surfaceGenerator);
    if (updateReturn != -1)
        phase = updateReturn;
    //if (updateReturn == 3)

    viewPos = lander->getPosition();

    int maxOffset = 5000;
    //double viewLanderOffset = maxOffset + surfaceGenerator.getHeight(lander.getPosition().x) / lander.getPosition().y * maxOffset;
    double viewLanderOffset = maxOffset + 20000 / lander->getPosition().y * maxOffset;
    viewLanderOffset = (viewLanderOffset > maxOffset ? maxOffset : viewLanderOffset);

    double zoomSpeed = 0.1;

    if (phase == MENU)
    {
        viewPos.x = viewPos.x + viewPosOffset.x * zoom;
        viewPos.y = viewPos.y + viewPosOffset.y * zoom;
    }
    else if (phase == ORBIT)
    {
        zoomSpeed = 0.3;

        if (lastPhase == MENU)
            sfx.at(goTrack)->play();
            //sfx.at(20 + randint(0, 1, (int)(totalTime.asSeconds())) * 2 + 1)->play();

        viewPos.x = viewPos.x + viewPosOffset.x * zoom;
        viewPos.y = viewPos.y + viewPosOffset.y * zoom;

        double transitionx = 500 * dt / 4;
        double transitiony = 300 * dt / 4;
        viewPosOffset.x = viewPosOffset.x - transitionx < 0 ? 0 : viewPosOffset.x - transitionx;
        viewPosOffset.y = viewPosOffset.y - transitiony < 0 ? 0 : viewPosOffset.y - transitiony;

        if (sfx.at(goTrack)->sound.getStatus() == 0)
            phase = DEORBIT;
    }
    else if (phase == DEORBIT)
    {
        viewPos = lander->getPosition();
    }
    else if (phase == LANDED)
    {
        viewPos = (Vector2f)(lander->getTouchdownPos());
    }

    horizontalWarning = lander->getVelocity().x < 150;
    verticalWarning = lander->getVelocity().y < 150;

    if (verticalWarning != lastVerticalWarning)
        sfx.at(30)->play();
    if (horizontalWarning != lastHorizontalWarning)
        sfx.at(30)->play();

    std::cout << horizontalWarning << " " << lastHorizontalWarning << "\n";


    if (abs(zoom - zoomGoal) < zoomSpeed)
        zoom = zoomGoal;
    if (zoom < zoomGoal)
        zoom += zoomSpeed;
    if (zoom > zoomGoal)
        zoom -= zoomSpeed;

    lastPhase = phase;
    lastHorizontalWarning = horizontalWarning;
    lastVerticalWarning = verticalWarning;

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
    surfacePosition.x = (int)(lander->getPosition().x / 65563.0) * 65563.0;

    surfacePosition.y = 0;
    //std::cout << surfacePosition.x << " " << surfacePosition.y << "\n";
    surfaceGenerator.drawSurface(window, surfacePosition);

    lander->draw(window, &textures, phase);

    if (phase == MENU)
    {
        Sprite menuSprite;
        menuSprite.setTexture(textures.at(2));
        menuSprite.setPosition(viewPos + Vector2f(0, -128) * (float)zoom);
        menuSprite.setScale(zoom, zoom);
        menuSprite.setOrigin(44, 0);
        menuSprite.setColor(Color(0, 200, 0));
        window->draw(menuSprite);

        Sprite selectSprite;
        selectSprite.setTexture(textures.at(3));
        selectSprite.setPosition(viewPos + Vector2f(-4 * zoom, (-128 + choice * 38) * zoom));
        selectSprite.setScale(zoom, zoom);
        selectSprite.setOrigin(64, 0);
        selectSprite.setColor(Color(0, 200, 0));
        window->draw(selectSprite);

        zoomGoal = STARTZOOM;
    }

    if (phase == ORBIT)
    {
        Sprite tutorialSprite;
        tutorialSprite.setTexture(textures.at(9));
        tutorialSprite.setPosition(viewPos + Vector2f(0, -windowHeight / 2.0 + 4) * (float)zoom);
        tutorialSprite.setColor(hudColor);
        tutorialSprite.setScale(zoom, zoom);
        tutorialSprite.setOrigin(230, 0);
        window->draw(tutorialSprite);
    }

    if (phase == DEORBIT || phase == ORBIT || phase == LANDED)
    {
        int maxZoom = 75; int minZoom =  2;
        int maxAlt = 15000; int minAlt = 0;
        zoomGoal = ((lander->getAltitude() - minAlt) / (maxAlt - minAlt)) * (maxZoom - minZoom);
        zoomGoal = zoomGoal < minZoom ? minZoom : zoomGoal;
        zoomGoal = zoomGoal > maxZoom ? maxZoom : zoomGoal;

        //metres
        Sprite altitudeMeter;
        altitudeMeter.setTexture(textures.at(4));
        altitudeMeter.setPosition(viewPos + Vector2f(windowWidth / 2.0, -windowHeight / 2.0 + 2) * (float)zoom);
        altitudeMeter.setScale(zoom, zoom);
        altitudeMeter.setColor(Color(0, 200, 0, hudtransparency));
        altitudeMeter.setOrigin(Vector2f(50, 0));
        window->draw(altitudeMeter);

        Sprite altitudeLM;
        int maxOffset = 534;
        double lmOffset = maxOffset - (lander->getAltitude() / 15000) * maxOffset;
        lmOffset = lmOffset < 0 ? 0 : lmOffset;
        lmOffset = lmOffset > maxOffset ? maxOffset : lmOffset;
        altitudeLM.setTexture(textures.at(5));
        altitudeLM.setPosition(viewPos + Vector2f(windowWidth / 2.0, -windowHeight / 2.0 + 2 + lmOffset) * (float)zoom);
        altitudeLM.setScale(zoom, zoom);
        altitudeLM.setColor(Color(0, 200, 0, hudtransparency));
        altitudeLM.setOrigin(46, 0);
        window->draw(altitudeLM);

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
        attitudeLM.setRotation(lander->getRotation());
        window->draw(attitudeLM);

        Sprite thrustText;
        thrustText.setTexture(textures.at(8));
        thrustText.setPosition(viewPos + Vector2f(-windowWidth / 2.0 + 2, -windowHeight / 2.0 + 168) * (float)zoom);
        thrustText.setScale(zoom, zoom);
        thrustText.setColor(Color(0, 200, 0, hudtransparency));
        window->draw(thrustText);

        RectangleShape thrustBar;
        thrustBar.setPosition(viewPos + Vector2f(-windowWidth / 2.0 + 69, -windowHeight / 2.0 + 169) * (float)zoom);
        thrustBar.setScale(zoom, zoom);
        thrustBar.setFillColor(Color(0, 0, 0, 0));
        thrustBar.setOutlineColor(Color(0, 200, 0, hudtransparency));
        thrustBar.setOutlineThickness(1);
        thrustBar.setSize(Vector2f(lander->getThrottle() / 10.0 * 92, 9));
        window->draw(thrustBar);
    }

    if (phase == DEORBIT)
    {
        Sprite horizontalWarn;
        horizontalWarn.setTexture(textures.at(15));
        horizontalWarn.setPosition(viewPos + Vector2f(-2, -windowHeight / 2.0 + 2) * (float)zoom);
        horizontalWarn.setScale(zoom, zoom);
        horizontalWarn.setColor(Color(200, 0, 0, 25));
        horizontalWarn.setOrigin(Vector2f(124, 0));

        Sprite verticalWarn;
        verticalWarn.setTexture(textures.at(16));
        verticalWarn.setPosition(viewPos + Vector2f(124 + 2, -windowHeight / 2.0 + 2) * (float)zoom);
        verticalWarn.setScale(zoom, zoom);
        verticalWarn.setColor(Color(200, 0, 0, 25));
        verticalWarn.setOrigin(Vector2f(124, 0));

        if (lander->getVelocity().x > 150)
        {
            horizontalWarn.setColor(Color(200, 0, 0, 255));
        }
        else if (lander->getVelocity().y > 150)
        {
            verticalWarn.setColor(Color(200, 0, 0, 255));
        }
        window->draw(horizontalWarn);
        window->draw(verticalWarn);
    }

    if (phase == LANDED)
    {
        DeathCause deathCause = lander->getDeathCause();
        Sprite resultText;
        resultText.setTexture(textures.at(10 + (int)deathCause));
        resultText.setOrigin(Vector2f(textures.at(10 + (int)deathCause).getSize().x / 2, 0));
        resultText.setPosition(viewPos + Vector2f(0, -windowHeight / 2.0 + 128) * (float)zoom);
        resultText.setScale(zoom, zoom);
        resultText.setColor(hudColor);
        window->draw(resultText);


        Sprite menuSprite;
        menuSprite.setTexture(textures.at(14));
        menuSprite.setPosition(viewPos + Vector2f(0, -128) * (float)zoom);
        menuSprite.setScale(zoom, zoom);
        menuSprite.setOrigin(108, 0);
        menuSprite.setColor(Color(0, 200, 0));
        window->draw(menuSprite);

        Sprite selectSprite;
        selectSprite.setTexture(textures.at(3));
        selectSprite.setPosition(viewPos + Vector2f(-68 * zoom, (-134 + choice * 38) * zoom));
        selectSprite.setScale(zoom, zoom);
        selectSprite.setOrigin(64, 0);
        selectSprite.setColor(Color(0, 200, 0));
        window->draw(selectSprite);
    }

    window->display();
}

void Game::resetGame()
{
    if (lander->getDeathCause() == ALIVE)
        sfx.at(2)->stop();
    else
        sfx.at(25)->stop();

    //delete &lander;
    lander = new Lander();
    LanderSettings landerSettings;
    landerSettings.position = Vector2<double>(16 * 2048, -28000);
    landerSettings.velocity = Vector2<double>(500, 0);
    landerSettings.sfx = &sfx;
    lander->init(landerSettings);

    lastPhase = MENU;
    phase = ORBIT;

    viewPosOffset = Vector2f(0, 0);
    zoom = STARTZOOM;
    zoomGoal = STARTZOOM;

    goTrack = 26 + randomint(0, 1) * 2;

    mission++;
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

int Game::randomint(int low, int high)
{
    srand(totalTime.asMicroseconds());
    int value = rand() % (high + 1 - low) + low;
    std::cout << totalTime.asMicroseconds() << "microseconds time\n";

    return value;
}
