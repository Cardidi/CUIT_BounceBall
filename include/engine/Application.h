//
// Created by Cardi on 2022/5/3.
//

#ifndef BOUNCEBALL_APPLICATION_H
#define BOUNCEBALL_APPLICATION_H
#include "string"
#include "vector"

namespace Engine::Application
{
    void Exit();
    void Exit(int code);
    std::string GetAppExecutablePath();
    std::vector<std::string> GetCommandLine();
    std::string GetAppStoragePath();
}

#endif //BOUNCEBALL_APPLICATION_H
