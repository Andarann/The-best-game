#pragma once

#include <SFML/Graphics.hpp>

#include <vector>

class RichString{
public:

private:

};

class RichText{
public:


private:
    std::vector<sf::String> characters;//Contains all the characters
    std::vector<std::size_t> stringSize;//Size of each string
    std::vector<sf::Color> stringColor;//Color of each string
    std::vector<sf::Uint32> stringStyle;//Style of each string
};
