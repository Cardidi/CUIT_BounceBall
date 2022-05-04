//
// Created by Cardi on 2022/5/4.
//

#ifndef BOUNCEBALL_GLDRAWBASE_H
#define BOUNCEBALL_GLDRAWBASE_H
#include "GL.h"

namespace Engine::GL::DrawBase
{
    GLFWwindow* GetWindow();
    bool ShouldCloseWindow();
}


#endif //BOUNCEBALL_GLDRAWBASE_H
