// header file with necessary components to render clusters
#ifndef CLUSTERS_H
#define CLUSTERS_H

#include <glad/glad.h>

#define CONVHULL_3D_ENABLE
#include "convhull_3d.h"

#include "paper_loader.h"

// namespace for rendering clusters
namespace Clusters
{
    struct ConvexHull
    {
        ch_vertex* vertices;
        int numVertices;
        int* faceIndices{nullptr};
        int numFaces;
    };

    struct ClusterData
    {
        ConvexHull* hull{nullptr};
        unsigned int VBO{0};
        unsigned int EBO{0};
        unsigned int VAO{0};
    };

    // loads convex hull for clusters and generates EBO, VBO & VAO
    class ClusterRenderer
    {
    public:
        ClusterRenderer();
        ~ClusterRenderer();
    
        int init(const std::vector<std::map<int, Cluster>>& clusters);
        void free();
    
    private:
        // flag to know if we need to free or not
        bool m_loaded{false};
        // contains cluster data for rendering
        std::vector<std::map<int, ClusterData>> m_clusters{};
    };
};

#endif