#include "clusters.h"

Clusters::ClusterRenderer::ClusterRenderer()
{
    m_clusters.resize(5);
}

Clusters::ClusterRenderer::~ClusterRenderer()
{
    free();
}

int Clusters::ClusterRenderer::init(const std::vector<std::map<int, Cluster>>& clusters)
{
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
            hull->vertices = new ch_vertex[cluster.num_papers];
            hull->numVertices = cluster.num_papers;

            // get vertices from cluster
            for (std::size_t v{0}; v < cluster.num_papers; ++v)
            {
                hull->vertices[v].x = cluster.vertices[v].x;
                hull->vertices[v].y = cluster.vertices[v].y;
                hull->vertices[v].z = cluster.vertices[v].z;
            }

            // build convex hull
            convhull_3d_build(hull->vertices, hull->numVertices, &hull->faceIndices, &hull->numFaces);
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
                vertices.push_back(static_cast<float>(hull->vertices[j].x));
                vertices.push_back(static_cast<float>(hull->vertices[j].y));
                vertices.push_back(static_cast<float>(hull->vertices[j].z));
            }

            std::vector<unsigned int> indices;
            for (std::size_t f{0}; f < hull->numFaces; ++f)
            {
                indices.push_back(hull->faceIndices[f]);
            }

            // generate VAO, VBO & EBO
            glGenVertexArrays(1, &clusterData.VAO);
            glGenBuffers(1, &clusterData.VBO);
            glGenBuffers(1, &clusterData.EBO);
            glGenBuffers(1, &clusterData.VBO);
            glBindVertexArray(clusterData.VAO);
            // buffer vertex data
            glBindBuffer(GL_ARRAY_BUFFER, clusterData.VBO);
            glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(vertices.size() * sizeof(float)), vertices.data(), GL_STATIC_DRAW);
            // buffer indices data
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, clusterData.EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(indices.size() * sizeof(unsigned int)), indices.data(), GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), reinterpret_cast<void*>(0));

            // unbind
            // don't unbind EBO, it's stored in VAO
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            glBindVertexArray(0);

            m_clusters[i].insert(std::pair{idx, clusterData});
        }
    }

    // all good
    return 0;
}