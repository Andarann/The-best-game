#ifndef INTERFACE_HPP_INCLUDED
#define INTERFACE_HPP_INCLUDED

#define ID_TYPE unsigned int
#define BOUNDARIES_THICKNESS 5//In pixels

#include <iostream>
#include <vector>
#include <string>
#include <memory>
#include <queue>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <typeinfo>
#include <windows.h>
#include <type_traits>

#include "tinyxml2.h"

/**
Hello and welcome to this tutorial on how to use the basics of this GUI system

The system works on a simple inheritance system from a class called Widget.
Widget is very bare-boned and not meant to be used directly.
However what you can do is use the current inherited classes coded by myself,
or you can code yours.
Coding your own class is easy, but follows some strict rules that cannot be avoided.
The main concern is that you inherited class (let's call it yourAwesomeWidget) looks like
this :

class yourAwesomeWidget : public Widget{
public:
    yourAwesomeWidget();//An empty constructor
    yourAwesomeWidget(whatever youWant);//Any other constructor that your heart desires
    ~yourAwesomeWidget();//A destructor


    virtual void update(sf::RenderWindow& window, System* systemContainer);
    virtual void draw(sf::RenderTarget& target) const;

private:
    //What you want
};

Regarding updating :
    The library required you to call a static function called Widget::

*/

/**IT IS TO BE NOTED THAT ALL POSITIONS ARE RELATIVE TO THE WIDGET'S CONTAINER. IT IS ALSO A RECURSIVE PROPERTY*/

//We will set a basic GUI system relying on SFML, based on flexibility (read : mod-ability), and easiness to use
//All the system will be based on a big inheritance system from this class (widget) to create the whole system

//Not to be used directly, only a basis to build stuff on
class Widget{//Ok
public:

    bool isHovered(sf::RenderWindow& window, sf::FloatRect* widgetShape, sf::Vector2f origin = sf::Vector2f(0,0));//
    bool isClicked() const;//
    bool wasUpdated() const;
    ID_TYPE getID() const;//

    Widget();
    virtual ~Widget();

    void setID(ID_TYPE newID);
    void setUpdatedState(bool toSet);

    bool updateWidgetState(sf::RenderWindow& window, sf::FloatRect* widgetShape, sf::Vector2f origin = sf::Vector2f(0,0));//Updates the hovered, clicked, clickMaintained flags

    static ID_TYPE giveTextureID(std::string texturePath);//Simply gives the pointer if there's one, otherwise loads then give. One file path = one texture, two same textures with different path will occupy two spaces
    static ID_TYPE giveFontID(std::string fontPath);//Same
    static void removeTexture(ID_TYPE textureID);
    static void removeFont(ID_TYPE fontID);
    static sf::Texture* giveTexturePointerOfID(ID_TYPE textureID);
    static sf::Font* giveFontPointerOfID(ID_TYPE fontID);

    static bool Init(sf::Window const& mainWindow);
    static void UpdateEvents(sf::Window const& mainWindow);
    static bool isKeyPressed(sf::Window const& mainWindow, sf::Keyboard::Key keyCheck);
    static bool isKeyPushed(sf::Window const& mainWindow, sf::Keyboard::Key keyCheck);
    static bool isKeyMaintained(sf::Window const& mainWindow, sf::Keyboard::Key keyCheck);
    static bool isButtonPressed(sf::Window const& mainWindow, sf::Mouse::Button buttonCheck);
    static bool isButtonClicked(sf::Window const& mainWindow, sf::Mouse::Button buttonCheck);
    static bool isButtonMaintained(sf::Window const& mainWindow, sf::Mouse::Button buttonCheck);
    static void ignoreButtonClick(sf::Window const& mainWindow, sf::Mouse::Button toIgnore);
    static void ignoreButtonMaintained(sf::Window const& mainWindow, sf::Mouse::Button toIgnore);

    static bool isWidgetContainer(Widget* toCheck);
    static bool isAWidgetHovered(sf::Window const& windowToCheck);

    virtual void loadFromXML(std::string filePath) = 0;
    virtual void saveToXML(std::string filePath) = 0;

    virtual void update(sf::RenderWindow& window, sf::Vector2f origin = sf::Vector2f(0,0)) = 0;
    virtual void draw(sf::RenderTarget& target) const = 0;

protected:
    //bool posAbsolute;//If true, the position of the widget is absolute and can be anything within the universe of the game. Otherwise, it is screen-bounded
    //bool dragable;//Defines whether you can change the widget's position by clicking on it or no
    //bool resizable;//Defines if you can resize the widget by dragging its boundaries
    bool clicked;//True only on the event of a click, then it goes to click maintained
    bool clickMaintained;
    bool hovered;
    bool updated;
    ID_TYPE ID;

    static std::vector<sf::Uint32> unicodeBuffer;
    static std::vector<sf::Keyboard::Key> keyBuffer;

private://Those two are essentially corresponding tables who will store all the necessary fonts/textures for the GUI
    static sf::Event events;

    static ID_TYPE currentID;
    static std::queue<ID_TYPE> vacantID;

    static ID_TYPE currentFontID;
    static std::map<std::string, ID_TYPE> stringToIDTableFont;//Every single file is given an ID
    static std::map<ID_TYPE, std::unique_ptr<sf::Font>> widgetFonts;//The name of the texture file + a pointer
    static std::map<ID_TYPE, unsigned int> numberOfFontUsers;
    static std::queue<ID_TYPE> vacantFontID;

