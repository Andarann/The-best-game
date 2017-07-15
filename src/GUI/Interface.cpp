
#include "Interface.hpp"

sf::Event Widget::events;

std::vector<sf::Uint32> Widget::unicodeBuffer;
std::vector<sf::Keyboard::Key> Widget::keyBuffer;

ID_TYPE Widget::currentFontID = 1;//1, because 0 means there was no texture initially bound
ID_TYPE Widget::currentTextureID = 1;

ID_TYPE Widget::currentID = 0;
std::queue<ID_TYPE> Widget::vacantID;

std::map<std::string, ID_TYPE> Widget::stringToIDTableFont;
std::map<ID_TYPE, std::unique_ptr<sf::Font>> Widget::widgetFonts;
std::map<ID_TYPE, unsigned int> Widget::numberOfFontUsers;
std::queue<ID_TYPE> Widget::vacantFontID;

std::map<std::string, ID_TYPE> Widget::stringToIDTableTexture;
std::map<ID_TYPE, std::unique_ptr<sf::Texture>> Widget::widgetTextures;
std::map<ID_TYPE, unsigned int> Widget::numberOfTextureUsers;
std::queue<ID_TYPE> Widget::vacantTextureID;

bool Container::memberInteracted = false;

bool Widget::widgetAlreadyHovered;
bool Widget::mouseButtonsMaintained[sf::Mouse::ButtonCount];
bool Widget::ignoreMouseMaintained[sf::Mouse::ButtonCount];
bool Widget::mouseButtonClick[sf::Mouse::ButtonCount];
bool Widget::ignoreMouseClick[sf::Mouse::ButtonCount];
bool Widget::keyboardButtonsMaintained[sf::Keyboard::KeyCount];
bool Widget::keyboardButtonPushed[sf::Keyboard::KeyCount];

Widget::Widget()
{
    clicked = false;
    clickMaintained = false;
    hovered = false;
    updated = true;

    if (Widget::vacantID.empty())
    {
        setID(Widget::currentID);
        Widget::currentID++;
    }
    else
    {
        setID(Widget::vacantID.front());
        Widget::vacantID.pop();
    }
}

Widget::~Widget()
{
    Widget::vacantID.push(getID());
}

bool Widget::isHovered(sf::RenderWindow& window, sf::FloatRect* widgetShape, sf::Vector2f origin)
{
    sf::FloatRect absoluteRect(*widgetShape);
    absoluteRect.top += origin.y;
    absoluteRect.left += origin.x;

    //bool isHoveredByMouse = widgetShape->contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);
    bool isHoveredByMouse = absoluteRect.contains(sf::Mouse::getPosition(window).x, sf::Mouse::getPosition(window).y);

    if (!isHoveredByMouse)
    {
        hovered = false;
        return false;
    }
    else
    {
        if (Widget::widgetAlreadyHovered)
        {
            hovered = false;
            return false;
        }
        else
        {
           hovered = true;
           Widget::widgetAlreadyHovered = true;
           return true;
        }
    }
}

bool Widget::isClicked() const
{
    return clicked;
}

bool Widget::wasUpdated() const
{
    return updated;
}

void Widget::update(sf::RenderWindow& window, sf::Vector2f origin)
{
    //Nothing since you're not supposed to use a the Widget class directly
}

ID_TYPE Widget::getID() const
{
    return ID;
}

void Widget::draw(sf::RenderTarget& target) const
{
    //Nothing, you're not supposed to render a widget directly
}

bool Widget::updateWidgetState(sf::RenderWindow& window, sf::FloatRect* widgetShape, sf::Vector2f origin)
{
    bool hoveredBef(hovered);
    bool clickedBef(clicked);
    bool clickMaintainedBef(clickMaintained);

    isHovered(window, widgetShape, origin);

    if (hovered &&
        Widget::isButtonClicked(window, sf::Mouse::Left) &&
        (!clicked))
    {
        Widget::ignoreButtonClick(window, sf::Mouse::Left);
        clicked = true;
    }
    else if (hovered &&
             Widget::isButtonMaintained(window, sf::Mouse::Left) &&
             clicked)
    {
        clicked = false;

        clickMaintained = true;
    }
    else if (!hovered ||
             (!Widget::isButtonMaintained(window, sf::Mouse::Left) &&
              !Widget::isButtonClicked(window, sf::Mouse::Left)))
    {
        clicked = false;
        clickMaintained = false;
    }

    return ((hoveredBef != hovered) || (clickedBef != clicked) || (clickMaintainedBef != clickMaintained));
}

