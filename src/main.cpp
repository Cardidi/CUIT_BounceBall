#include "main.h"

int main(int argc, char** argv)
{
    Engine::Application::Setting setting;
    setting.Window_Init_GameTitle = "test";

    return Engine::Application::Boot(&setting, argc, argv);
}