
#include "game.hpp"
#include "Nodes.hpp"
#include "OpenGLRenderer.hpp"

#include "RichText+.hpp"
#include "Entity.hpp"

unsigned int wishedFPS = 60;
unsigned int Path::currentID = 0;

void gameLoop()
{
    System mainSystem;

    Widget::Init(*mainSystem.returnRenderWindowPointer());

    HorizontalSlider mySlider(sf::Vector2f(0,0), sf::Vector2f(500,50), 0.1);
    mySlider.setBackgroundTexture("myImage.png");
    mySlider.setSliderTexture("Slider.png");
    mainSystem.addWidget(mySlider, "me");

    Entity myEntity("Entity", 5,5,5, "resources/objects/nanosuit/nanosuit.obj");

    for (int i(1); i < 10 ; i++)
    {
        std::stringstream myStream;
        myStream << "me" << i;

        mySlider.setPosition(sf::Vector2f(mySlider.getPosition().x, mySlider.getPosition().y + 50));
        mainSystem.addWidget(mySlider, myStream.str());
    }

    do
    {
        mainSystem.Update();

        mainSystem.Render();

    }while (1);
}

void System::Update()
{
    if (clockFPS.getElapsedTime().asSeconds() >= 1.0)
    {
        FPS = numberOfFrames;

        //std::cout << FPS << '\n';

        if (numberOfFrames == 0)
            FPS = 1;

        numberOfFrames = 0;
        clockFPS.restart();
    }

    windowMain.pollEvent(event);

    updateMouseButton();
    updateKeyboardButton();
    manageWindowEvent(event, windowMain, FPS);

    Widget::UpdateEvents(windowMain);

    updateWidgets();


        cameraRender.adaptToEvents(checkKeyboardButtonMaintained(sf::Keyboard::Up) || checkKeyboardButtonPushed(sf::Keyboard::Up) || checkKeyboardButtonMaintained(sf::Keyboard::Z) || checkKeyboardButtonPushed(sf::Keyboard::Z),
                                   checkKeyboardButtonMaintained(sf::Keyboard::Down) || checkKeyboardButtonPushed(sf::Keyboard::Down) || checkKeyboardButtonMaintained(sf::Keyboard::S) || checkKeyboardButtonPushed(sf::Keyboard::S),
                                   checkKeyboardButtonMaintained(sf::Keyboard::Left) || checkKeyboardButtonPushed(sf::Keyboard::Left) || checkKeyboardButtonMaintained(sf::Keyboard::Q) || checkKeyboardButtonPushed(sf::Keyboard::Q),
                                   checkKeyboardButtonMaintained(sf::Keyboard::Right) || checkKeyboardButtonPushed(sf::Keyboard::Right) || checkKeyboardButtonMaintained(sf::Keyboard::D) || checkKeyboardButtonPushed(sf::Keyboard::D),
                                   glm::vec2(sf::Mouse::getPosition().x, sf::Mouse::getPosition().y), !Widget::isAWidgetHovered(windowMain));

}

void System::Render()
{

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    windowMain.clear(sf::Color(255,0,255));

    renderOpenGL(windowMain);

    windowMain.pushGLStates();

        renderWidgets();

    windowMain.popGLStates();

    windowMain.display();

    numberOfFrames++;
}

void System::blockFPS(sf::Clock &fpsClock, int &currentFrameNumber, const int wishedFPS) const
{
    while (fpsClock.getElapsedTime().asSeconds() < static_cast<float>(currentFrameNumber)/static_cast<float>(wishedFPS));
}

sf::RenderWindow* System::returnRenderWindowPointer() const
{
    return &windowMain;
}

void System::updateMouseButton()
{
    for (int i(0);i<sf::Mouse::ButtonCount;i++)
    {
        if (sf::Mouse::isButtonPressed(i))//If the button is pressed...
        {
            if (mouseButtonClick[i] == false && mouseButtonsMaintained[i] == false)//We first for once set it on clicked...
            {
                mouseButtonClick[i] = true;
                ignoreMouseClick[i] = false;
            }
            else if (mouseButtonClick[i] == true && mouseButtonsMaintained[i] == false)//Then block it on maintained
            {
                mouseButtonClick[i] = false;
                mouseButtonsMaintained[i] = true;
                ignoreMouseMaintained[i] = false;
            }
        }
        else if (mouseButtonClick[i] || mouseButtonsMaintained[i])//Release the button
        {
            mouseButtonClick[i] = false;
            mouseButtonsMaintained[i] = false;
            ignoreMouseClick[i] = false;
            ignoreMouseMaintained[i] = false;
        }
    }
}

void System::writeDebugInfo(sf::RenderWindow& window, int FPS) const
{
    if (displayDebug)
    {
        sf::Text debug_text;
        debug_text.setCharacterSize(16);
        debug_text.setColor(sf::Color(255,0,0));
        debug_text.setFont(debug_font);
        debug_text.setPosition(0,0);

        std::stringstream myStringStream;

        myStringStream << "Welcome to the debug menu (press F12 to leave ;) )\n";

            myStringStream << "Current FPS count : " << FPS << '\n';

        myStringStream << "That's all I'm allowed to say...\n";

        debug_text.setString(myStringStream.str());
        window.draw(debug_text);
    }
}

bool System::isDebugDisplayed() const
{
    return displayDebug;
}