void Widget::setUpdatedState(bool toSet)
{
    updated = toSet;
}

void Widget::setID(ID_TYPE newID)
{
    ID = newID;
}

ID_TYPE Widget::giveTextureID(std::string texturePath)
{
    if (Widget::stringToIDTableTexture.find(texturePath) == Widget::stringToIDTableTexture.end())
    {
        sf::Texture loadingTexture;

        if (loadingTexture.loadFromFile(texturePath));
        else
        {
            std::stringstream errorStream;
            errorStream << "From Widget::giveTextureID : Failed to load image of the following path : " << texturePath << '\n';
            std::cerr << errorStream.str();
            throw errorStream.str().c_str();
        }

        ID_TYPE givenID;

        if (Widget::vacantTextureID.empty())
        {
            givenID = Widget::currentTextureID;
            Widget::currentTextureID++;
        }
        else
        {
            givenID = Widget::vacantTextureID.front();
            Widget::vacantTextureID.pop();
        }

        Widget::stringToIDTableTexture.insert(std::make_pair(texturePath, givenID));
        Widget::widgetTextures.insert(std::make_pair(givenID, std::unique_ptr<sf::Texture>(new sf::Texture(loadingTexture)) ));
    }
    //else, the texture is loaded

    return Widget::stringToIDTableTexture.find(texturePath)->second;//Yummy yummy syntax
}

ID_TYPE Widget::giveFontID(std::string fontPath)
{
    if (Widget::stringToIDTableFont.find(fontPath) == Widget::stringToIDTableFont.end())
    {
        sf::Font loadingFont;

        if (loadingFont.loadFromFile(fontPath));
        else
        {
            std::stringstream errorStream;
            errorStream << "From Widget::giveFontID : Failed to load font of the following path : " << fontPath << '\n';
            std::cerr << errorStream.str();
            throw errorStream.str().c_str();
        }

        ID_TYPE givenID;

        if (Widget::vacantFontID.empty())
        {
            givenID = Widget::currentFontID;
            Widget::currentFontID++;
        }
        else
        {
            givenID = Widget::vacantFontID.front();
            Widget::vacantFontID.pop();
        }

        Widget::stringToIDTableFont.insert(std::make_pair(fontPath, givenID));
        Widget::widgetFonts.insert(std::make_pair(givenID, std::unique_ptr<sf::Font>(new sf::Font(loadingFont)) ));
    }
    //else, the font is already loaded

    return Widget::stringToIDTableFont.find(fontPath)->second;
}

sf::Texture* Widget::giveTexturePointerOfID(ID_TYPE textureID)
{
    if (Widget::widgetTextures.find(textureID) == Widget::widgetTextures.end())
    {
        std::stringstream errorStream;
        errorStream << "From Widget::giveTexturePointerOfID : Error : There is no texture of ID " << textureID << " stored\n";
        std::cerr << errorStream.str();
        throw errorStream.str().c_str();
    }
    else
        return Widget::widgetTextures.find(textureID)->second.get();
}

sf::Font* Widget::giveFontPointerOfID(ID_TYPE fontID)
{
    if (Widget::widgetFonts.find(fontID) == Widget::widgetFonts.end())
    {
        std::stringstream errorStream;
        errorStream << "From Widget::giveFontPointerOfID : Error : There is no font of ID " << fontID << " stored\n";
        std::cerr << errorStream.str();
        throw errorStream.str().c_str();
    }
    else
        return Widget::widgetFonts.find(fontID)->second.get();
}

void Widget::removeTexture(ID_TYPE textureID)
{

}

void Widget::removeFont(ID_TYPE fontID)
{

}

bool Widget::Init(sf::Window const& mainWindow)
{
    for (int i(0) ; i < sf::Mouse::ButtonCount ; i++)
    {
        Widget::mouseButtonsMaintained[i] = false;
        Widget::ignoreMouseMaintained[i] = false;
        Widget::mouseButtonClick[i] = false;
        Widget::ignoreMouseClick[i] = false;
    }

    for (int i(0) ; i < sf::Keyboard::KeyCount ; i++)
    {
        Widget::keyboardButtonsMaintained[i] = false;
        Widget::keyboardButtonPushed[i] = false;
    }

    Widget::widgetAlreadyHovered = false;
}

