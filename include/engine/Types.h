//
// Created by Cardi on 2022/5/3.
//

#ifndef BOUNCEBALL_TYPES_H
#define BOUNCEBALL_TYPES_H

// Colors
namespace Engine::Type
{
    typedef struct color {
    public:
        float r = 0, g = 0, b = 0, a = 1;
        explicit color() = default;

        explicit color(float r)
        {
            this->r = r;
        }

        explicit color(float r, float g)
        {
            this->r = r;
            this->g = g;
        }

        explicit color(float r, float g, float b)
        {
            this->r = r;
            this->g = g;
            this->b = b;
        }

        explicit color(float r, float , float b, float a)
        {
            this->r = r;
            this->g = g;
            this->b = b;
            this->a = a;
        }
    } Color;

    const Color Color_Red(1, 0 , 0);
    const Color Color_Green(0, 1, 0);
    const Color Color_Blue(0, 0, 1);
    const Color Color_Black(0, 0, 0);
    const Color Color_White(1, 1, 1);
}


#endif //BOUNCEBALL_TYPES_H
