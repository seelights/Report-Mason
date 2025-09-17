@echo off
echo 开始编译Poppler 24.08.0 for Qt 6.8.2...

cd poppler-24.08.0

echo 配置Poppler编译环境...
mkdir build
cd build

echo 运行CMake配置...
cmake .. ^
    -G "MinGW Makefiles" ^
    -DCMAKE_PREFIX_PATH="D:\Qt6\6.8.2\mingw_64" ^
    -DCMAKE_C_COMPILER="D:\Qt6\Tools\mingw1310_64\bin\gcc.exe" ^
    -DCMAKE_CXX_COMPILER="D:\Qt6\Tools\mingw1310_64\bin\g++.exe" ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DENABLE_QT6=ON ^
    -DENABLE_QT5=OFF ^
    -DENABLE_GLIB=ON ^
    -DENABLE_CPP=ON ^
    -DENABLE_UNSTABLE_API_ABI_HEADERS=ON ^
    -DCMAKE_INSTALL_PREFIX="D:\projects\ReportMason\poppler_build"

if %ERRORLEVEL% neq 0 (
    echo CMake配置失败！
    pause
    exit /b 1
)

echo 开始编译...
D:\Qt6\Tools\mingw1310_64\bin\mingw32-make.exe -j4

if %ERRORLEVEL% neq 0 (
    echo 编译失败！
    pause
    exit /b 1
)

echo 安装Poppler...
D:\Qt6\Tools\mingw1310_64\bin\mingw32-make.exe install

if %ERRORLEVEL% neq 0 (
    echo 安装失败！
    pause
    exit /b 1
)

echo Poppler编译完成！
echo 库文件位置: D:\projects\ReportMason\poppler_build
pause