void Widget::UpdateEvents(sf::Window const& mainWindow)
{
    if (mainWindow.hasFocus())//In case there are multiple windows, we ensure that we only update events in the right window
    {
        Widget::widgetAlreadyHovered = false;
        Widget::unicodeBuffer.clear();
        Widget::keyBuffer.clear();

        for (int i(0); i<sf::Keyboard::KeyCount ; i++)
        {
            if (sf::Keyboard::isKeyPressed(i))//If the button is pressed...
            {
                if (Widget::keyboardButtonPushed[i] == false && Widget::keyboardButtonsMaintained[i] == false)//We first for once set it on clicked...
                {
                    Widget::keyboardButtonPushed[i] = true;
                }
                else if (Widget::keyboardButtonPushed[i] == true && Widget::keyboardButtonsMaintained[i] == false)//Then block it on maintained
                {
                    Widget::keyboardButtonPushed[i] = false;
                    Widget::keyboardButtonsMaintained[i] = true;
                }
            }
            else if (Widget::keyboardButtonPushed[i] || Widget::keyboardButtonsMaintained[i])//Release the button
            {
                Widget::keyboardButtonPushed[i] = false;
                Widget::keyboardButtonsMaintained[i] = false;
            }
        }

        for (int i(0);i<sf::Mouse::ButtonCount;i++)
        {
            if (sf::Mouse::isButtonPressed(i))//If the button is pressed...
            {
                if (Widget::mouseButtonClick[i] == false && Widget::mouseButtonsMaintained[i] == false)//We first for once set it on clicked...
                {
                    Widget::mouseButtonClick[i] = true;
                    Widget::ignoreMouseClick[i] = false;
                }
                else if (Widget::mouseButtonClick[i] == true && Widget::mouseButtonsMaintained[i] == false)//Then block it on maintained
                {
                    Widget::mouseButtonClick[i] = false;
                    Widget::mouseButtonsMaintained[i] = true;
                    Widget::ignoreMouseMaintained[i] = false;
                }
            }
            else if (Widget::mouseButtonClick[i] || Widget::mouseButtonsMaintained[i])//Release the button
            {
                Widget::mouseButtonClick[i] = false;
                Widget::mouseButtonsMaintained[i] = false;
                Widget::ignoreMouseClick[i] = false;
                Widget::ignoreMouseMaintained[i] = false;
            }
        }

        sf::String userInput;
        // ...
        while( mainWindow.pollEvent(Widget::events) )
        {
            if(Widget::events.type == sf::Event::TextEntered)
            {
                Widget::unicodeBuffer.push_back(Widget::events.text.unicode);
            }
            if (Widget::events.type == sf::Event::KeyPressed)
            {
                Widget::keyBuffer.push_back(Widget::events.key.code);
            }
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            Widget::keyBuffer.push_back(sf::Keyboard::Left);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            Widget::keyBuffer.push_back(sf::Keyboard::Right);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            Widget::keyBuffer.push_back(sf::Keyboard::Up);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            Widget::keyBuffer.push_back(sf::Keyboard::Down);
    }
}

bool Widget::isKeyPressed(sf::Window const& mainWindow, sf::Keyboard::Key keyCheck)
{
    if (mainWindow.hasFocus())
        return (Widget::keyboardButtonsMaintained[keyCheck] || Widget::keyboardButtonPushed[keyCheck]);
    else
        return false;//No interaction
}

bool Widget::isKeyPushed(sf::Window const& mainWindow, sf::Keyboard::Key keyCheck)
{
    if (mainWindow.hasFocus())
        return (Widget::keyboardButtonPushed[keyCheck]);
    else
        return false;//No interaction
}

bool Widget::isKeyMaintained(sf::Window const& mainWindow, sf::Keyboard::Key keyCheck)
{
    if (mainWindow.hasFocus())
        return (Widget::keyboardButtonsMaintained[keyCheck]);
    else
        return false;//No interaction
}

bool Widget::isButtonPressed(sf::Window const& mainWindow, sf::Mouse::Button buttonCheck)
{
    if (mainWindow.hasFocus())
        return ((Widget::mouseButtonsMaintained[buttonCheck] && !Widget::ignoreMouseMaintained[buttonCheck]) ||
                (Widget::mouseButtonClick[buttonCheck] && !Widget::ignoreMouseClick[buttonCheck]));
    else
        return false;//No interaction
}

bool Widget::isButtonClicked(sf::Window const& mainWindow, sf::Mouse::Button buttonCheck)
{
    if (mainWindow.hasFocus())
        return (Widget::mouseButtonClick[buttonCheck] && !Widget::ignoreMouseClick[buttonCheck]);
    else
        return false;//No interaction
}

bool Widget::isButtonMaintained(sf::Window const& mainWindow, sf::Mouse::Button buttonCheck)
{
    if (mainWindow.hasFocus())
        return (Widget::mouseButtonsMaintained[buttonCheck] && !Widget::ignoreMouseMaintained[buttonCheck]);
    else
        return false;//No interaction
}

void Widget::ignoreButtonClick(sf::Window const& mainWindow, sf::Mouse::Button toIgnore)
{
    if (mainWindow.hasFocus())
        Widget::ignoreMouseClick[toIgnore] = true;
}

void Widget::ignoreButtonMaintained(sf::Window const& mainWindow, sf::Mouse::Button toIgnore)
{
    if (mainWindow.hasFocus())
        Widget::ignoreMouseMaintained[toIgnore] = true;
}


bool Widget::isWidgetContainer(Widget* toCheck)
{
    return (typeid(*toCheck) == typeid(Container));
}

bool Widget::isAWidgetHovered(sf::Window const& windowToCheck)
{
    return (windowToCheck.hasFocus() && Widget::widgetAlreadyHovered);
}

Dummy::Dummy(std::string imagePath, sf::Vector2f size, sf::Vector2f pos)
{
    textureID = Widget::giveTextureID(imagePath);
    sprite_.setTexture(*Widget::giveTexturePointerOfID(textureID));
    sprite_.setScale(size.x / sprite_.getTextureRect().width, size.y / sprite_.getTextureRect().height);
    sprite_.setPosition(pos);
}

Dummy::Dummy(sf::Color const& fillColor, sf::Vector2f size, sf::Vector2f pos)
{

    textureID = 0;
    sf::Texture tempTexture;
    tempTexture.create(size.x, size.y);
    sprite_.setTexture(tempTexture);//Small hack : We purposely let it go out of scope so that we can color the sprite as we wish
    sprite_.setPosition(pos);
    sprite_.setColor(fillColor);
}

void Dummy::setTexture(std::string imagePath)
{
    textureID = Widget::giveTextureID(imagePath);
    sprite_.setTexture(*Widget::giveTexturePointerOfID(textureID));
}

void Dummy::setPosition(sf::Vector2f newPos)
{
    sprite_.setPosition(newPos);
}

void Dummy::setSize(sf::Vector2f newSize)
{
    if (sprite_.getTextureRect().width && sprite_.getTextureRect().height)
    {
        sprite_.setScale(newSize.x / sprite_.getTextureRect().width, newSize.y / sprite_.getTextureRect().height);
    }
    else
    {
        sf::Texture tempTexture;
        tempTexture.create(newSize.x, newSize.y);
        sprite_.setTexture(tempTexture);
    }
}

void Dummy::setFillColor(sf::Color newColor)
{
    sprite_.setColor(newColor);
}

sf::FloatRect Dummy::getLocalBounds() const
{
    return sprite_.getLocalBounds();
}

sf::FloatRect Dummy::getGlobalBounds() const
{
    return sprite_.getGlobalBounds();
}

sf::Vector2f Dummy::getSize() const
{
    return sf::Vector2f(sprite_.getGlobalBounds().width - sprite_.getGlobalBounds().left, sprite_.getGlobalBounds().height - sprite_.getGlobalBounds().top);
}

sf::Vector2f Dummy::getPosition() const
{
    return sprite_.getPosition();
}

sf::Color Dummy::getFillColor() const
{
    return sprite_.getColor();
}

void Dummy::update(sf::RenderWindow& window, sf::Vector2f origin)
{
    updated = false;//A dummy can't be interacted with : therefore, it is never updated
}

void Dummy::draw(sf::RenderTarget& target) const
{
    //sprite_.setTexture(*Widget::giveTexturePointerOfID(textureID));
    target.draw(sprite_);
}

Button::Button()
{
    buttonShape.setSize(sf::Vector2f(1,1));
    buttonShape.setFillColor(sf::Color(255,255,255));
    buttonShape.setPosition(sf::Vector2f(0,0));
}

Button::Button(sf::Vector2f pos, sf::Vector2i boundariesSize, sf::Color buttonColoration, std::string buttonContent, short fontSize, sf::Vector2f textPosPercent)
{
    buttonColor = buttonColoration;
    hoveredColor = buttonColor;
    clickedColor = buttonColor;

    fontID = Widget::giveFontID("Cousine-Regular.TTF");
    buttonString.setFont(*Widget::giveFontPointerOfID(fontID));

    buttonString.setString(buttonContent);
    buttonString.setColor(sf::Color(0,0,0));
    buttonString.setCharacterSize(fontSize);

    buttonShape.setSize(sf::Vector2f(boundariesSize.x + buttonString.getLocalBounds().width ,
                                     boundariesSize.y + buttonString.getLocalBounds().height));

    textPos = textPosPercent;

    buttonShape.setFillColor(buttonColoration);
    buttonShape.setPosition(pos);

    buttonString.setPosition(-buttonString.getLocalBounds().left + buttonShape.getPosition().x + (float)textPos.x * (buttonShape.getSize().x - buttonString.getLocalBounds().width) ,
                             -buttonString.getLocalBounds().top + buttonShape.getPosition().y + (float)textPos.y * (buttonShape.getSize().y - buttonString.getLocalBounds().height));
}

Button::Button(sf::Vector2f pos, sf::Vector2i boundariesSize, sf::Color buttonColoration, std::string texturePath, std::string buttonContent, short fontSize, sf::Vector2f textPosPercent)
{
    buttonColor = buttonColoration;
    hoveredColor = buttonColor;
    clickedColor = buttonColor;

    fontID = Widget::giveFontID("Cousine-Regular.TTF");
    buttonString.setFont(*Widget::giveFontPointerOfID(fontID));

    buttonString.setString(buttonContent);
    buttonString.setColor(sf::Color(0,0,0));
    buttonString.setCharacterSize(fontSize);

    buttonShape.setTexture(texturePath);

    buttonShape.setSize(sf::Vector2f(boundariesSize.x + buttonString.getLocalBounds().width ,
                                     boundariesSize.y + buttonString.getLocalBounds().height));

    textPos = textPosPercent;

    buttonShape.setFillColor(buttonColoration);
    buttonShape.setPosition(pos);

    buttonString.setPosition(-buttonString.getLocalBounds().left + buttonShape.getPosition().x + (float)textPos.x * (buttonShape.getSize().x - buttonString.getLocalBounds().width) ,
                             -buttonString.getLocalBounds().top + buttonShape.getPosition().y + (float)textPos.y * (buttonShape.getSize().y - buttonString.getLocalBounds().height));
}

Button::~Button()
{

}

void Button::setString(std::string stringToPut)
{
    sf::Vector2i boundariesSize;
    boundariesSize.x = buttonShape.getSize().x - buttonString.getLocalBounds().width;
    boundariesSize.y = buttonShape.getSize().y - buttonString.getLocalBounds().height;

    buttonString.setString(stringToPut);

    buttonShape.setSize(sf::Vector2f(boundariesSize.x + buttonString.getLocalBounds().width ,
                                     boundariesSize.y + buttonString.getLocalBounds().height));


    buttonString.setPosition(-buttonString.getLocalBounds().left + buttonShape.getPosition().x + (float)textPos.x * (buttonShape.getSize().x - buttonString.getLocalBounds().width) ,
                             -buttonString.getLocalBounds().top + buttonShape.getPosition().y + (float)textPos.y * (buttonShape.getSize().y - buttonString.getLocalBounds().height));
}

void Button::setFillColor(sf::Color newColor)
{
    buttonColor = newColor;
}

void Button::setHoveredColor(sf::Color newColor)
{
    hoveredColor = newColor;
}

void Button::setClickedColor(sf::Color newColor)
{
    clickedColor = newColor;
}

void Button::setTexture(std::string texturePath)
{
    buttonShape.setTexture(texturePath);
}

void Button::setDummy(Dummy& toPut)
{
    toPut.setSize(buttonShape.getSize());
    toPut.setPosition(buttonShape.getPosition());
    buttonShape = toPut;
}

void Button::setSize(sf::Vector2f size_)
{
    buttonShape.setSize(size_);
    buttonString.setPosition(-buttonString.getLocalBounds().left + buttonShape.getPosition().x + (float)textPos.x * (buttonShape.getSize().x - buttonString.getLocalBounds().width) ,
                             -buttonString.getLocalBounds().top + buttonShape.getPosition().y + (float)textPos.y * (buttonShape.getSize().y - buttonString.getLocalBounds().height));
}

void Button::setPosition(sf::Vector2f pos)
{
    buttonShape.setPosition(pos);
    buttonString.setPosition(-buttonString.getLocalBounds().left + buttonShape.getPosition().x + (float)textPos.x * (buttonShape.getSize().x - buttonString.getLocalBounds().width) ,
                             -buttonString.getLocalBounds().top + buttonShape.getPosition().y + (float)textPos.y * (buttonShape.getSize().y - buttonString.getLocalBounds().height));
}

sf::Vector2f Button::getSize() const
{
    return buttonShape.getSize();
}

sf::Vector2f Button::getPosition() const
{
    return buttonShape.getPosition();
}

sf::Color Button::getColor() const
{
    return buttonColor;
}

sf::Color Button::getHoveredColor() const
{
    return hoveredColor;
}

sf::Color Button::getClickedColor() const
{
    return clickedColor;
}

std::string Button::getButtonString() const
{
    return buttonString.getString();
}

void Button::update(sf::RenderWindow& window, sf::Vector2f origin)
{
    updated = updateWidgetState(window, &buttonShape.getGlobalBounds(), origin);
}

void Button::draw(sf::RenderTarget& target) const
{
    if (clicked || clickMaintained)
    {
        buttonShape.setFillColor(clickedColor);
        buttonShape.draw(target);
        target.draw(buttonString);
    }
    else if (hovered)
    {
        buttonShape.setFillColor(hoveredColor);
        buttonShape.draw(target);
        target.draw(buttonString);
    }
    else
    {
        buttonShape.setFillColor(buttonColor);
        buttonShape.draw(target);
        target.draw(buttonString);
    }

}

void Button::operator=(Dummy& a)
{
    setDummy(a);
}

HorizontalSlider::HorizontalSlider()
{

    leftClickMaintained = false;

    sliderBackground.setSize(sf::Vector2f(1,1));

    graphicalSlider.setSize(sf::Vector2f(sliderBackground.getSize().x/5, sliderBackground.getSize().y));

    setPosition(sf::Vector2f(0,0));
}

HorizontalSlider::HorizontalSlider(sf::Vector2f pos, sf::Vector2f size_)
{
    leftClickMaintained = false;

    sliderBackground.setSize(size_);
    graphicalSlider.setSize(sf::Vector2f(sliderBackground.getSize().x/5, sliderBackground.getSize().y));

    setPosition(pos);
}

HorizontalSlider::HorizontalSlider(sf::Vector2f pos, sf::Vector2f size_, double sliderRelativeSize)
{
    leftClickMaintained = false;

    sliderBackground.setSize(size_);

    while (sliderRelativeSize > 1.0)
        sliderRelativeSize -= 1.0;

    graphicalSlider.setSize(sf::Vector2f(sliderBackground.getSize().x * sliderRelativeSize, sliderBackground.getSize().y));

    setPosition(pos);
}

HorizontalSlider::~HorizontalSlider()
{

}

void HorizontalSlider::setSliderTexture(Dummy dummySlider)
{
    dummySlider.setSize(graphicalSlider.getSize());
    dummySlider.setPosition(graphicalSlider.getPosition());
    graphicalSlider = dummySlider;
}

void HorizontalSlider::setSliderTexture(std::string sliderTexturePath)
{
    graphicalSlider.setTexture(sliderTexturePath);
}

void HorizontalSlider::setSliderTexture(sf::Color sliderColor)
{
    graphicalSlider.setFillColor(sliderColor);
}

void HorizontalSlider::setBackgroundTexture(Dummy dummyBackground)
{
    dummyBackground.setSize(sliderBackground.getSize());
    dummyBackground.setPosition(sliderBackground.getPosition());
    sliderBackground = dummyBackground;
}

void HorizontalSlider::setBackgroundTexture(std::string backgroundTexturePath)
{
    sliderBackground.setTexture(backgroundTexturePath);
}

void HorizontalSlider::setBackgroundTexture(sf::Color backgroundColor)
{
    sliderBackground.setFillColor(backgroundColor);
}

float HorizontalSlider::getCurrentPosPercent() const
{
    return currentPos*2;
}

void HorizontalSlider::setPosition(sf::Vector2f newPos)
{
    sliderBackground.setPosition(newPos);
    graphicalSlider.setPosition(sf::Vector2f(sliderBackground.getPosition().x + currentPos * (sliderBackground.getSize().x - graphicalSlider.getSize().x), sliderBackground.getPosition().y));
}
void HorizontalSlider::setSize(sf::Vector2f newSize)
{
    float currentRelSize = getSliderRelativeLength();
    sliderBackground.setSize(newSize);
    graphicalSlider.setSize(sf::Vector2f(sliderBackground.getSize().x * currentRelSize, sliderBackground.getSize().y));
}

void HorizontalSlider::setSliderRelativeLength(double newRelLen)
{
    graphicalSlider.setSize(sf::Vector2f(sliderBackground.getSize().x * newRelLen, sliderBackground.getSize().y));
}

sf::Vector2f HorizontalSlider::getPosition() const
{
    return sliderBackground.getPosition();
}

sf::Vector2f HorizontalSlider::getSize() const
{
    return sliderBackground.getSize();
}

double HorizontalSlider::getSliderRelativeLength() const
{
    return graphicalSlider.getSize().x / sliderBackground.getSize().x;
}

void HorizontalSlider::update(sf::RenderWindow& window, sf::Vector2f origin)
{
    updated = false;

    updateWidgetState(window, &graphicalSlider.getGlobalBounds(), origin);

    if (clicked || clickMaintained)
    {
        if (clicked)
            clickOrigin = sf::Mouse::getPosition(window).x - origin.x - graphicalSlider.getPosition().x;

        leftClickMaintained = true;
    }

    if (Widget::isButtonPressed(window, sf::Mouse::Left) == false)
        leftClickMaintained = false;

    if (leftClickMaintained)
    {
        currentPos = ((sf::Mouse::getPosition(window).x - origin.x - clickOrigin)- sliderBackground.getPosition().x) / (sliderBackground.getSize().x - graphicalSlider.getSize().x);

        if (currentPos < 0)
            currentPos = 0;
        else if (currentPos > 0.5)
            currentPos = 0.5;

        sf::Vector2f ancientPos = graphicalSlider.getPosition();
        graphicalSlider.setPosition(sf::Vector2f(sliderBackground.getPosition().x + currentPos * (sliderBackground.getSize().x - graphicalSlider.getSize().x), sliderBackground.getPosition().y));

        if (ancientPos != graphicalSlider.getPosition())
            updated = true;
    }
}

void HorizontalSlider::draw(sf::RenderTarget& target) const
{
    sliderBackground.draw(target);
    graphicalSlider.draw(target);
}

Checkbox::Checkbox() : checkBoxChecked("checkboxcheck.png", sf::Vector2f(50,50)), checkBoxUnchecked("checkboxuncheck.png", sf::Vector2f(50,50))
{
    value = false;

}

Checkbox::Checkbox(bool init, sf::Vector2f widSize, sf::Vector2f pos) : checkBoxChecked("checkboxcheck.png", widSize, pos), checkBoxUnchecked("checkboxuncheck.png", widSize, pos)
{
    value = init;
}

Checkbox::~Checkbox()
{

}

bool Checkbox::getValue() const
{
    return value;
}

void Checkbox::update(sf::RenderWindow& window, sf::Vector2f origin)
{
    updated = false;

    updateWidgetState(window, &checkBoxChecked.getGlobalBounds(), origin);

    if (clicked)
    {
        value = !value;
        updated = true;
    }
}

void Checkbox::draw(sf::RenderTarget& target) const
{
    if (value)
    {
        checkBoxChecked.draw(target);
    }
    else
    {
        checkBoxUnchecked.draw(target);
    }
}

Container::Container()
{
    forceRender = true;

    renderSprite.setPosition(sf::Vector2f(0,0));
    size = sf::Vector2f(1,1);

    myRenderTexture = std::shared_ptr<sf::RenderTexture>(new sf::RenderTexture());
    myRenderTexture.get()->create(size.x,size.y);

    background.setSize(size);
    background.setFillColor(sf::Color(0,0,0,0));
}

Container::Container(sf::Vector2f pos, sf::Vector2f size_, sf::Color backgroundColor)
{
    forceRender = true;

    renderSprite.setPosition(pos);
    size = size_;

    myRenderTexture = std::shared_ptr<sf::RenderTexture>(new sf::RenderTexture());
    myRenderTexture.get()->create(size.x,size.y);

    background.setSize(size);
    background.setFillColor(backgroundColor);
}

Container::Container(sf::Vector2f pos, sf::Vector2f size_, std::string texturePath)
{
    forceRender = true;

    renderSprite.setPosition(pos);
    size = size_;

    myRenderTexture = std::shared_ptr<sf::RenderTexture>(new sf::RenderTexture());
    myRenderTexture.get()->create(size.x,size.y);

    background.setSize(size);
    background.setTexture(texturePath);
}

Container::~Container()
{

}

void Container::setTitle(std::string newTitle)
{

}

void Container::update(sf::RenderWindow& window, sf::Vector2f origin)
{
    Container::memberInteracted = false;


    //It is necessary to ensure that the origin is correctly set, since all updates are relative to the origin
    sf::Vector2f thisContainerOrigin(renderSprite.getPosition().x + origin.x,
                                     renderSprite.getPosition().y + origin.y);

    if (forceRender)
    {
        forceRender = false;
        updated = true;
    }
    else
        updated = false;

    //Then we update every widget contained
    for (std::map<std::string, WidgetAndAttribs>::reverse_iterator it = allWidgets.rbegin() ; it != allWidgets.rend() ; it++)
    {
        updateWidgetDirection(*(it->second.widget), window, thisContainerOrigin);
        if ((*(it->second.widget)).wasUpdated() && !(it->second.hideState))
        {
            updated = true;
            (*(it->second.widget)).setUpdatedState(false);
        }
    }

    //We update the container after so that it does not mess the hovering, since he is not prioritary
    updateWidgetState(window, &renderSprite.getGlobalBounds(), origin);
}

void Container::draw(sf::RenderTarget& target) const
{
    if (updated)//We only do that if there is a need to re-draw the Container
    {
        myRenderTexture.get()->setActive();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //std::cout << "Drawn\n";
        background.setPosition(sf::Vector2f(0,0));

        myRenderTexture.get()->clear(sf::Color(0,0,0,0));
        background.draw(*myRenderTexture.get());

        for (auto it = allWidgets.begin() ; it != allWidgets.end() ; it++)
            if (it->second.hideState == false)
                renderWidgetDirection(*((it->second).widget), *myRenderTexture.get());

        myRenderTexture.get()->display();

        renderSprite.setTexture(myRenderTexture.get()->getTexture());
    }

    target.draw(renderSprite);

}

void Container::removeWidget(std::string name)
{
    allWidgets.erase(name);
    forceRender = true;
}

Widget* Container::giveWidgetPointer(std::string widgetName) const
{
    auto findID = allWidgets.find(widgetName);

    if (findID == allWidgets.end())
        std::cout << "Widget of name '" << widgetName << "' does not exist in this system\n";
    else
        return findID->second.widget.get();
}

void Container::hideWidget(std::string widgetName)
{
    auto findID = allWidgets.find(widgetName);

    if (findID == allWidgets.end())
        std::cout << "Widget of name '" << widgetName << "' does not exist in this system. Can't hide\n";
    else
    {
        forceRender = true;
        findID->second.hideState = true;
    }
}

void Container::showWidget(std::string widgetName)
{
    auto findID = allWidgets.find(widgetName);

    if (findID == allWidgets.end())
        std::cout << "Widget of name '" << widgetName << "' does not exist in this system. Can't show\n";
    else
    {
        forceRender = true;
        findID->second.hideState = false;
    }
}

bool Container::isWidgetHidden(std::string widgetName) const
{
    auto findID = allWidgets.find(widgetName);

    if (findID == allWidgets.end())
        std::cout << "Widget of name '" << widgetName << "' does not exist in this system. Can't give hidden state\n";
    else
        return findID->second.hideState;
}

void updateWidgetDirection(Widget& toUpdate, sf::RenderWindow& window, sf::Vector2f origin)
{
    toUpdate.update(window, origin);
}

void renderWidgetDirection(Widget& toRender, sf::RenderTarget& target)
{
    toRender.draw(target);
}