    static ID_TYPE currentTextureID;
    static std::map<std::string, ID_TYPE> stringToIDTableTexture;
    static std::map<ID_TYPE, std::unique_ptr<sf::Texture>> widgetTextures;//Same
    static std::map<ID_TYPE, unsigned int> numberOfTextureUsers;
    static std::queue<ID_TYPE> vacantTextureID;

    //All the stuff for the own GUI event management
    static bool widgetAlreadyHovered;

    static bool mouseButtonsMaintained[sf::Mouse::ButtonCount];
    static bool ignoreMouseMaintained[sf::Mouse::ButtonCount];
    static bool mouseButtonClick[sf::Mouse::ButtonCount];
    static bool ignoreMouseClick[sf::Mouse::ButtonCount];

    static bool keyboardButtonsMaintained[sf::Keyboard::KeyCount];
    static bool keyboardButtonPushed[sf::Keyboard::KeyCount];
};

//Basically just a graphical widget, that will be used by other ones to display their content
//It can be anything, a colored rectangle, a 2d texture, text etc.
class Dummy : public Widget{//Ok
public:
    /**
    Creates a Dummy Widget from an image file.
    This constructor also adds the loaded texture to
    the widgetTextures static container, in Widget

    @param Essentially the file's path, joined with
    the desired size of the dummy

    */
    Dummy() {};
    Dummy(std::string imagePath, sf::Vector2f size, sf::Vector2f pos = sf::Vector2f(0,0));//Load a texture from a file
    Dummy(sf::Color const& fillColor, sf::Vector2f size, sf::Vector2f pos = sf::Vector2f(0,0));//Create a colored rectangle. Probably very slow
    ~Dummy() {};

    void setTexture(std::string imagePath);
    void setPosition(sf::Vector2f newPos);
    void setFillColor(sf::Color newColor);
    void setSize(sf::Vector2f newSize);

    sf::Vector2f getSize() const;
    sf::Vector2f getPosition() const;
    sf::Color getFillColor() const;

    sf::FloatRect getLocalBounds() const;
    sf::FloatRect getGlobalBounds() const;

    virtual void loadFromXML(std::string filePath) {};
    virtual void saveToXML(std::string filePath) {};

    virtual void update(sf::RenderWindow& window, sf::Vector2f origin = sf::Vector2f(0,0));
    virtual void draw(sf::RenderTarget& target) const;


private:
    double posX, posY;
    double sizeX, sizeY;
    ID_TYPE textureID;
    sf::Sprite sprite_;
};

//A button is a (colored/textured) rectangle with text centered in it. It reacts when clicked on
class Button : public Widget{//Ok
public:
    Button();
    Button(sf::Vector2f pos, sf::Vector2i boundariesSize, sf::Color buttonColoration, std::string buttonContent = "", short fontSize = 0, sf::Vector2f textPosPercent = sf::Vector2f(0.5, 0.5));//Create a button with a colored rectangle as background
    Button(sf::Vector2f pos, sf::Vector2i boundariesSize, sf::Color buttonColoration, std::string texturePath, std::string buttonContent = "", short fontSize = 0, sf::Vector2f textPosPercent = sf::Vector2f(0.5, 0.5));
    ~Button();

    void setString(std::string stringToPut);
    void setFillColor(sf::Color newColor);
    void setHoveredColor(sf::Color newColor);
    void setClickedColor(sf::Color newColor);

    void setTexture(std::string texturePath);
    void setDummy(Dummy& toPut);
    void setSize(sf::Vector2f size_);
    void setPosition(sf::Vector2f pos);

    sf::Vector2f getSize() const;
    sf::Vector2f getPosition() const;

    sf::Color getColor() const;
    sf::Color getHoveredColor() const;
    sf::Color getClickedColor() const;
    std::string getButtonString() const;

    bool isPressed() const {return (clicked || clickMaintained);};

    virtual void loadFromXML(std::string filePath) {};
    virtual void saveToXML(std::string filePath) {};

    virtual void update(sf::RenderWindow& window, sf::Vector2f origin = sf::Vector2f(0,0));
    virtual void draw(sf::RenderTarget& target) const;

    void operator=(Dummy& a);
private:
    sf::Vector2f textPos;//(0.5;0.5) would indicate middle, (1;0) top left etc.
    sf::Text buttonString;
    sf::Color buttonColor;
    sf::Color hoveredColor;
    sf::Color clickedColor;
    //sf::RectangleShape buttonShape;//To be replaced by a sf::Sprite, or sf::Texture, or sf::Whatever
    ID_TYPE fontID;
    Dummy buttonShape;
};

//Checkbox is a box that is whether true, or false. A click changes this state
class Checkbox : public Widget{//Ok
public:
    Checkbox();
    Checkbox(bool init, sf::Vector2f widSize, sf::Vector2f pos);
    ~Checkbox();

    bool getValue() const;

    virtual void update(sf::RenderWindow& window, sf::Vector2f origin = sf::Vector2f(0,0));
    virtual void draw(sf::RenderTarget& target) const;

private:
    bool value;
    Dummy checkBoxUnchecked;
    Dummy checkBoxChecked;
};

//Two rectangles, one smaller sliding in a bigger one
//It can also have two buttons on the extremities, with two arrows in it
class HorizontalSlider : public Widget{
public:
    HorizontalSlider();
    HorizontalSlider(sf::Vector2f pos, sf::Vector2f size_);
    HorizontalSlider(sf::Vector2f pos, sf::Vector2f size_, double sliderRelativeSize);
    ~HorizontalSlider();

