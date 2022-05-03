//
// Created by Cardi on 2022/5/3.
//

#include "engine/Debuger.h"
#include "iostream"
#include "time.h"

namespace Engine::Debug
{
    static LogLevelMask _mask = 15;

    void SetDebugLevelMask(LogLevelMask mask)
    {
        _mask = mask;
    }

    bool HasDebugLevel(LogLevel level)
    {
        return (level & _mask) != 0;
    }

    void Log(const char* text)
    {
        Log(Info, text);
    }

    void Log(LogLevel level, const char* text)
    {
        LogLevel l = (LogLevel) (_mask & level);
        char levelChar = 0;
        time_t now;
        time(&now);

        switch (l)
        {
            default:
                return;

            case LogLevel::Debug:
                levelChar = 'D';
                break;

            case LogLevel::Info:
                levelChar = 'I';
                break;

            case LogLevel::Warning:
                levelChar = 'W';
                break;

            case LogLevel::Error:
                levelChar = 'E';
                break;
        }

        char buf[80];
        tm *ltm = localtime(&now);
        strftime(buf, 80, "%H:%M:%S", ltm);
        std::cout << buf << " | " << levelChar << " : " << text << std::endl;
    }
}