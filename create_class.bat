@echo off
setlocal EnableDelayedExpansion

:: 現在のバッチファイルのパスを取得
set script_dir=%~dp0
set script_dir=%script_dir:~0,-1%  :: 最後のバックスラッシュを削除

set /p classname=Enter class name: 
set /p filepath=Enter the path where you want to create the file (e.g. C:\programming\CG2\CG2\Engine\graphics\):

:: if folder is not found, make it.
if not exist "%filepath%" (
    mkdir "%filepath%"
)

:: 特定のフォルダ名を除いたパスを取得する
set "include_path=%filepath%"

:: フォルダ名を削除する処理
set include_path=%include_path:*Engine\=%
set include_path=%include_path:*Game\=%
set include_path=%include_path:*externals\=%
set include_path=%include_path:*lib\=%

:: 最初のバックスラッシュまたはスラッシュを削除
if "!include_path:~0,1!"=="\" set include_path=!include_path:~1!
if "!include_path:~0,1!"=="/" set include_path=!include_path:~1!

:: バックスラッシュをスラッシュに変換
set include_path=!include_path:\=/!

:: .h createFile
echo #pragma once > "%filepath%\%classname%.h"
echo class %classname% { >> "%filepath%\%classname%.h"
echo public: >> "%filepath%\%classname%.h"
echo     %classname%(); >> "%filepath%\%classname%.h"
echo     ~%classname%(); >> "%filepath%\%classname%.h"
echo private: >> "%filepath%\%classname%.h"
echo }; >> "%filepath%\%classname%.h"

:: .cpp createFile
:: include path based on the extracted subdirectory relative to the root directories
echo #include "%include_path%/%classname%.h" > "%filepath%\%classname%.cpp"
echo %classname%::%classname%() { >> "%filepath%\%classname%.cpp"
echo } >> "%filepath%\%classname%.cpp"

echo %classname%::~%classname%() { >> "%filepath%\%classname%.cpp"
echo } >> "%filepath%\%classname%.cpp"

echo "%filepath%\%classname%.h" と "%filepath%\%classname%.cpp" を作成しました。

endlocal
