//
// Created by Cardi on 2022/5/3.
//

#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <stdexcept>
#include "list"
#include "sstream"
#include "engine/Debuger.h"
#include "engine/Application.h"
#include "engine/PlayerTime.h"
#include "engine/PlayerLoop.h"
#include "string"
#include "utilities/nameof.hpp"

using Engine::Debug::Log;
using Engine::Debug::LogLevel;

namespace Engine::PlayerTime
{
    double _fixedDeltaTime;
    double _deltaTime;
    double _scaleFactor, _pScaleFactor;
    FrameID _frame = 0;

    void Init(double fixedDT, double dt)
    {
        Debug::Log(LogLevel::Debug, "Init Player Time");

        if (fixedDT <= 0)
            throw std::out_of_range("fixedDT should more than 0.0!");
        if (dt <= 0)
            throw std::out_of_range("dt should more than 0.0!");
        if (fixedDT > dt)
            throw std::out_of_range("dt should more than fixedDT!");

        _fixedDeltaTime = fixedDT;
        _deltaTime = dt;
        _scaleFactor = _pScaleFactor = 1.0f;
    }

    void OnNewFrame()
    {
        _scaleFactor = _pScaleFactor;
        _frame++;
    }

    double GetGameStartupTime()
    {
        return glfwGetTime();
    }

    double GetDeltaTime(bool unscaled)
    {
        if (unscaled) return _deltaTime;
        else return _deltaTime * _scaleFactor;
    }

    double GetFixedDeltaTime(bool unscaled)
    {
        if (unscaled) return _fixedDeltaTime;
        else return _fixedDeltaTime * _scaleFactor;
    }

    double GetTimeScale()
    {
        return _scaleFactor;
    }

    void SetTimeScale(double scaleFactor)
    {
        if (_scaleFactor <= 0)
            throw std::out_of_range("scaleFactor should more than 0.0!");
        _pScaleFactor = scaleFactor;
    }

    FrameID GetCurrentFrameCount()
    {
        return _frame;
    }
}

namespace Engine::PlayerLoop
{
    PlayerLoopTiming _timing = PlayerLoopTiming::IDLE;
    std::list<PlayerLoopCallback>* _callback = new std::list<PlayerLoopCallback>;

    PlayerLoopTiming GetCurrentTiming()
    {
        return _timing;
    }

    bool AddCallback(PlayerLoopCallback callback)
    {
        auto exist = std::binary_search(_callback->begin(), _callback->end(), callback);
        if (exist) return false;

        _callback->push_front(callback);
        return true;
    }

    bool RemoveCallback(PlayerLoopCallback callback)
    {
        auto result = std::remove(_callback->begin(), _callback->end(), callback);
        if (result == _callback->end()) return false;
        return true;
    }

    void DoUpdateEvent(PlayerLoopTiming timing)
    {
        _timing = timing;
        for (PlayerLoopCallback callback : *_callback)
        {
            try
            {
                callback(timing);
            }
            catch (std::exception e)
            {
                std::stringstream ss;
                ss << "Meet error in frame " << Engine::PlayerTime::_frame << " on timing " << NAMEOF_ENUM(timing);
                Debug::Log(Debug::Warning, ss.str());
                Debug::Log(e);
            }
        }
    }

