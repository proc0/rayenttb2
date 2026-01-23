#pragma once

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <chrono>

#include "display.hpp"

#define TARGET_FPS 120

#ifdef __EMSCRIPTEN__
// padding for web components
// NOTE: canvas width should not be 100% in CSS
#define WINDOW_PAD 0
#endif
// resize debounce, prevents too many calls for Web
static inline constexpr auto RESIZE_COOLDOWN = std::chrono::milliseconds(300);

class Game {
    enum State {
        BEGIN, // initialization
        START, // intro and menu screen
        PLAY,  
        READY, // level transition
        PAUSE, 
        OVER,  // game over
        WIN,   // win conditions met
        END    // breaks loop
    };

    World world;
    Screen screen = Screen(SCREEN_WIDTH, SCREEN_HEIGHT, PIXELS_PER_METER);
    Display display;
    std::chrono::steady_clock::time_point lastResize = std::chrono::steady_clock::now();
    State state = BEGIN;
    
    public:
    int screenWidth = SCREEN_WIDTH;
    int screenHeight = SCREEN_HEIGHT;
    
    Game(): world(screen) {};
    ~Game() = default;

    const bool isRunning() const;
    void load();
    static void loop(void* self);
    void render() const;
    void debugRender();
    void resize();
    void run();
    void unload();
    void update();
};
