//
// Created by Jens Kromdijk on 22/05/25.
//

#ifndef BAR_CHART_H
#define BAR_CHART_H

#include <string>

typedef struct Bar
{
    float pos{0.0f}; // y position
    int numPapers{0}; // number of papers found in cluster
    int clusterIdx{0}; // cluster index
    std::string name; // cluster label
    int totalPapers{0}; // total number of papers in cluster
    int numIncluded{0}; // number of included papers found in cluster
    int numNotIncluded{0}; // number of not included papers found in cluster
} Bar;

#endif // BAR_CHART_H
