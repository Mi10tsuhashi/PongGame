#include "game.h"
#include <cmath>
#include <random> 

namespace game {
    using namespace std;
    const char* TITLE = "title";
    const int INIT_WIDTH = 1024;
    const int INIT_HEIGHT =768;
    const int WALL_THICKNESS = 15;
    const int PADDLE_WIDTH = 5;
    const int PADDLE_HEIGHT = 40;
    const int BALL_WIDTH = 5;
    const int BALL_HEIGHT = 5;

    // constructor
    Game::Game() :  mWindow(nullptr), mRenderer(nullptr), running(true), mPrevTick(0) {

    }

    // destructor
    Game::~Game() {
        ShutDown();
    }

    // ┌ public function ┐
    bool Game::Initialize() {
        int sdlResult = SDL_Init(SDL_INIT_AUDIO);
        // デバッグログを有効化
        SDL_LogSetAllPriority(SDL_LOG_PRIORITY_DEBUG);

        if(sdlResult != 0) {
            SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
            return false;
        }

        mPrevTick = SDL_GetTicks();

        mWindow = SDL_CreateWindow(
            TITLE,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            INIT_WIDTH,
            INIT_HEIGHT,
            0
        );

        if(!mWindow) {
            SDL_Log("Unable to initialize window: %s", SDL_GetError());
            return false;
        }

        mRenderer = SDL_CreateRenderer(
            mWindow,
            -1,
            SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
        );

        if(!mRenderer) {
            SDL_Log("Unable to initialize renderer: %s", SDL_GetError());
            return false;
        }

        // Player1のパドルの初期化
        mPlayer1PaddlePos.x = 10;
        mPlayer1PaddlePos.y = INIT_HEIGHT / 2.0f;

        // Player2のパドルの初期化
        mPlayer2PaddlePos.x = INIT_WIDTH - 10;
        mPlayer2PaddlePos.y = INIT_HEIGHT / 2.0f;

        // ballの初期化
        std::random_device rd;
        std::mt19937 gen(rd());

        // 0°～360°のランダムな角度
        std::uniform_real_distribution<float> angleDist(0.0f, 2.0f * M_PI);

        // X方向の速度: INIT_WIDTH/6.5 ～ INIT_WIDTH/6.0
        std::uniform_real_distribution<float> speedXDist(INIT_WIDTH / 6.5f, INIT_WIDTH / 6.0f);

        // Y方向の速度: INIT_HEIGHT/6.5 ～ INIT_HEIGHT/6.0
        std::uniform_real_distribution<float> speedYDist(INIT_HEIGHT / 6.5f, INIT_HEIGHT / 6.0f);

        for(int i = 0; i < INIT_BALL_AMOUNT; i++){
            Ball ball;
            ball.pos.x = INIT_WIDTH / 2.0f;
            ball.pos.y = INIT_HEIGHT / 2.0f;

            float angle = angleDist(gen);
            float speedX = speedXDist(gen);
            float speedY = speedYDist(gen);

            // 角度から単位ベクトルを計算し、速度の大きさを掛ける
            ball.velocity.x = std::cos(angle) * speedX;
            ball.velocity.y = std::sin(angle) * speedY;

            mBalls.push_back(ball);
        }

        return true;
    }
    void Game::RunLoop() {
        while(running) {
            ProcessInput();
            UpdateGame();
            GenerateOutPut();
        }

    }
    void Game::ShutDown() {
        SDL_DestroyRenderer(mRenderer);
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
    }
    // └ public function ┘

    // ┌ private function ┐
    void Game::ProcessInput() {
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            switch(event.type) {
                case SDL_QUIT:
                    running = false;
                    break;
            }
        }

        const Uint8* keyState = SDL_GetKeyboardState(NULL);

        // Player1
        mPlayer1PaddleDir = 0;
        if(keyState[SDL_SCANCODE_W]) {
            SDL_LogDebug(SDL_LOG_CATEGORY_INPUT , "W");
            mPlayer1PaddleDir -= 1;
        }

        if(keyState[SDL_SCANCODE_A]) {
            SDL_LogDebug(SDL_LOG_CATEGORY_INPUT , "A");
        }