bool System::checkMouseButtonClicked(sf::Mouse::Button button) const
{
    if (ignoreMouseClick[button])
        return false;
    else
        return mouseButtonClick[button];
}

bool System::checkMouseButtonMaintained(sf::Mouse::Button button) const
{
    if (ignoreMouseMaintained[button])
        return false;
    else
        return mouseButtonsMaintained[button];
}

void System::nullifyClick(sf::Mouse::Button button)
{
    ignoreMouseClick[button] = true;
}

void System::nullifyClickMaintained(sf::Mouse::Button button)
{
    ignoreMouseMaintained[button] = true;
}

void System::updateKeyboardButton()
{
    for (int i(0); i<sf::Keyboard::KeyCount ; i++)
    {
        if (sf::Keyboard::isKeyPressed(i))//If the button is pressed...
        {
            if (keyboardButtonPushed[i] == false && keyboardButtonsMaintained[i] == false)//We first for once set it on clicked...
            {
                keyboardButtonPushed[i] = true;
            }
            else if (keyboardButtonPushed[i] == true && keyboardButtonsMaintained[i] == false)//Then block it on maintained
            {
                keyboardButtonPushed[i] = false;
                keyboardButtonsMaintained[i] = true;
            }
        }
        else if (keyboardButtonPushed[i] || keyboardButtonsMaintained[i])//Release the button
        {

            keyboardButtonPushed[i] = false;
            keyboardButtonsMaintained[i] = false;
        }
    }
}

void System::editMenu()
{

}

void System::console()
{
    if (displayConsole)
    {
        std::cout << "MEH";
    }
}

void System::manageWindowEvent(sf::Event event, sf::RenderWindow& window, int FPS)
{
    double speed((double)SCROLL_SPEED / FPS);

    if (checkKeyboardButtonPushed(sf::Keyboard::F12))
        displayDebug = !displayDebug;

    if (checkKeyboardButtonPushed(sf::Keyboard::T))
        displayConsole = !displayConsole;
}

bool System::checkKeyboardButtonMaintained(sf::Keyboard::Key key) const
{
    return keyboardButtonsMaintained[key];
}

bool System::checkKeyboardButtonPushed(sf::Keyboard::Key key) const
{
    return keyboardButtonPushed[key];
}

void System::nullifyPush(sf::Keyboard::Key key)
{
    keyboardButtonPushed[key] = false;
    if (sf::Keyboard::isKeyPressed(key))
        keyboardButtonsMaintained[key] = true;
    else
        keyboardButtonsMaintained[key] = false;
}

System::System() :  context(),
                    windowMain(sf::VideoMode(INITIAL_WINDOW_WIDTH, INITIAL_WINDOW_HEIGHT),
                               "Game", sf::Style::Default, sf::ContextSettings(24,8,4)),
                    primaryContainer(sf::Vector2f(0,0), sf::Vector2f(500, 400), "resources/window_background.png")

{
    glViewport(0, 0, windowMain.getSize().x, windowMain.getSize().y);
    windowMain.setVerticalSyncEnabled(true);
    windowMain.setActive(true);

    oglpart.Init(windowMain);

    glEnable(GL_DEPTH_TEST);

    for (int i(0) ; i < sf::Mouse::ButtonCount ; i++)
    {
        ignoreMouseClick[i] = false;
        ignoreMouseMaintained[i] = false;
    }

    debug_font.loadFromFile("arial.ttf");//Thanks to the author (REMIND : credit him)

    displayConsole = false;
    displayDebug = false;//Pretty self-explanatory : if true, debug infos are displayed

    consoleFontSize = 40;

    clockFPS.restart();
    numberOfFrames = 0;
    FPS = wishedFPS;

    //Initializing primary container (adding console loading widgets etc.)
    primaryContainer.setTitle("Main system primary container");

    this->addWidget(InputString(sf::Vector2f(0,windowMain.getSize().y - 15), sf::Vector2f(windowMain.getSize().x/2,15), sf::Color(255,255,255), sf::Vector2f(0,0), sf::Vector2f(1,1), 12, "CAKE"),
                         "Console");
    //primaryContainer.hideWidget("Console");

    for (int i(0) ; i<sf::Mouse::ButtonCount ; i++)//Initialize states of the mouse buttons
    {
        mouseButtonsMaintained[i] = false;
        mouseButtonClick[i] = false;
    }

    for (int i(0) ; i<sf::Keyboard::KeyCount ; i++)//And of the keyboard buttons
    {
        keyboardButtonPushed[i] = false;
        keyboardButtonsMaintained[i] = false;
    }


}

template<typename T>
Widget* System::addWidget(T toAdd, std::string name)
{
    return primaryContainer.addWidget(toAdd, name);
}

void System::removeWidget(std::string name)
{
    primaryContainer.removeWidget(name);
}

Widget* System::giveWidgetPointer(std::string widgetName) const
{
    return primaryContainer.giveWidgetPointer(widgetName);
}

void System::updateWidgets()
{
    updateWidgetDirection(primaryContainer, windowMain);
}

void System::renderWidgets()
{

    renderWidgetDirection(primaryContainer, windowMain);

}

void System::renderOpenGL(sf::RenderTarget& target)
{
    oglpart.render(target, cameraRender, windowMain);
}

System::~System(){};

