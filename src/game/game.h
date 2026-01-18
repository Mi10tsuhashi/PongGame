#include <SDL.h>
#include <vector>

namespace game {
 struct Vector2 {
    float x;
    float y;
 };

 struct Ball {
    Vector2 pos;
    Vector2 velocity;
 };

 class Game {
    public:
        Game();
        ~Game();
        bool Initialize();
        void RunLoop();
        void ShutDown();
    private:
        void ProcessInput();
        void UpdateGame();
        void GenerateOutPut();

        SDL_Window* mWindow;
        SDL_Renderer* mRenderer;
        bool running = false;

        Uint32 mPrevTick;

        // 左側のプレイヤー
        Vector2 mPlayer1PaddlePos;

        int mPlayer1PaddleDir;

        // 右側のプレイヤー
        Vector2 mPlayer2PaddlePos;

        int mPlayer2PaddleDir;

        std::vector<Ball> mBalls;

        const int INIT_BALL_AMOUNT = 5;
 };
}