//
// Created by Cardi on 2022/5/3.
//

#include "engine/gl/GL.h"
#include <stdexcept>
#include "list"
#include "sstream"
#include "engine/Debuger.h"
#include "engine/Application.h"
#include "engine/PlayerTime.h"
#include "engine/PlayerLoop.h"
#include "engine/gl/GLDrawBaseInternal.h"
#include "engine/gl/GLDrawBase.h"
#include "string"
#include "utilities/nameof.hpp"

using Engine::Debug::Log;
using Engine::Debug::LogLevel;

namespace Engine::PlayerTime
{
    double _targetFrameTime = 0;
    double _currentFixedTime = 0;
    int _fixedUpdateCounter = 0;
    double _frameTime = 0;
    double _updatingTime = 0;
    bool _skipUpdate = false;

    double _fixedDeltaTime;
    double _deltaTime;
    double _scaleFactor, _pScaleFactor;
    FrameID _frame = 0;

    void SetUpdateTime(double fixedDT, double dt)
    {
        Debug::Log( "Set Player Updating Time");

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

    static inline void ResetTimer()
    {
        _targetFrameTime = glfwGetTime();
        _currentFixedTime = _targetFrameTime;
        _frame = 0;
        _skipUpdate = false;
        _frameTime = 0;
        _fixedUpdateCounter = 0;
        _updatingTime = 0;
    }

    static inline void OnNewFrame()
    {
        _scaleFactor = _pScaleFactor;
    }

    double GetGameTimeFromStartup()
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

    double GetFixedUpdateTimer()
    {
        return _fixedDeltaTime;
    }

    double GetTargetUpdateTimer()
    {
        return _targetFrameTime;
    }

    int GetFixedUpdateCountLastFrame()
    {
        return _fixedUpdateCounter;
    }

    double GetFrameTimeLastFrame()
    {
        return _frameTime;
    }

    double GetUpdateTimeLastFrame()
    {
        return _updatingTime;
    }

    bool GetSkipUpdateLastFrame()
    {
        return _skipUpdate;
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

    static inline void DoUpdateEvent(PlayerLoopTiming timing)
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

    static inline void SingleFrameLoop()
    {
        using Engine::PlayerLoop::PlayerLoopTiming;
        using Engine::PlayerTime::_fixedDeltaTime;
        using Engine::PlayerTime::_deltaTime;
        using Engine::PlayerTime::_targetFrameTime;
        using Engine::PlayerTime::_currentFixedTime;
        using Engine::PlayerTime::_skipUpdate;
        using Engine::PlayerTime::_updatingTime;
        using Engine::PlayerTime::_frameTime;
        using Engine::PlayerTime::_fixedUpdateCounter;
        using Engine::PlayerTime::_frame;

        // Frame Init
        _targetFrameTime += _deltaTime;
        double frameStart = glfwGetTime();
        _frame++;

        // Frame Start
        DoUpdateEvent(PlayerLoopTiming::IDLE);

        Engine::PlayerTime::OnNewFrame();
        DoUpdateEvent(PlayerLoopTiming::PRE_NewFrame);
        DoUpdateEvent(PlayerLoopTiming::NewFrame);
        DoUpdateEvent(PlayerLoopTiming::POST_NewFrame);

        int c1 = 0;
        while (_currentFixedTime < _targetFrameTime)
        {
            c1++;
            // Fixed Update
            DoUpdateEvent(PlayerLoopTiming::PRE_FixedUpdate);
            DoUpdateEvent(PlayerLoopTiming::FixedUpdate);
            DoUpdateEvent(PlayerLoopTiming::POST_FixedUpdate);

            _currentFixedTime += _fixedDeltaTime;
        }

        bool sk = false;
        if (glfwGetTime() < _targetFrameTime)
        {
            // Update
            DoUpdateEvent(PlayerLoopTiming::PRE_Update);
            DoUpdateEvent(PlayerLoopTiming::Update);
            DoUpdateEvent(PlayerLoopTiming::POST_Update);

            if (glfwGetTime() < _targetFrameTime)
            {
                // Render
                DoUpdateEvent(PlayerLoopTiming::PRE_RenderUpdate);
                DoUpdateEvent(PlayerLoopTiming::RenderUpdate);
                DoUpdateEvent(PlayerLoopTiming::POST_RenderUpdate);
            } else sk = true;
        } else sk = true;

        // End Frame
        DoUpdateEvent(PlayerLoopTiming::PRE_EndFrame);
        DoUpdateEvent(PlayerLoopTiming::EndFrame);
        DoUpdateEvent(PlayerLoopTiming::POST_EndFrame);

        double fin = glfwGetTime();
        _updatingTime = fin - frameStart;

        while (fin < _targetFrameTime) fin = glfwGetTime();

        _frameTime = fin - frameStart;
        _skipUpdate = sk;
        _fixedUpdateCounter = c1;

        DoUpdateEvent(PlayerLoopTiming::BetweenFrame);
    }

}

namespace Engine::Application
{
    bool running = false;
    bool _exitNextFrame = false;
    bool _exitCode = 0;
    Setting* _setting;
    GLFWwindow* window;

    std::string appExecPath;
    std::vector<std::string> appComLin;

    Setting& GetSetting()
    {
        return *_setting;
    }

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
        using namespace Engine::Debug;
        _setting = setting;
        Log("-- Engine Start --");

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

        try
        {
            // Load Game Systems
            Log("-- Initialize Subsystems --");
            Engine::PlayerTime::SetUpdateTime(setting->Time_fixedDeltaTime, setting->Time_deltaTime);
            Engine::GL::DrawBaseInternal::GLBaseInitial();

            Log("Call Game Create Callback");
            if (setting->CreateGameEvent != nullptr) setting->CreateGameEvent();

            ///////////////////////////////////
            Log("-- Start Render Loop --");
            running = true;
            Engine::PlayerTime::ResetTimer();
            if (setting->Tool_FPSCounter)
            {
                Log(Debug::Debug, "Enable FPS Counter Tool");
                double timeCounter = PlayerTime::GetGameTimeFromStartup();
                PlayerTime::FrameID frameCounter = PlayerTime::GetCurrentFrameCount();

                while (!GL::DrawBase::ShouldCloseWindow() && !_exitNextFrame)
                {
                    Engine::PlayerLoop::SingleFrameLoop();
                    if (PlayerTime::GetGameTimeFromStartup() - timeCounter >= 1)
                    {
                        std::stringstream ss;
                        ss << "Frame " << PlayerTime::GetCurrentFrameCount() << " - FPS: " << PlayerTime::GetCurrentFrameCount() - frameCounter;
                        timeCounter = PlayerTime::GetGameTimeFromStartup();
                        frameCounter = PlayerTime::GetCurrentFrameCount();
                        if (PlayerTime::GetSkipUpdateLastFrame()) ss << " (Some frames has skipped.)";
                        Log(Debug::Debug, ss.str());
                    }
                }
            }
            else
            {
                while (!GL::DrawBase::ShouldCloseWindow() && !_exitNextFrame)
                {
                    Engine::PlayerLoop::SingleFrameLoop();
                }
            }
            Log("-- End Render Loop --");

            ///////////////////////////////////
            Log("Call Game Destroy Callback");
            if (setting->DestroyGameEvent != nullptr) setting->DestroyGameEvent();
        }
        catch (std::exception& e)
        {
            std::stringstream ss;
            ss << "Critical Error! " << e.what();
            Debug::Log(Error, ss.str());
        }

        Log("-- Terminate Subsystems --");
        Engine::GL::DrawBaseInternal::GLBaseTerminate();

        Log("-- Engine Close --");
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