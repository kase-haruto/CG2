@echo off
setlocal

rem クラス名をユーザーに入力させる
set /p CLASSNAME=クラス名を入力してください: 

rem クラス名が空でないかチェック
if "%CLASSNAME%"=="" (
    echo クラス名が入力されていません。
    pause
    exit /b
)

rem 小文字のクラス名を生成
set LOWERCLASSNAME=%CLASSNAME%
for %%A in (A B C D E F G H I J K L M N O P Q R S T U V W X Y Z) do (
    set LOWERCLASSNAME=!LOWERCLASSNAME:%%A=%%A!
)

rem 現在のディレクトリを取得
set SCRIPT_DIR=%~dp0

rem .h ファイルの作成
(
    echo #pragma once
    echo.
    echo class %CLASSNAME% {
    echo public:
    echo     %CLASSNAME%();
    echo     ~%CLASSNAME%();
    echo private:
    echo };
) > "%SCRIPT_DIR%%CLASSNAME%.h"

rem .cpp ファイルの作成
(
    echo #include "%LOWERCLASSNAME%.h"
    echo.
    echo %CLASSNAME%::%CLASSNAME%() {
    echo     // コンストラクタの実装
    echo }
    echo.
    echo %CLASSNAME%::~%CLASSNAME%() {
    echo     // デストラクタの実装
    echo }
) > "%SCRIPT_DIR%%CLASSNAME%.cpp"

echo %CLASSNAME%.h および %CLASSNAME%.cpp を作成しました。
pause