    void setSliderTexture(Dummy dummySlider);
    void setSliderTexture(std::string sliderTexturePath);
    void setSliderTexture(sf::Color sliderColor);

    void setBackgroundTexture(Dummy dummyBackground);
    void setBackgroundTexture(std::string backgroundTexturePath);
    void setBackgroundTexture(sf::Color backgroundColor);

    float getCurrentPosPercent() const;

    void setPosition(sf::Vector2f newPos);
    void setSize(sf::Vector2f newSize);
    void setSliderRelativeLength(double newRelLen);

    sf::Vector2f getPosition() const;
    sf::Vector2f getSize() const;
    double getSliderRelativeLength() const;

    virtual void loadFromXML(std::string filePath) {};
    virtual void saveToXML(std::string filePath) {};

    virtual void update(sf::RenderWindow& window, sf::Vector2f origin = sf::Vector2f(0,0));
    virtual void draw(sf::RenderTarget& target) const;

private:
    float clickOrigin;
    float currentPos;

    bool leftClickMaintained;

    Dummy graphicalSlider;
    Dummy sliderBackground;
};

//A big rectangle containing other widgets (can be recursive, e.g a container containing a container etc.)
//If needed, it will contain one or two sliders, depending on whether the container is too small or not
class Container : public Widget{//Ok
public:
    Container();
    Container(sf::Vector2f pos, sf::Vector2f size_, sf::Color backgroundColor = sf::Color(0,0,0,0));
    Container(sf::Vector2f pos, sf::Vector2f size_, std::string texturePath);
    ~Container();

    void setTitle(std::string newTitle);

    template<class T>
    T* addWidget(T toAdd, std::string name)
    {
        auto findWidget = allWidgets.find(name);

        if (findWidget == allWidgets.end())
        {
            WidgetAndAttribs temp;

            temp.widget = std::shared_ptr<Widget>(new T(toAdd));
            temp.hideState = false;

            allWidgets.insert( std::make_pair(name, temp));
        }

        return giveWidgetPointer(name);
    };

    void removeWidget(std::string name);
    Widget* giveWidgetPointer(std::string widgetName) const;//Matches the name and the ID, to then get pointer from ID
    Widget* returnWidgetPointerOfID(ID_TYPE ID) const;//Not meant to be used directly. Matches ID and Widget*

    void hideWidget(std::string widgetName);
    void showWidget(std::string widgetName);
    bool isWidgetHidden(std::string widgetName) const;

    void updateTexture(sf::RenderTexture& texture);

    virtual void loadFromXML(std::string filePath) {};
    virtual void saveToXML(std::string filePath) {};

    virtual void update(sf::RenderWindow& window, sf::Vector2f origin = sf::Vector2f(0,0));
    virtual void draw(sf::RenderTarget& target) const;

private:
    struct WidgetAndAttribs{
        std::shared_ptr<Widget> widget;
        bool hideState;
    };

    static bool memberInteracted;

    Dummy background;

    sf::Sprite renderSprite;
    std::shared_ptr<sf::RenderTexture> myRenderTexture;

    sf::Vector2f size;
    bool forceRender;

    std::map<std::string, WidgetAndAttribs> allWidgets;
};

//A class in which you can input any value you want : int, double, float, std::string, any object.
//The required operators, however, are the >, <, =, <=, >= and == operators, for comparison matter.
//The << operator is also needed, otherwise the text will not be able to be displayed
template<class T>
class InputValue : public Widget{
public:
    //The default constructor of T will be used, no parameter sent
    InputValue() : background()
    {
        isNumber();

        textVerticalOffset = 0;
        textHorizontalOffset = 0;

        limits = false;
        isSigned = isVariableSigned();
        selected = false;

        textRelPos = sf::Vector2f(0,0);
        textBoxSize = sf::Vector2f(1,1);

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(1);

        changeValue(T());

        cursorPos = valueContained.size();

        updateSprite();
    };

    InputValue(T initValue) : background()
    {
        isNumber();

        textVerticalOffset = 0;
        textHorizontalOffset = 0;

        limits = false;
        isSigned = isVariableSigned();
        selected = false;

        textRelPos = sf::Vector2f(0,0);
        textBoxSize = sf::Vector2f(1,1);

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(1);

        changeValue(initValue);

        cursorPos = valueContained.size();

        updateSprite();
    };

