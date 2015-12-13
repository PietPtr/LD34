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
    landerSettings.position = Vector2<double>(0, -32000);
    landerSettings.velocity = Vector2<double>(1, 0);
    landerSettings.speed = 1150;
    landerSettings.sfx = &sfx;
    std::cout << &sfx << " &sfx\n";
    std::cout << landerSettings.sfx << " landerSettings.sfx\n";
    lander.init(landerSettings);

    mission = randint(11, 17);

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
            if (event.key.code == Keyboard::Return)
                phase = phase == DEORBIT ? ORBIT : DEORBIT;
            if (event.key.code == Keyboard::Space)
                sfx.at(0)->play();
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


    int maxOffset = 5000;
    //double viewLanderOffset = maxOffset + surfaceGenerator.getHeight(lander.getPosition().x) / lander.getPosition().y * maxOffset;
    double viewLanderOffset = maxOffset + 20000 / lander.getPosition().y * maxOffset;
    viewLanderOffset = viewLanderOffset > maxOffset ? maxOffset : viewLanderOffset;

    viewPos = Vector2f(lander.getPosition().x, lander.getPosition().y + viewLanderOffset);

    if (phase == MENU)
    {
        viewPos.x = viewPos.x + 500 * zoom;
        viewPos.y = viewPos.y + 200 * zoom;
    }

    frame++;
}

void Game::draw()
{
    window->clear();

    view.setSize(Vector2f(windowWidth, windowHeight));
    view.setCenter(viewPos);
    view.zoom(zoom);

    if (phase == MENU)
    {
        Sprite menuSprite;
        menuSprite.setTexture(textures.at(2));
        menuSprite.setPosition(viewPos);
        menuSprite.setScale(zoom, zoom);
        menuSprite.setOrigin(128, 128);
        window->draw(menuSprite);
    }

    int maxZoom = 20; int minZoom =  4;
    int maxAlt = 3e4; int minAlt = 2e4;
    zoom = ((-lander.getPosition().y - minAlt) / (maxAlt - minAlt)) * (maxZoom - minZoom);
    zoom = zoom < minZoom ? minZoom : zoom;
    zoom = zoom > maxZoom ? maxZoom : zoom;

    window->setView(view);

    //Vector2<double> surfacePosition = Vector2<double>(lander.getPosition().x - windowWidth / 2, lander.getPosition().y + windowHeight / 2);
    Vector2<double> surfacePosition;
    surfacePosition.x = (int)(lander.getPosition().x / 65563.0) * 65563.0;

    surfacePosition.y = 0;
    //std::cout << surfacePosition.x << " " << surfacePosition.y << "\n";
    surfaceGenerator.drawSurface(window, surfacePosition);

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
