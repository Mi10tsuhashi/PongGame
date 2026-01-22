# PongGame

https://qiita.com/Mi10tsuhashi/items/2bd844f74b3944e54774

vcpkg, CMake, VSCode, MSbuild構成のサンプルプロジェクトです。
SDL2 を使用した簡単な Pong ゲームを実装しています。
ゲーム部分は[ゲームプログラミング C++](https://www.amazon.co.jp/dp/4798157619)の第一章を少し改変したものです。

## 操作方法

- Player1（左）: W / S キー
- Player2（右）: I / K キー
- 終了: ウィンドウを閉じる または Ctrl+C

## 前提
- [Visual Studio 18 2026](https://download.visualstudio.microsoft.com/download/pr/1dac374e-2701-4b9e-b5b6-4438f957c242/6ced20b44c7ab087b6124f711938de5b64ca147538b811a70c0b24c406ee76e1/vs_BuildTools.exe)
- [vcpkg](https://github.com/microsoft/vcpkg.git)
- CMake 3.19
- VSCode + 拡張機能
  - C/C++ Extension Pack
  - CMake Tools

`VCPKG_ROOT`が設定されていること

## ビルド

1. git clone

2. buildディレクトリの作成

3. `CMakeUserPresets.example.json` を参考に`CMakeUserPresets.json` を作成、
`VCPKG_ROOT`を指定する

4. VsDevCmd.batが適用されたコマンドプロンプト/PowershellからVScode起動

5. コマンドパレットから：
   - `CMake: Select Configure Preset` で `debug/release` を選択
   - `CMake: Build` でビルド

## 起動
`\build\Debug\PongGame.exe`
