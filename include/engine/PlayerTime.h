//
// Created by Cardi on 2022/5/3.
//

#ifndef BOUNCEBALL_PLAYERTIME_H
#define BOUNCEBALL_PLAYERTIME_H

namespace Engine::PlayerTime
{
    typedef unsigned long int FrameID;

    double GetGameStartupTime();
    double GetDeltaTime(bool unscaled);
    double GetFixedDeltaTime(bool unscaled);
    double GetTimeScale();
    void SetTimeScale(double scaleFactor);
    FrameID GetCurrentFrameCount();
}

#endif //BOUNCEBALL_PLAYERTIME_H
