//
// Created by Ang Zhi Wei on 11/2/2026.
//

#ifndef DOODLE_UTILS_H
#define DOODLE_UTILS_H

#include <numeric>
// Very simple lerp, why no c++20 :(
namespace Utils{
    float Lerp(float start, float end, float t){
        return start + (end - start) * t;
    }
}
#endif //DOODLE_UTILS_H
