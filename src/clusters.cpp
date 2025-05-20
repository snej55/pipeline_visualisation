#include "clusters.h"

// convex hull library
#define CONVHULL_3D_ENABLE
#include <convhull_3d.h>

Clusters::ClusterRenderer::ClusterRenderer()
{
    m_clusters.resize(5);
}

Clusters::ClusterRenderer::~ClusterRenderer()
{
    free();
}

// load convex hull for each cluster
int Clusters::ClusterRenderer::generateClusters(const std::vector<std::map<int, Cluster>>& clusters, const float scale)
{
    m_loaded = false;
    // iterate over each cluster depth
    for (std::size_t i{0}; i < clusters.size(); ++i)
    {
        // go through each cluster
        for (const auto&[idx, cluster] : clusters[i])
        {
            // cluster rendering data
            ClusterData clusterData{};
            ConvexHull* hull {new ConvexHull};
            clusterData.hull = hull;

            // allocate memory for papers
            auto* chVertices = new ch_vertex[cluster.num_papers];
            hull->numVertices = cluster.num_papers;

            // get vertices from cluster
            for (std::size_t v{0}; v < cluster.num_papers; ++v)
            {
                chVertices[v].x = cluster.vertices[v].x * scale;
                chVertices[v].y = cluster.vertices[v].y * scale;
                chVertices[v].z = cluster.vertices[v].z * scale;
            }

            // build convex hull
            convhull_3d_build(chVertices, hull->numVertices, &hull->faceIndices, &hull->numFaces);
            // export for testing
            std::stringstream filename;
            filename << "../data/cluster_models/cluster_" << i + 2 << "_" << idx;
            std::string name {filename.str()};
            convhull_3d_export_obj(chVertices, hull->numVertices, hull->faceIndices, hull->numFaces, 1, (char*)name.c_str());
            // success check
            if (hull->faceIndices == nullptr)
            {
                std::cout << "ERROR::CLUSTER_RENDERER::INIT: Failed to create convex hull!" << std::endl;
                return -1;
            }

            // generate vector of floats to copy into vertex buffer
            std::vector<float> vertices;
            for (std::size_t j{0}; j < hull->numVertices; ++j)
            {
                // CH_FLOAT -> float
                vertices.push_back(static_cast<float>(chVertices[j].x));
                vertices.push_back(static_cast<float>(chVertices[j].y));
                vertices.push_back(static_cast<float>(chVertices[j].z));
            }

            std::vector<unsigned int> indices;
            for (std::size_t f{0}; f < hull->numFaces; ++f)
            {
                indices.push_back(hull->faceIndices[f]);
            }

            m_clusters[i].insert(std::pair{idx, clusterData});
            delete[] chVertices;
        }
        std::cout << "Loaded cluster level " << i + 2 << std::endl;
    }

    // all good
    m_loaded = true;
    std::cout << "Clusters loaded!" << std::endl;
    return 0;
}

// free memory from cluster data
void Clusters::ClusterRenderer::free()
{
    if (m_loaded)
    {
        for (std::size_t i{0}; i < m_clusters.size(); ++i)
        {
            for (const auto&[idx, cluster] : m_clusters[i])
            {
                delete[] cluster.hull->faceIndices;
                delete cluster.hull;
            }
            m_clusters[i].clear();
        }
        m_clusters.clear();
        m_loaded = false;
    }
}

Clusters::ClusterData* Clusters::ClusterRenderer::getClusterData(int depth, int idx)
{
    return &m_clusters[depth - 2][idx];
}

void Clusters::ClusterRenderer::renderCluster(const Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::vec3& color, int depth, int idx)
{
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setMat4("model", glm::mat4(1.0f));
    shader.setVec3("color", color);

    // get cluster at index idx from depth level
    // const ClusterData* cluster {getClusterData(depth, idx)};
    // glDrawElements(GL_TRIANGLES, cluster->hull->numFaces, GL_UNSIGNED_INT, nullptr);
}