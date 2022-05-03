//
// Created by Cardi on 2022/5/3.
//

#ifndef BOUNCEBALL_DEBUGER_H
#define BOUNCEBALL_DEBUGER_H

namespace Engine::Debug
{
    typedef unsigned char LogLevelMask;

    enum LogLevel
    {
        // Normal log information for debugging
        Debug = 1,

        // Normal log information in product
        Info = 2,

        // Something unusual but is not necessary to handle with.
        Warning = 4,

        // Serious thing that may cause application quit.
        Error = 8
    };

    void SetDebugLevelMask(LogLevelMask mask);
    bool HasDebugLevel(LogLevel level);
    void Log(const char* text);
    void Log(LogLevel level, const char* text);
}

#endif //BOUNCEBALL_DEBUGER_H