    void SingleFrameLoop()
    {
        using Engine::PlayerTime::_fixedDeltaTime;
        using Engine::PlayerTime::_deltaTime;

        double latestUpdate = glfwGetTime() + _deltaTime;
        double skipUpdate = latestUpdate + _deltaTime;

        // Frame Init
        DoUpdateEvent(PlayerLoopTiming::IDLE);

        Engine::PlayerTime::OnNewFrame();
        DoUpdateEvent(PlayerLoopTiming::PRE_NewFrame);
        DoUpdateEvent(PlayerLoopTiming::NewFrame);
        DoUpdateEvent(PlayerLoopTiming::POST_NewFrame);

        while (true)
        {
            // Fixed Update
            double latestFixedUpdate = glfwGetTime() + _fixedDeltaTime;

            DoUpdateEvent(PlayerLoopTiming::PRE_FixedUpdate);
            DoUpdateEvent(PlayerLoopTiming::FixedUpdate);
            DoUpdateEvent(PlayerLoopTiming::POST_FixedUpdate);

            while (glfwGetTime() < latestFixedUpdate); // Wait for end of fixed update
            if (glfwGetTime() >= latestUpdate) break; // If enter update
        }

        bool skiped = false;
        if (glfwGetTime() < skipUpdate)
        {
            // Update
            DoUpdateEvent(PlayerLoopTiming::PRE_Update);
            DoUpdateEvent(PlayerLoopTiming::Update);
            DoUpdateEvent(PlayerLoopTiming::POST_Update);

            if (glfwGetTime() < skipUpdate)
            {
                // Render
                DoUpdateEvent(PlayerLoopTiming::PRE_RenderUpdate);
                DoUpdateEvent(PlayerLoopTiming::RenderUpdate);
                DoUpdateEvent(PlayerLoopTiming::POST_RenderUpdate);
            } else skiped = true;
        } else skiped = true;

        if (skiped)
        {
            std::stringstream ss;
            ss << "Frame " << PlayerTime::GetCurrentFrameCount() << " Update was skipped";
            Debug::Log(Debug::Debug, ss.str());
        }

        // End Frame
        DoUpdateEvent(PlayerLoopTiming::PRE_EndFrame);
        DoUpdateEvent(PlayerLoopTiming::EndFrame);
        DoUpdateEvent(PlayerLoopTiming::POST_EndFrame);

        DoUpdateEvent(PlayerLoopTiming::BetweenFrame);
    }

}

namespace Engine::Application
{
    bool running = false;
    bool _exitNextFrame = false;
    bool _exitCode = 0;
    std::string appExecPath;
    std::vector<std::string> appComLin;

    void Exit()
    {
        _exitNextFrame = true;
    }

    void Exit(int code)
    {
        _exitNextFrame = true;
        _exitCode = code;
    }

    int Boot(struct Setting* setting, int argc, char** argv)
    {
        using Engine::Debug::Log;
        using Engine::Debug::LogLevel;
        Log("Engine Start");

        // Parse command line
        Log("Read Command Line");
        appExecPath = std::string(argv[0]);
        if (argc > 1)
        {
            appComLin = std::vector<std::string>(argc - 1);
            for (int i = 1; i < argc; ++i) {
                appComLin[i] = std::string (argv[i]);
            }
        } else
        {
            appComLin = std::vector<std::string>(0);
        }

        // Initialize OpenGL
        Log("Initialize GLFW");
        glfwInit();

        Log(LogLevel::Debug, "Initialize Window");
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        GLFWwindow* window = glfwCreateWindow
                (setting->Window_Init_Width,setting->Window_Init_Height,
                 setting->Window_Init_GameTitle.c_str(),NULL, NULL);
        if (window == NULL)
        {
            Log(LogLevel::Error, "Failed to create window!");
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);

        Log(LogLevel::Debug, "Initialize OpenGL Loader");
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            Log(LogLevel::Error, "Failed to initialize GLAD");
            glfwTerminate();
            return -1;
        }

        // Load Game Systems
        Log("Initialize Game Subsystems");
        Engine::PlayerTime::Init(setting->Time_fixedDeltaTime, setting->Time_deltaTime);

        running = true;
        Log("Call Game Create Callback");
        if (setting->CreateGameEvent != nullptr) setting->CreateGameEvent();

        Log("-- Start Render Loop --");
        while (!glfwWindowShouldClose(window) && !_exitNextFrame)
        {
            glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            Engine::PlayerLoop::SingleFrameLoop();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        Log("-- End Render Loop --");


        Log("Call Game Destroy Callback");
        if (setting->DestroyGameEvent != nullptr) setting->DestroyGameEvent();

        Log("Terminate GLFW");
        glfwTerminate();
        running = false;
        return _exitCode;
    }

    bool IsRunning()
    {
        return running;
    }

    std::string GetAppExecutablePath()
    {
        return appExecPath;
    }

    std::vector<std::string> GetCommandLine()
    {
        return appComLin;
    }
}