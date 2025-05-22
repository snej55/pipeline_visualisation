//
// Created by Jens Kromdijk on 22/05/25.
//

#ifndef BAR_CHART_H
#define BAR_CHART_H

#include <string>

typedef struct Bar
{
    float pos{0.0f}; // y position
    int numPapers{0};
    int clusterIdx{0};
    std::string name;
    int totalPapers{0};
} Bar;

#endif // BAR_CHART_H