        if(keyState[SDL_SCANCODE_S]) {
            SDL_LogDebug(SDL_LOG_CATEGORY_INPUT , "S");
            mPlayer1PaddleDir += 1;
        }

        if(keyState[SDL_SCANCODE_D]) {
            SDL_LogDebug(SDL_LOG_CATEGORY_INPUT , "D");
        }

        // Player2
        mPlayer2PaddleDir = 0;
        if(keyState[SDL_SCANCODE_I]) {
            SDL_LogDebug(SDL_LOG_CATEGORY_INPUT , "I");
            mPlayer2PaddleDir -= 1;
        }

        if(keyState[SDL_SCANCODE_K]) {
            SDL_LogDebug(SDL_LOG_CATEGORY_INPUT , "K");
            mPlayer2PaddleDir += 1;
        }

        if(keyState[SDL_SCANCODE_LCTRL] && keyState[SDL_SCANCODE_C]) {
            running = false;
            SDL_LogDebug(SDL_LOG_CATEGORY_INPUT , "quit(Ctrl + C)");
        }
    }
    void Game::UpdateGame() {

        // 物理演算の誤差軽減のため、高FPSに制限をかける(最低16msは待たせる) 1000/60 = 62.5FPS
        while(!SDL_TICKS_PASSED(SDL_GetTicks(), mPrevTick + 16));

        // 前フレームからの経過時間（秒）
        float deltaTime = (SDL_GetTicks() - mPrevTick) / 1000.0f;

        // 低FPSすぎる場合や、pause時にdeltaTimeが肥大しないように上限を20FPSにする
        // 1000/20 = 50ms, 50/1000 = 0.05
        if(0.05 <= deltaTime){
            deltaTime = 0.05;
        }

        // 次フレームのために更新
        mPrevTick = SDL_GetTicks();

        // deltaTimeが0.016 ~ 0.05になっているはず(62.5FPS ~ 20FPS)
        //SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "deltaTime: %.6f", deltaTime);

        // Player1のpaddle更新
        if(mPlayer1PaddleDir != 0){
            mPlayer1PaddlePos.y += mPlayer1PaddleDir * (INIT_HEIGHT / 1.5f) * deltaTime;

            // paddle境界補正
            if(
            mPlayer1PaddlePos.y < (WALL_THICKNESS + PADDLE_HEIGHT / 2.0f) 
            ) {
                mPlayer1PaddlePos.y = WALL_THICKNESS + PADDLE_HEIGHT / 2.0f;
            } else if (
                mPlayer1PaddlePos.y > (INIT_HEIGHT - WALL_THICKNESS - PADDLE_HEIGHT / 2.0f)
            ) {
                mPlayer1PaddlePos.y = INIT_HEIGHT - WALL_THICKNESS - PADDLE_HEIGHT / 2.0f;
            }
        }

        // Player2のpaddle更新
        if(mPlayer2PaddleDir != 0){
            mPlayer2PaddlePos.y += mPlayer2PaddleDir * (INIT_HEIGHT / 1.5f) * deltaTime;

            // paddle境界補正
            if(
            mPlayer2PaddlePos.y < (WALL_THICKNESS + PADDLE_HEIGHT / 2.0f) 
            ) {
                mPlayer2PaddlePos.y = WALL_THICKNESS + PADDLE_HEIGHT / 2.0f;
            } else if (
                mPlayer2PaddlePos.y > (INIT_HEIGHT - WALL_THICKNESS - PADDLE_HEIGHT / 2.0f)
            ) {
                mPlayer2PaddlePos.y = INIT_HEIGHT - WALL_THICKNESS - PADDLE_HEIGHT / 2.0f;
            }
        }

        // ball更新
        for(size_t i = 0; i < mBalls.size(); ++i){
            Ball& ball = mBalls[i];
            ball.pos.x += ball.velocity.x * deltaTime;
            ball.pos.y += ball.velocity.y * deltaTime;

            // 範囲外判定（範囲外に出たボールは削除）
            if(ball.pos.x < -BALL_WIDTH / 2.0f || ball.pos.x > INIT_WIDTH + BALL_WIDTH / 2.0f) {
                SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "ball removed");
                mBalls.erase(mBalls.begin() + i);
                --i;
                continue;
            }

            // Player1のpaddleとballの当たり判定
            if(
                std::fabs(mPlayer1PaddlePos.y - ball.pos.y) <= PADDLE_HEIGHT / 2.0f &&
                ball.pos.x >= mPlayer1PaddlePos.x &&
                ball.pos.x <= mPlayer1PaddlePos.x + PADDLE_WIDTH / 2.0f + BALL_WIDTH / 2.0f &&
                ball.velocity.x < 0
            ) {
                ball.velocity.x *= -1;
            }

            // Player2のpaddleとballの当たり判定
            if(
                std::fabs(mPlayer2PaddlePos.y - ball.pos.y) <= PADDLE_HEIGHT / 2.0f &&
                ball.pos.x <= mPlayer2PaddlePos.x &&
                ball.pos.x >= mPlayer2PaddlePos.x - PADDLE_WIDTH / 2.0f - BALL_WIDTH / 2.0f &&
                ball.velocity.x > 0
            ) {
                ball.velocity.x *= -1;
            }

            // ball下壁判定
            if(
                ball.velocity.y > 0 &&
                ball.pos.y > INIT_HEIGHT - WALL_THICKNESS - BALL_HEIGHT / 2.0f
            ) {
                ball.velocity.y *= -1;
            }

            // ball上壁判定
            if(
                ball.velocity.y < 0 &&
                ball.pos.y < WALL_THICKNESS + BALL_HEIGHT / 2.0f
            ) {
                ball.velocity.y *= -1;
            }
        }

        // 全てのボールがなくなったらgameover
        if(mBalls.empty()) {
            SDL_LogDebug(SDL_LOG_CATEGORY_APPLICATION, "game over - no balls remaining");
            running = false;
        }

    }
    void Game::GenerateOutPut() {
        // 背景色の設定(RGBA)
        SDL_SetRenderDrawColor(mRenderer, 0, 0, 255, 255);

        // 現在の色でバックバッファを塗りつぶす
        SDL_RenderClear(mRenderer);

        // 壁の色を設定
        SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 200);

        // 壁用構造体の用意
        SDL_Rect wall {
            0,
            0,
            INIT_WIDTH,
            WALL_THICKNESS
        };

        // 上の壁を描画
        SDL_RenderFillRect(mRenderer, &wall);

        // 下の壁を描画
        wall.x = 0;
        wall.y = INIT_HEIGHT - WALL_THICKNESS;
        wall.w = INIT_WIDTH;
        wall.h = WALL_THICKNESS;
        SDL_RenderFillRect(mRenderer, &wall);

        // Player1のパドルの描画
        SDL_Rect player1Paddle {
            static_cast<int>(mPlayer1PaddlePos.x - PADDLE_WIDTH/2.0f),
            static_cast<int>(mPlayer1PaddlePos.y - PADDLE_HEIGHT/2.0f),
            PADDLE_WIDTH,
            PADDLE_HEIGHT
        };

        SDL_RenderFillRect(mRenderer, &player1Paddle);

        // Player2のパドルの描画
        SDL_Rect player2Paddle {
            static_cast<int>(mPlayer2PaddlePos.x - PADDLE_WIDTH/2.0f),
            static_cast<int>(mPlayer2PaddlePos.y - PADDLE_HEIGHT/2.0f),
            PADDLE_WIDTH,
            PADDLE_HEIGHT
        };

        SDL_RenderFillRect(mRenderer, &player2Paddle);

        // ball描画
        for(const Ball& ball : mBalls){
            SDL_Rect renderBall {
                static_cast<int>(ball.pos.x - BALL_WIDTH/2.0f),
                static_cast<int>(ball.pos.y - BALL_HEIGHT/2.0f),
                BALL_WIDTH,
                BALL_HEIGHT
            };

            SDL_RenderFillRect(mRenderer, &renderBall);
        }

        // フロントバッファと交換して反映させる
        SDL_RenderPresent(mRenderer);
    }
    // └ private function ┘
}