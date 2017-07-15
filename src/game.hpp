#ifndef GAME_HPP_INCLUDED
#define GAME_HPP_INCLUDED

#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>

#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#define SCROLL_SPEED 500//Number of pixel scrolled every second
#define INITIAL_WINDOW_WIDTH 1000
#define INITIAL_WINDOW_HEIGHT 640

#include "Classes.h"

void gameLoop();



#endif // GAME_HPP_INCLUDED