    InputValue(T initValue, T minValue_, T maxValue_) : background()
    {
        isNumber();

        textVerticalOffset = 0;
        textHorizontalOffset = 0;

        limits = true;
        isSigned = isVariableSigned();
        selected = false;

        minValue = minValue_;
        maxValue = maxValue_;

        textRelPos = sf::Vector2f(0,0);
        textBoxSize = sf::Vector2f(1,1);

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(1);

        changeValue(initValue);

        cursorPos = valueContained.size();

        updateSprite();
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, sf::Color color, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize)//The default constructor of T will be used, no parameter sent
        : background(color, size, pos)
    {
        isNumber();

        textVerticalOffset = 0;
        textHorizontalOffset = 0;

        limits = false;
        isSigned = isVariableSigned();
        selected = false;

        textRelPos = sf::Vector2f(pos.x * relativeTextPosPer_.x, pos.y * relativeTextPosPer_.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        changeValue(T());

        cursorPos = valueContained.size();

        updateSprite();
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, sf::Color color, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize, T initValue)
            : background(color, size, pos)
    {
        isNumber();

        textVerticalOffset = 0;
        textHorizontalOffset = 0;

        limits = false;
        isSigned = isVariableSigned();
        selected = false;

        textRelPos = sf::Vector2f(pos.x * relativeTextPosPer_.x, pos.y * relativeTextPosPer_.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        changeValue(initValue);

        cursorPos = valueContained.size();

        updateSprite();
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, sf::Color color, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize, T initValue, T minValue_, T maxValue_)
            : background(color, size, pos)
    {
        isNumber();

        textVerticalOffset = 0;
        textHorizontalOffset = 0;

        limits = true;
        isSigned = isVariableSigned();
        selected = false;

        minValue = minValue_;
        maxValue = maxValue_;

        textRelPos = sf::Vector2f(pos.x * relativeTextPosPer_.x, pos.y * relativeTextPosPer_.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        changeValue(initValue);

        cursorPos = valueContained.size();

        updateSprite();
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, std::string texturePath, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize)//The default constructor of T will be used, no parameter sent
            : background(texturePath, size, pos)
    {
        isNumber();

        textVerticalOffset = 0;
        textHorizontalOffset = 0;

        limits = false;
        isSigned = isVariableSigned();
        selected = false;

        textRelPos = sf::Vector2f(pos.x * relativeTextPosPer_.x, pos.y * relativeTextPosPer_.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        changeValue(T());

        cursorPos = valueContained.size();

        updateSprite();
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, std::string texturePath, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize, T initValue)
            : background(texturePath, size, pos)
    {
        isNumber();

        textVerticalOffset = 0;
        textHorizontalOffset = 0;

        limits = false;
        isSigned = isVariableSigned();
        selected = false;

        textRelPos = sf::Vector2f(pos.x * relativeTextPosPer_.x, pos.y * relativeTextPosPer_.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        changeValue(initValue);

        cursorPos = valueContained.size();

        updateSprite();
    };

    InputValue(sf::Vector2f pos, sf::Vector2f size, std::string texturePath, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize, T initValue, T minValue_, T maxValue_)
            : background(texturePath, size, pos)
    {
        isNumber();

        textVerticalOffset = 0;
        textHorizontalOffset = 0;

        limits = true;
        isSigned = isVariableSigned();
        selected = false;

        minValue = minValue_;
        maxValue = maxValue_;

        textRelPos = sf::Vector2f(pos.x * relativeTextPosPer_.x, pos.y * relativeTextPosPer_.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(valueContained);
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        changeValue(initValue);

        cursorPos = valueContained.size();

        updateSprite();
    };

    ~InputValue() {};

    T returnValue() const
    {
        if (valueContained.size())
        {
            T toReturn;
            std::stringstream valueStr;

            valueStr << valueContained;
            valueStr >> toReturn;

            return toReturn;
        }
        else
        {
            return T();
        }
    };

    std::string returnString() const
    {
        return valueContained;
    };

    bool loadFont(std::string fontToLoad)
    {
        fontID = Widget::giveFontID(fontToLoad);
        contentText.setFont(*Widget::giveFontPointerOfID(fontID));
        return true;
    };

    void changeValue(T toPut)
    {
        valueContained.clear();

        std::stringstream newValue;
        newValue << toPut;
        valueContained = newValue.str();

        updateSprite();
    };

    bool isNumber() const
    {
        std::stringstream myStream;
        T testType = T();
        T before(testType);
        myStream << "a";
        myStream >> testType;

        if (testType != before)
        {
            std::stringstream errorStream;
            errorStream << "Error : InputValue not meant to be used with variable of type "
                        << typeid(T).name()
                        << ". You want to use the InputString class for any non-numeric input.\n";
            std::cerr << errorStream.str() << '\n';
            throw errorStream.str().c_str();
        }
        return (testType == before);
    }

    bool isVariableSigned() const
    {
        return (T(-1) < 0);
    }

    void updateSprite()
    {
        double x(0.0);
        double y(0.0);

        sf::RenderTexture tempText;
        sf::Vector2f tempPos(contentText.getPosition());

        tempText.create(textBoxSize.x, textBoxSize.y);
        contentText.setString(returnString());

        std::size_t cursorAbsolutePos(0);

        cursorAbsolutePos += cursorPos;

        adaptHorizontalBounds(cursorAbsolutePos);
        x = textHorizontalOffset;

        adaptVerticalBounds(cursorAbsolutePos);
        y = textVerticalOffset;

        this->contentText.setPosition(x,y);

        tempText.draw(contentText);
        tempText.display();

        myTexture.reset();
        myTexture = std::make_shared<sf::Texture>(tempText.getTexture());

        textToDisplay.setTexture(*myTexture);
        textToDisplay.setPosition(background.getPosition().x + textRelPos.x, background.getPosition().y + textRelPos.y);

        cursorMarker.setFillColor(contentText.getFillColor());
        cursorMarker.setSize(sf::Vector2f(1,contentText.getFont()->getLineSpacing(contentText.getCharacterSize())));
        cursorMarker.setPosition(sf::Vector2f(tempPos.x + textHorizontalOffset + contentText.findCharacterPos(cursorAbsolutePos).x - contentText.findCharacterPos(0).x
                                              ,tempPos.y + textVerticalOffset + contentText.findCharacterPos(cursorAbsolutePos).y - contentText.findCharacterPos(0).y
                                              ));

        this->contentText.setPosition(tempPos);
    }

    void adaptHorizontalBounds(std::size_t cursorAbsolutePos)
    {
        if (textHorizontalOffset + contentText.findCharacterPos(cursorAbsolutePos).x - contentText.findCharacterPos(0).x < 0)
        {
            while (textHorizontalOffset + contentText.findCharacterPos(cursorAbsolutePos).x - contentText.findCharacterPos(0).x < 0)
                textHorizontalOffset++;

            if (textHorizontalOffset > 0)
                textHorizontalOffset = 0;
        }
        else if (textHorizontalOffset + contentText.findCharacterPos(cursorAbsolutePos).x - contentText.findCharacterPos(0).x > textBoxSize.x)
        {
            if (-contentText.getCharacterSize()*2 + textHorizontalOffset - contentText.findCharacterPos(0).x < -contentText.findCharacterPos(valueContained.size()).x)
                textHorizontalOffset = -contentText.findCharacterPos(valueContained.size()).x;
            else
                while ((textHorizontalOffset + contentText.findCharacterPos(cursorAbsolutePos).x - contentText.findCharacterPos(0).x > textBoxSize.x))
                    textHorizontalOffset--;
        }
    }

    void adaptVerticalBounds(std::size_t cursorAbsolutePos)
    {
        if (textVerticalOffset + contentText.findCharacterPos(cursorAbsolutePos).y - contentText.findCharacterPos(0).y < 0)
        {
            while (textVerticalOffset + contentText.findCharacterPos(cursorAbsolutePos).y - contentText.findCharacterPos(0).y < 0)
                textVerticalOffset ++;

            if (textVerticalOffset > 0)
                textVerticalOffset = 0;
        }
        else if (textVerticalOffset + contentText.findCharacterPos(cursorAbsolutePos).y - contentText.findCharacterPos(0).y + contentText.getFont()->getLineSpacing(contentText.getCharacterSize()) > textBoxSize.y)
        {
            while ((textVerticalOffset + contentText.findCharacterPos(cursorAbsolutePos).y - contentText.findCharacterPos(0).y + contentText.getFont()->getLineSpacing(contentText.getCharacterSize()) > textBoxSize.y))
                textVerticalOffset--;
        }
    }

    void putValueBetweenLimits()
    {
        T currentValue = returnValue();

        if (currentValue != T())
        {
            if (!limits)
            {
                if (currentValue > std::numeric_limits<T>::max() - 1)
                {
                    changeValue(std::numeric_limits<T>::max());
                }
                else if (currentValue < std::numeric_limits<T>::min() + 1)
                {
                    changeValue(std::numeric_limits<T>::min());
                }
            }
            else
            {

                if (currentValue > maxValue  - 1)
                {
                    changeValue(maxValue);
                }
                else if (currentValue < minValue + 1)
                {
                    changeValue(minValue);
                }
            }
        }
    }

    virtual void loadFromXML(std::string filePath) {};
    virtual void saveToXML(std::string filePath) {};

    virtual void update(sf::RenderWindow& window, sf::Vector2f origin = sf::Vector2f(0,0))
    {
        updated = false;

        updateWidgetState(window, &background.getGlobalBounds(), origin);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            if (hovered)
            {
                if (!selected)
                    updated = true;

                selected = true;
            }
            else
            {
                if (selected)
                    updated = true;

                selected = false;
            }
        }

        if (selected)
        {
            std::size_t cursorPosBefore(cursorPos);
            std::size_t currentLineBefore(currentLine);
            std::string beforeModifText(valueContained);

            if (Widget::unicodeBuffer.size())
            {

                for (int i(0) ; i < Widget::unicodeBuffer.size() ; i++)
                {
                    switch (Widget::unicodeBuffer[i])
                    {
                    case 8://Delete

                        if (cursorPos > 0)
                        {
                            valueContained.erase(valueContained.begin() + cursorPos - 1);
                        }

                        break;


                    /*case 13://Enter

                        if (!aNumber)
                            valueContained += '\n';

                        break;*/

                    case 45://Minus

                        if (isSigned && cursorPos == 0 && minValue < 0)
                        {
                            minValue;
                            if (valueContained.find((char)45) == std::string::npos)
                                valueContained.insert(valueContained.begin() + cursorPos, (char)Widget::unicodeBuffer[i]);
                        }

                        break;

                    case 46://Comma


                        if (valueContained.find((char)46) == std::string::npos)//To be sure that there is at most one comma
                            valueContained.insert(valueContained.begin() + cursorPos, (char)Widget::unicodeBuffer[i]);



                        break;

                    case 48:
                    case 49:
                    case 50:
                    case 51:
                    case 52:
                    case 53:
                    case 54:
                    case 55:
                    case 56:
                    case 57://Numbers
                        valueContained.insert(valueContained.begin() + cursorPos, (char)Widget::unicodeBuffer[i]);
                        break;
                    }
                }
            }

            if (Widget::keyBuffer.size())
            {
                for (int i(0) ; i < Widget::keyBuffer.size() ; i++)
                {
                    switch (Widget::keyBuffer[i])
                    {
                        case 71://Left
                            if (cursorPos)
                                cursorPos--;
                            break;
                        case 72://Right
                            if (cursorPos < valueContained.size())
                                cursorPos++;
                            break;
                        case 73://Up

                            if (currentLine > 0)
                            {
                                if (valueContained.size() < cursorPos)
                                {
                                    cursorPos = valueContained.size();
                                }
                                //else, value unchanged
                                currentLine--;
                            }

                            break;
                        case 74://Down

                            if (currentLine < valueContained.size() - 1)
                            {
                                if (valueContained.size() < cursorPos)
                                {
                                    cursorPos = valueContained.size();
                                }
                                //else, value unchanged
                                currentLine++;
                            }

                            break;
                    }
                }
            }


            if (valueContained != beforeModifText || cursorPosBefore != cursorPos || currentLineBefore != currentLine)//Update the sprite
            {

                std::string valueBefore(valueContained);

                putValueBetweenLimits();

                if (valueBefore != valueContained && valueContained != beforeModifText)
                    valueContained = beforeModifText;

                cursorPos += valueContained.size() - beforeModifText.size();

                updateSprite();

                updated = true;
            }
        }
    };

    virtual void draw(sf::RenderTarget& target) const
    {
        background.draw(target);
        target.draw(textToDisplay);

        if (selected)
            target.draw(cursorMarker);
    };

private:
    std::size_t cursorPos;
    std::size_t currentLine;
    bool selected;

    bool limits;
    bool isSigned;
    bool isDecimal;

    T minValue, maxValue;
    T currentValue;

    std::string valueContained;

    ID_TYPE fontID;
    sf::Text contentText;
    sf::Vector2f textRelPos;
    sf::Vector2f textBoxSize;
    sf::Sprite textToDisplay;
    sf::RectangleShape cursorMarker;

    std::shared_ptr<sf::Texture> myTexture;

    Dummy background;

    int textHorizontalOffset;//number of pixels by which the text is pasted to the right
    int textVerticalOffset;//same but on y-axis
};

typedef InputValue<signed int> InputSignedInt;
typedef InputValue<int> InputInt;
typedef InputValue<unsigned int> InputUnsignedInt;
typedef InputValue<signed short> InputSignedShort;
typedef InputValue<short> InputShort;
typedef InputValue<unsigned short> InputUnsignedShort;
typedef InputValue<signed long> InputSignedLong;
typedef InputValue<long> InputLong;
typedef InputValue<unsigned long> InputUnsignedLong;
typedef InputValue<signed long long> InputSignedLongLong;
typedef InputValue<long long> InputLongLong;
typedef InputValue<unsigned long long> InputUnsignedLongLong;

//Same as InputValue, except most of the functions have been changed to host a string.
//One can input text with any legal Unicode character, and get it as a basic_string,
//whether it be a 8-UTF, 16-UTF or 32-UTF (std::string, std::
class InputString : public Widget{
public:
    //The default constructor of T will be used, no parameter sent
    InputString() : background()
    {
        textHorizontalOffset = 0;
        textVerticalOffset = 0;
        selected = false;

        changeValue(std::string());

        textRelPos = sf::Vector2f(0,0);
        textBoxSize = sf::Vector2f(1,1);

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(returnValue());
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(1);

        updateSprite();
    };

    InputString(sf::String initValue) : background()
    {
        textHorizontalOffset = 0;
        textVerticalOffset = 0;
        selected = false;

        changeValue(initValue);

        textRelPos = sf::Vector2f(0,0);
        textBoxSize = sf::Vector2f(1,1);

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(returnValue());
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(1);

        updateSprite();
    };

    InputString(sf::Vector2f pos, sf::Vector2f size, sf::Color color, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize)//The default constructor of T will be used, no parameter sent
        : background(color, size, pos)
    {
        textVerticalOffset = 0;
        textHorizontalOffset = 0;
        selected = false;

        changeValue(std::string());

        textRelPos = sf::Vector2f(relativeTextPosPer_.x * size.x, relativeTextPosPer_.y * size.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(returnValue());
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        updateSprite();
    };

    InputString(sf::Vector2f pos, sf::Vector2f size, sf::Color color, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize, sf::String initValue)
            : background(color, size, pos)
    {
        textVerticalOffset = 0;
        textHorizontalOffset = 0;
        selected = false;

        changeValue(initValue);

        textRelPos = sf::Vector2f(relativeTextPosPer_.x * size.x, relativeTextPosPer_.y * size.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(returnValue());
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        updateSprite();
    };

    InputString(sf::Vector2f pos, sf::Vector2f size, std::string texturePath, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize)//The default constructor of T will be used, no parameter sent
            : background(texturePath, size, pos)
    {
        textVerticalOffset = 0;
        textHorizontalOffset = 0;
        selected = false;

        changeValue(std::string());

        textRelPos = sf::Vector2f(relativeTextPosPer_.x * size.x, relativeTextPosPer_.y * size.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(returnValue());
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        updateSprite();
    };

    InputString(sf::Vector2f pos, sf::Vector2f size, std::string texturePath, sf::Vector2f relativeTextPosPer_, sf::Vector2f textBoxSizePer_, unsigned int characterSize, sf::String initValue)
            : background(texturePath, size, pos)
    {
        textVerticalOffset = 0;
        textHorizontalOffset = 0;
        selected = false;

        changeValue(initValue);

        textRelPos = sf::Vector2f(relativeTextPosPer_.x * size.x, relativeTextPosPer_.y * size.y);
        textBoxSize = sf::Vector2f(size.x * textBoxSizePer_.x, size.y * textBoxSizePer_.y);

        if (textRelPos.x + textBoxSize.x > size.x)
            textBoxSize.x = size.x - textRelPos.x;

        if (textRelPos.y + textBoxSize.y > size.y)
            textBoxSize.y = size.y - textRelPos.y;

        loadFont("arial.ttf");
        contentText.setFillColor(sf::Color(0,0,0));
        contentText.setString(returnValue());
        contentText.setPosition(textRelPos.x + background.getPosition().x, textRelPos.y + background.getPosition().y);
        contentText.setCharacterSize(characterSize);

        updateSprite();
    };


    ~InputString() {};

    sf::String returnValue() const
    {
        sf::String toReturn;

        for (int i(0) ; i < valueContained.size() ; i++)
        {
            toReturn += valueContained[i];
            toReturn += '\n';
        }

        return toReturn;
    };

    std::string returnValueAsUtf8() const
    {
        sf::String toReturn;

        for (int i(0) ; i < valueContained.size() ; i++)
        {
            toReturn += valueContained[i];
            toReturn += '\n';
        }

        return toReturn.toAnsiString();
    };

    std::basic_string<short unsigned int> returnValueAsUtf16() const
    {
        sf::String toReturn;

        for (int i(0) ; i < valueContained.size() ; i++)
        {
            toReturn += valueContained[i];
            toReturn += '\n';
        }

        return toReturn.toUtf16();
    };

    sf::String returnValueAsUtf32() const
    {
        sf::String toReturn;

        for (int i(0) ; i < valueContained.size() ; i++)
        {
            toReturn += valueContained[i];
            toReturn += '\n';
        }

        return toReturn.toUtf32();
    };

    bool loadFont(std::string fontToLoad)
    {
        fontID = Widget::giveFontID(fontToLoad);
        contentText.setFont(*Widget::giveFontPointerOfID(fontID));
    };

    void changeValue(sf::String toPut)
    {
        valueContained.clear();

        if (toPut.find('\n') == sf::String::InvalidPos)
        {
            valueContained.push_back(toPut);
        }
        else
        {
            std::size_t pos(0);
            std::size_t nextJump(0);

            do
            {
                nextJump = toPut.find(pos + 1);
                valueContained.push_back(toPut.substring(pos, nextJump));
                pos = nextJump;
            } while (pos != sf::String::InvalidPos);
        }

        currentLine = valueContained.size() - 1;

        if (valueContained.size() >= 1)
            cursorPos = valueContained[currentLine].getSize();
    };

    void updateSprite()
    {
        double x(0.0);
        double y(0.0);

        sf::RenderTexture tempText;
        sf::Vector2f tempPos(contentText.getPosition());

        tempText.create(textBoxSize.x, textBoxSize.y);
        contentText.setString(returnValue());

        std::size_t cursorAbsolutePos(0);

        for (int i(0) ; i < currentLine ; i++)
            cursorAbsolutePos += valueContained[i].getSize() + 1;

        cursorAbsolutePos += cursorPos;

        adaptHorizontalBounds(cursorAbsolutePos);
        x = textHorizontalOffset;

        adaptVerticalBounds(cursorAbsolutePos);
        y = textVerticalOffset;

        this->contentText.setPosition(x,y);

        tempText.draw(contentText);
        tempText.display();

        myTexture.reset();
        myTexture = std::make_shared<sf::Texture>(tempText.getTexture());

        textToDisplay.setTexture(*myTexture);
        textToDisplay.setPosition(background.getPosition().x + textRelPos.x, background.getPosition().y + textRelPos.y);

        cursorMarker.setFillColor(contentText.getFillColor());
        cursorMarker.setSize(sf::Vector2f(1,contentText.getFont()->getLineSpacing(contentText.getCharacterSize())));
        cursorMarker.setPosition(sf::Vector2f(tempPos.x + textHorizontalOffset + contentText.findCharacterPos(cursorAbsolutePos).x - contentText.findCharacterPos(0).x
                                              ,tempPos.y + textVerticalOffset + contentText.findCharacterPos(cursorAbsolutePos).y - contentText.findCharacterPos(0).y
                                              ));

        this->contentText.setPosition(tempPos);
    }

    void adaptHorizontalBounds(std::size_t cursorAbsolutePos)
    {
        if (textHorizontalOffset + contentText.findCharacterPos(cursorAbsolutePos).x - contentText.findCharacterPos(0).x < 0)
        {
            while (textHorizontalOffset + contentText.findCharacterPos(cursorAbsolutePos).x - contentText.findCharacterPos(0).x < 0)
                textHorizontalOffset++;

            if (textHorizontalOffset > 0)
                textHorizontalOffset = 0;
        }
        else if (textHorizontalOffset + contentText.findCharacterPos(cursorAbsolutePos).x - contentText.findCharacterPos(0).x > textBoxSize.x)
        {
                while ((textHorizontalOffset + contentText.findCharacterPos(cursorAbsolutePos).x - contentText.findCharacterPos(0).x > textBoxSize.x))
                    textHorizontalOffset--;
        }
    }

    void adaptVerticalBounds(std::size_t cursorAbsolutePos)
    {
        if (textVerticalOffset + contentText.findCharacterPos(cursorAbsolutePos).y - contentText.findCharacterPos(0).y < 0)
        {
            while (textVerticalOffset + (contentText.findCharacterPos(cursorAbsolutePos).y - contentText.findCharacterPos(0).y) * contentText.getFont()->getLineSpacing(contentText.getCharacterSize()) < 0)
                textVerticalOffset++;

            if (textVerticalOffset > 0)
                textVerticalOffset = 0;
        }
        else if (textVerticalOffset + (contentText.findCharacterPos(cursorAbsolutePos).y - contentText.findCharacterPos(0).y) * contentText.getFont()->getLineSpacing(contentText.getCharacterSize()) > textBoxSize.y)
        {
            while ((textVerticalOffset + contentText.findCharacterPos(cursorAbsolutePos - cursorPos).y - contentText.findCharacterPos(0).y + contentText.getFont()->getLineSpacing(contentText.getCharacterSize()) > textBoxSize.y))
                textVerticalOffset--;
        }

    }

    virtual void loadFromXML(std::string filePath) {};
    virtual void saveToXML(std::string filePath) {};

    virtual void update(sf::RenderWindow& window, sf::Vector2f origin = sf::Vector2f(0,0))
    {
        updated = false;

        updateWidgetState(window, &background.getGlobalBounds(), origin);

        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) || sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            if (hovered)
            {
                if (!selected)
                    updated = true;

                selected = true;
            }
            else
            {
                if (selected)
                    updated = true;

                selected = false;
            }
        }

        if (selected)
        {
            std::size_t cursorPosBefore(cursorPos);
            std::size_t currentLineBefore(currentLine);

            std::size_t lettersCountBefore(0);

            for (int i(0) ; i < valueContained.size() ; i++)
                lettersCountBefore += valueContained[i].getSize();

            if (Widget::unicodeBuffer.size())
            {
                for (int i(0) ; i < Widget::unicodeBuffer.size() ; i++)
                {
                    switch (Widget::unicodeBuffer[i])
                    {
                    case 8://Delete

                        if (cursorPos > 0)
                        {
                            valueContained[currentLine].erase(/**valueContained[currentLine].begin() + */cursorPos - 1);
                            cursorPos--;
                        }
                        else//cursorPos == 0
                        {
                            if (currentLine != 0)
                            {
                                cursorPos = valueContained[currentLine - 1].getSize();//We put the cursor at the beginning of the upper line
                                valueContained[currentLine - 1] += valueContained[currentLine];//We add the older line to the upper
                                valueContained.erase(valueContained.begin() + currentLine);//We erase the previous line
                                currentLine--;//And we say that we're on a new line
                            }
                        }


                        break;

                    case 13://Enter

                        valueContained.insert(valueContained.begin()+currentLine+1, valueContained[currentLine].substring(cursorPos));
                        valueContained[currentLine] = valueContained[currentLine].substring(0,cursorPos);
                        cursorPos = 0;
                        currentLine++;

                        break;

                    default:

                        valueContained[currentLine].insert(cursorPos, Widget::unicodeBuffer[i]);
                        cursorPos++;
                        break;
                    }
                }
            }

            if (Widget::keyBuffer.size())
            {
                for (int i(0) ; i < Widget::keyBuffer.size() ; i++)
                {
                    switch (Widget::keyBuffer[i])
                    {
                    case 71://Left
                        if (cursorPos)
                            cursorPos--;
                        break;
                    case 72://Right
                        if (cursorPos < valueContained[currentLine].getSize())
                            cursorPos++;
                        break;
                    case 73://Up

                        if (currentLine > 0)
                        {
                            if (valueContained[currentLine - 1].getSize() < cursorPos)
                            {
                                cursorPos = valueContained[currentLine - 1].getSize();
                            }
                            //else, value unchanged
                            currentLine--;
                        }

                        break;
                    case 74://Down

                        if (currentLine < valueContained.size() - 1)
                        {
                            if (valueContained[currentLine + 1].getSize() < cursorPos)
                            {
                                cursorPos = valueContained[currentLine + 1].getSize();
                            }
                            //else, value unchanged
                            currentLine++;
                        }

                        break;
                    default:
                        break;
                    }
                }
            }

            std::size_t lettersCountAfter(0);

            for (int i(0) ; i < valueContained.size() ; i++)
                lettersCountAfter += valueContained[i].getSize();

            if (lettersCountBefore != lettersCountAfter || cursorPosBefore != cursorPos || currentLineBefore != currentLine)//Update the sprite
            {
  //              if (currentLineBefore == currentLine)
//                    currentLineBounds.y += lettersCountAfter - lettersCountBefore;

                updateSprite();
                updated = true;

            }
        }
    };

    virtual void draw(sf::RenderTarget& target) const
    {
        background.draw(target);
        target.draw(textToDisplay);

        if (selected)
            target.draw(cursorMarker);
    };

private:
    std::size_t cursorPos;
    std::size_t currentLine;
    bool selected;

    int textHorizontalOffset;//number of pixels by which the text is pasted to the right
    int textVerticalOffset;//same but on y-axis

    std::vector<sf::String> valueContained;

    ID_TYPE fontID;
    sf::Text contentText;
    sf::Vector2f textRelPos;
    sf::Vector2f textBoxSize;
    sf::Sprite textToDisplay;
    sf::RectangleShape cursorMarker;

    std::shared_ptr<sf::Texture> myTexture;

    Dummy background;
};

void updateWidgetDirection(Widget& toUpdate, sf::RenderWindow& window, sf::Vector2f origin = sf::Vector2f(0,0));
void renderWidgetDirection(Widget& toRender, sf::RenderTarget& target);
//void renderWidgetDirection(Widget& toRender, sf::RenderTexture& texture);

#include "Classes.h"

#endif // INTERFACE_HPP_INCLUDED
