//
// Created by jenskromdijk on 07/04/25.
//

#ifndef UTIL_H
#define UTIL_H

#include <cstdlib>

namespace Util
{
    inline double random()
    {
        return std::rand() / RAND_MAX;
    }
}

#endif // UTIL_H
