//
// Created by Cardi on 2022/5/4.
//
#include <exception>
#include "engine/gl/GL.h"
#include "engine/gl/GLDrawBase.h"
#include "engine/gl/GLDrawBaseInternal.h"
#include "engine/Debuger.h"
#include "engine/Application.h"
#include "engine/PlayerTime.h"

namespace Engine::GL::DrawBase
{
    GLFWwindow* window;

    GLFWwindow* GetWindow()
    {
        return window;
    }

    bool ShouldCloseWindow()
    {
        return glfwWindowShouldClose(window);
    }
}

namespace Engine::GL::DrawBaseInternal
{
    using namespace Engine::Debug;
    using namespace Engine::GL::DrawBase;
    bool _inited = false;

    void GLBaseUpdatingFunction(PlayerLoop::PlayerLoopTiming timing)
    {
        using namespace Engine::PlayerLoop;
        using namespace Engine::PlayerTime;

        if (timing == PlayerLoop::PlayerLoopTiming::NewFrame)
        {
            float delta = PlayerTime::GetTargetUpdateTimer() - (int) PlayerTime::GetTargetUpdateTimer();
            glClearColor(delta, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        if (timing == PlayerLoop::PlayerLoopTiming::PRE_Update)
        {
            glfwPollEvents();
        }

        if (timing == PlayerLoop::PlayerLoopTiming::BetweenFrame)
        {
            glfwSwapBuffers(window);
        }
    }

    void GLBaseInitial()
    {
        if (_inited) return;
        Application::Setting& setting = Application::GetSetting();

        // Initialize OpenGL
        Log("Initialize GLFW");
        glfwInit();

        Log(LogLevel::Debug, "Initialize Window");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        window = glfwCreateWindow
                (setting.Window_Init_Width,setting.Window_Init_Height,
                 setting.Window_Init_GameTitle.c_str(),NULL, NULL);
        if (window == NULL)
        {
            Log(LogLevel::Error, "Failed to create window!");
            glfwTerminate();
            throw std::exception();
        }
        glfwMakeContextCurrent(window);

        Log(LogLevel::Debug, "Initialize OpenGL Loader");
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            Log(LogLevel::Error, "Failed to initialize GLAD");
            glfwTerminate();
            throw std::exception();
        }
        Engine::PlayerLoop::AddCallback(GLBaseUpdatingFunction);

        _inited = true;
    }

    void GLBaseTerminate()
    {
        if (_inited)
        {
            Log("Terminate GLFW");
            Engine::PlayerLoop::RemoveCallback(GLBaseUpdatingFunction);
            glfwTerminate();
        }
    }
}