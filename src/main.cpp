#include "game.hpp"

int main(void){
    Game game;

#ifdef __EMSCRIPTEN__
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
#else
    SetConfigFlags(FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT | FLAG_WINDOW_RESIZABLE);
#endif

    InitWindow(game.screenWidth, game.screenHeight, PROJECT_NAME);
    InitAudioDevice();
    SetExitKey(KEY_NULL);
    
    game.load();
    game.run();
    game.unload();

    CloseAudioDevice();
    CloseWindow();

    return 0;
}