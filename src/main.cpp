#include "game/game.h"

int main(int argc, char *argv[]) {
    using namespace game;
    Game game;

    bool success = game.Initialize();

    if(success) {
        game.RunLoop();
    }
    game.ShutDown();
    return 0;
}