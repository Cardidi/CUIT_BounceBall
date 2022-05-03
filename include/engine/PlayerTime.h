//
// Created by Cardi on 2022/5/3.
//

#ifndef BOUNCEBALL_PLAYERTIME_H
#define BOUNCEBALL_PLAYERTIME_H

namespace Engine::PlayerTime
{
    typedef unsigned long int FrameID;

    double GetGameTimeFromStartup();
    double GetDeltaTime(bool unscaled);
    double GetFixedDeltaTime(bool unscaled);
    double GetTimeScale();
    void SetTimeScale(double scaleFactor);

    FrameID GetCurrentFrameCount();
    double GetFixedUpdateTimer();
    double GetTargetUpdateTimer();

    int GetFixedUpdateCountLastFrame();
    double GetFrameTimeLastFrame();
    double GetUpdateTimeLastFrame();
    bool GetSkipUpdateLastFrame();
}

#endif //BOUNCEBALL_PLAYERTIME_H
