#ifndef CLASSES_H_INCLUDED
#define CLASSES_H_INCLUDED

#include "OpenGLRenderer.hpp"
#include "Camera.hpp"

#include <SFML/Graphics.hpp>
#include <queue>
#include <memory>

#include "GUI/Interface.hpp"
#include "Nodes.hpp"

#define ID_TYPE unsigned int

class System{
public:
    System();
    ~System();

    //sf::RenderWindow getRenderWindow() const;

    void updateMouseButton();
    void updateKeyboardButton();

    void manageWindowEvent(sf::Event event, sf::RenderWindow& window, int FPS);//Processes event in the window (zooms, moving...)
    void writeDebugInfo(sf::RenderWindow& window, int FPS) const;//Used for debug. Write info on the screen about the current system, window...

    bool checkKeyboardButtonMaintained(sf::Keyboard::Key key) const;
    bool checkKeyboardButtonPushed(sf::Keyboard::Key key) const;
    void nullifyPush(sf::Keyboard::Key key);//For example, when the key's push has already been used (less useful than click)

    bool checkMouseButtonMaintained(sf::Mouse::Button button) const;
    bool checkMouseButtonClicked(sf::Mouse::Button button) const;
    void nullifyClick(sf::Mouse::Button button);//For example, when the click has already been registered and used, or used on something forefront (interfaces...)
    void nullifyClickMaintained(sf::Mouse::Button button);

    bool isDebugDisplayed() const;

    sf::RenderWindow* returnRenderWindowPointer() const;

    void blockFPS(sf::Clock &fpsClock, int &currentFrameNumber, const int wishedFPS) const;
    void editMenu();

    void console();

    template<class T> Widget* addWidget(T toAdd, std::string name);
    void removeWidget(std::string name);
    Widget* giveWidgetPointer(std::string widgetName) const;//Matches the name and the ID, to then get pointer from ID

    void updateWidgets();
    void renderWidgets();

    void renderOpenGL(sf::RenderTarget& target);

    void Update();
    void Render();

private:

    std::map<sf::Vector2i, Node> allNodes;

    Container primaryContainer;

    Camera cameraRender;
    OpenGLRenderer oglpart;

    sf::Context context;

    sf::Font debug_font;

    sf::RenderWindow windowMain;

    short consoleFontSize;

    bool mouseButtonsMaintained[sf::Mouse::ButtonCount];
    bool ignoreMouseMaintained[sf::Mouse::ButtonCount];
    bool mouseButtonClick[sf::Mouse::ButtonCount];
    bool ignoreMouseClick[sf::Mouse::ButtonCount];

    bool keyboardButtonsMaintained[sf::Keyboard::KeyCount];
    bool keyboardButtonPushed[sf::Keyboard::KeyCount];

    bool displayConsole;
    std::string consoleString;

    bool displayDebug;

    sf::Event event;

    int FPS;
    int numberOfFrames;

    sf::Clock clockFPS;
    sf::Clock imGuiClock;
};

//A simple class to symbolize and perform operation on vectors
class Vector{
public:
    Vector() {x=0; y=0;};
    Vector(float xToGive, float yToGive) {x=xToGive; y=yToGive;};
    ~Vector() {};

    Vector normalize() const {double length(sqrt((x*x) + (y*y))); return Vector(x / length, y / length);};
    static Vector sumVector(Vector *data, int dataSize)
    {
        Vector dummy(0,0);

        for (int i(0) ; i < dataSize ; i++)
            dummy += *(data + i);

        return dummy;
    };

    friend Vector operator+(const Vector& A, const Vector& B) {return Vector(A.x + B.x, A.y + B.y);};
    friend void operator+=(Vector& A, const Vector& B) {A.x += B.x; A.y += B.y;};
    friend std::ostream& operator<<(std::ostream& out, const Vector A)
    {
        out << '(' << A.x << ';' << A.y << ')';
        return out;
    };

private:
    float x, y;
};

#endif // CLASSES_H_INCLUDED
