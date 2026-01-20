#include "display.hpp"

void Display::load(){

}

void Display::render() const {
    // const char* countText = TextFormat("Count: %i", count);
    // DrawText(countText, 50, 50, 20, WHITE);
    // if (IsCursorHidden()) {
    //     DrawText("CURSOR HIDDEN", screenWidth/2-100, 60, 20, RED);
    // } else {
    //     DrawText("CURSOR VISIBLE", screenWidth/2-100, 60, 20, GREEN);
    // }
}

void Display::resize(int width, int height) {
    screenWidth = width;
    screenHeight = width;
}

void Display::update(World& world){
}

void Display::unload(){

}
