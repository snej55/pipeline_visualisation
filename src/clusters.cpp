#include "clusters.h"

Clusters::ClusterRenderer::ClusterRenderer()
{
    m_clusters.resize(5);
}

Clusters::ClusterRenderer::~ClusterRenderer()
{
    free();
}

void Clusters::ClusterRenderer::init(const std::vector<std::map<int, Cluster>>& clusters, const std::vector<Paper>& papers)
{
    for (std::size_t i{0}; i < clusters.size(); ++i)
    {

    }
}