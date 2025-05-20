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
int Clusters::ClusterRenderer::init(const std::vector<std::map<int, Cluster>>& clusters)
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
                chVertices[v].x = cluster.vertices[v].x;
                chVertices[v].y = cluster.vertices[v].y;
                chVertices[v].z = cluster.vertices[v].z;
            }

            // build convex hull
            convhull_3d_build(chVertices, hull->numVertices, &hull->faceIndices, &hull->numFaces);
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

            {
                // generate VAO, VBO & EBO
                unsigned int VAO, VBO, EBO;
                glGenVertexArrays(1, &VAO);
                glGenBuffers(1, &VBO);
                glGenBuffers(1, &EBO);
                glBindVertexArray(VAO);
                // buffer vertex data
                glBindBuffer(GL_ARRAY_BUFFER, VBO);
                glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(), GL_STATIC_DRAW);
                // buffer indices data
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), indices.data(), GL_STATIC_DRAW);

                glEnableVertexAttribArray(0);
                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));

                // unbind
                // don't unbind EBO, it's stored in VAO
                glBindBuffer(GL_ARRAY_BUFFER, 0);
                glBindVertexArray(0);

                clusterData.VAO = VAO;
                clusterData.VBO = VBO;
                clusterData.EBO = EBO;
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
                glDeleteVertexArrays(1, &cluster.VAO);
                glDeleteBuffers(1, &cluster.VBO);
                glDeleteBuffers(1, &cluster.EBO);
            }
            m_clusters[i].clear();
        }
        m_clusters.clear();
        m_loaded = false;
    }
}

Clusters::ClusterData* Clusters::ClusterRenderer::getClusterData(int depth, int idx)
{
    return &m_clusters[depth][idx];
}

void Clusters::ClusterRenderer::renderCluster(const Shader& shader, const glm::mat4& projection, const glm::mat4& view, int depth, int idx)
{
    shader.use();
    shader.setMat4("projection", projection);
    shader.setMat4("view", view);
    shader.setMat4("model", glm::mat4(1.0f));

    // get cluster at index idx from depth level
    const ClusterData* cluster {getClusterData(depth, idx)};
    
    glBindVertexArray(cluster->VAO);
    glDrawElements(GL_TRIANGLES, cluster->hull->numFaces * 3, GL_UNSIGNED_INT, nullptr);
}