//
// Created by Cardi on 2022/5/3.
//

#ifndef BOUNCEBALL_PLAYERLOOP_H
#define BOUNCEBALL_PLAYERLOOP_H

namespace Engine::PlayerLoop
{
    enum class PlayerLoopTiming
    {
        IDLE = 0,

        PRE_NewFrame = 1,
        NewFrame = 2,
        POST_NewFrame = 4,

        PRE_FixedUpdate = 8,
        FixedUpdate = 16,
        POST_FixedUpdate = 32,

        PRE_Update = 64,
        Update = 128,
        POST_Update = 256,

        PRE_RenderUpdate = 512,
        RenderUpdate = 1024,
        POST_RenderUpdate = 2048,

        PRE_EndFrame = 4096,
        EndFrame = 8192,
        POST_EndFrame = 16384,

        BetweenFrame = 32768
    };

    typedef void (*PlayerLoopCallback)(PlayerLoopTiming timing);

    PlayerLoopTiming GetCurrentTiming();
    bool AddCallback(PlayerLoopCallback callback);
    bool RemoveCallback(PlayerLoopCallback callback);
}

#endif //BOUNCEBALL_PLAYERLOOP_H
