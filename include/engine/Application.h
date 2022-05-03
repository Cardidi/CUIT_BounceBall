//
// Created by Cardi on 2022/5/3.
//

#ifndef BOUNCEBALL_APPLICATION_H
#define BOUNCEBALL_APPLICATION_H
#include "string"
#include "vector"

namespace Engine::Application
{
    typedef void (*ApplicationEventHook)();

    struct Setting
    {
        double Time_fixedDeltaTime = 0.002;
        double Time_deltaTime = 0.017;
        std::string Window_Init_GameTitle = "OpenGL Window";
        bool Window_KeepInitRatio = true;
        unsigned int Window_Init_Width = 800;
        unsigned int Window_Init_Height = 600;

        ApplicationEventHook CreateGameEvent = nullptr;
        ApplicationEventHook DestroyGameEvent = nullptr;
    };

    void Exit();
    void Exit(int code);
    int Boot(struct Setting* setting, int argc, char** argv);
    bool IsRunning();
    std::string GetAppExecutablePath();
    std::vector<std::string> GetCommandLine();
}

#endif //BOUNCEBALL_APPLICATION_H
