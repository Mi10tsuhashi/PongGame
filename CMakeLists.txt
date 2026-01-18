cmake_minimum_required(VERSION 3.19)
project(PongGame LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 静的ランタイムライブラリを使用（vcpkgのx64-windows-staticと一致させる）
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# MSVCでUTF-8ソースファイルを正しく認識させる
if(MSVC)
    add_compile_options(/utf-8)
endif()

# SDL2
find_package(SDL2 CONFIG REQUIRED)

# src/hoge配下のcppを再帰的に収集
file(GLOB_RECURSE GAME_SOURCES src/game/*.cpp)
# ソースファイル
set(SOURCES
    src/main.cpp
    ${GAME_SOURCES}
)

# 実行可能ファイルの作成
# リリースビルド時のみWIN32フラグでコンソールウィンドウを非表示にする
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(${PROJECT_NAME} WIN32 ${SOURCES})
else()
    add_executable(${PROJECT_NAME} ${SOURCES})
endif()

# SDL2のリンク
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    $<TARGET_NAME_IF_EXISTS:SDL2::SDL2main>
    $<IF:$<TARGET_EXISTS:SDL2::SDL2>,SDL2::SDL2,SDL2::SDL2-static>
)