#include "main.h"
#include <iostream>

int main(int argc, char** argv)
{
    using Engine::Debug::LogLevel;
    std::cout << "Ca2didi Product 2022 : Bounce Ball (using OpenGL and Box2D)" << std::endl;
    std::cout << "Applying for CUIT Work" << std::endl << std::endl;

    // Engine::Debug::SetDebugLevelMask(LogLevel::All ^ LogLevel::Debug);
    Engine::Application::Setting setting;
    setting.Window_Init_GameTitle = "test";

    return Engine::Application::Boot(&setting, argc, argv);
}