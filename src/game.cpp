#include "game.hpp"

void Game::load(){
    resize();
    display.load();
    world.load();
}

void Game::render() const {
    BeginDrawing();
        ClearBackground(BLACK);

        world.render();
        display.render();
    EndDrawing();
}

void Game::update(){
    if(state == END) return;

    if(!IsWindowFullscreen() && IsWindowResized()){
        auto now = std::chrono::steady_clock::now();
        if (now - lastResize > RESIZE_COOLDOWN) {
            resize();
        }
    }

    // toggle pause menu
    if(IsKeyPressed(KEY_ESCAPE)){
        if(state == PAUSE) {
            state = PLAY;
            HideCursor();
            return;
        }
        
        if(state == PLAY || state == READY) {
            state = PAUSE;
            ShowCursor();
            return;
        }
    }

    // run background stuff and UI to begin
    if(state == OVER || state == PAUSE || state == START || state == WIN) {
        // listen for play button here
        display.update(world);
        return;
    }

    if(state == PLAY){
        world.update();
        display.update(world);
    }

    if(state == BEGIN) {
        display.update(world);
        // press any key screen (needed to load sound for web, as it needs user input to load audio)
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) || IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || GetKeyPressed() != 0){
            state = START;
        }
    }

    if(state == START){
        // render some intro
        state = PLAY;
        return;
    }

    if(state == WIN){
        // render winning screen
        return;
    }
}

void Game::unload(){
    world.unload();
    display.unload();
}

void Game::loop(void* self) {
    Game* game = static_cast<Game*>(self);

#ifdef __EMSCRIPTEN__
    if (!game->isRunning()) return;
#endif

    game->update();
    game->render();

#if DEBUG 
    game->debugRender();
#endif
}

void Game::run() {
    resize();
#ifdef __EMSCRIPTEN__
    // no target FPS (3rd param) for performance
    emscripten_set_main_loop_arg(loop, this, 0, 1);
#else
    SetTargetFPS(TARGET_FPS);
    while (!WindowShouldClose() && isRunning()) {
        loop(this);
    }
#endif
}

const bool Game::isRunning() const {
    return state != END;
}

#if __EMSCRIPTEN__
EM_JS(int, getWindowWidth, (), {
    return window.document.querySelector('canvas').clientWidth;
});

EM_JS(int, getWindowHeight, (), {
    return window.document.querySelector('canvas').clientHeight;
});
#endif

void Game::resize() {
    #if __EMSCRIPTEN__
        // add padding to fit other web elements 
        int width = getWindowWidth() - WINDOW_PAD;
        int height = getWindowHeight() - WINDOW_PAD;
    #else
        int width = GetScreenWidth();
        int height = GetScreenHeight();
    #endif

    if(screenHeight != height || screenWidth != width || state == BEGIN){
        screenWidth = width; 
        screenHeight = height;
        screen.resize(width, height);
        display.resize(width, height);
    #if __EMSCRIPTEN__
        SetWindowSize(screenWidth, screenHeight);
    #endif
    }

    lastResize = std::chrono::steady_clock::now();
    TraceLog(LOG_INFO, "Window resized %dx%d", screenWidth, screenHeight);
}

void Game::debugRender() {
    world.debugRender();
}
