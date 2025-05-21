#include "clusters.h"

// convex hull library
#define CONVHULL_3D_ENABLE
#include <convhull_3d.h>

#include <utility>

Clusters::ClusterRenderer::ClusterRenderer()
{
    m_clusters.resize(5);
}

Clusters::ClusterRenderer::~ClusterRenderer()
= default;

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
            convhull_3d_export_obj(chVertices, hull->numVertices, hull->faceIndices, hull->numFaces, 1, const_cast<char*>(name.c_str()));
            // success check
            if (hull->faceIndices == nullptr)
            {
                std::cout << "ERROR::CLUSTER_RENDERER::GENERATE_CLUSTERS: Failed to create convex hull!" << std::endl;
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
        std::cout << "Generated cluster level " << i + 2 << std::endl;
    }

    // all good
    std::cout << "Clusters generated!" << std::endl;
    freeHulls();
    m_loaded = true;
    return 0;
}

// free memory from cluster data
void Clusters::ClusterRenderer::freeHulls()
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

// ------------ Model Loading ------------ //

// Convex Hull mesh
// get vertices & vertex indices and set up the mesh
Clusters::ClusterMesh::ClusterMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
    : m_vertices{vertices}, m_indices{indices}
{
    setupMesh();
}

// set up vertex arrays and buffers
void Clusters::ClusterMesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // struct Vertex has correct alignment in memory (structs have sequential memory layout)
    glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_vertices.size() * sizeof(Vertex)), m_vertices.data(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(m_indices.size() * sizeof(unsigned int)),
                 m_indices.data(), GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(0));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glBindVertexArray(0);
}

void Clusters::ClusterMesh::render(const Shader& shader) const
{
    shader.use();
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(m_indices.size()), GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

// Convex Hull model
// use assimp to load model meshes from path
Clusters::ClusterModel::ClusterModel(const std::string& path)
    : m_path{path}
{
    std::cout << "Created cluster model at path: `" << path <<"`\n";
    loadModel(path);
}

// render all the model meshes
void Clusters::ClusterModel::render(const Shader& shader)
{
    for (const ClusterMesh& mesh : m_meshes)
    {
        mesh.render(shader);
    }
}

// load meshes from path
void Clusters::ClusterModel::loadModel(const std::string& path)
{
    std::cout << "Loading cluster model from path: `" << path << "`... ";
    Assimp::Importer importer;
    const aiScene* scene {importer.ReadFile(path.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace)};
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:" << importer.GetErrorString() << std::endl;
        return;
    }
    m_directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);
    std::cout << "Loaded cluster model from path: `" << path << "`!\n";
}

// recursively load meshes from root node and children
void Clusters::ClusterModel::processNode(aiNode* node, const aiScene* scene)
{
    for (std::size_t i {0}; i < node->mNumMeshes; ++i)
    {
        // get mesh from scene
        aiMesh* mesh {scene->mMeshes[node->mMeshes[i]]};
        m_meshes.push_back(processMesh(mesh));
    }

    // repeat for child nodes
    for (std::size_t n {0}; n < node->mNumChildren; ++n)
    {
        processNode(node->mChildren[n], scene);
    }
}

Clusters::ClusterMesh Clusters::ClusterModel::processMesh(const aiMesh* mesh)
{
    std::vector<Vertex> vertices{};
    std::vector<unsigned int> indices{};

    for (std::size_t v {0}; v < mesh->mNumVertices; ++v)
    {
        // process vertex
        Vertex vertex {
            glm::vec3 {mesh->mVertices[v].x, mesh->mVertices[v].y, mesh->mVertices[v].z},
            glm::vec3 {mesh->mNormals[v].x, mesh->mNormals[v].y, mesh->mNormals[v].z},
        };

        vertices.push_back(vertex);
    }

    // same idea for indices
    for (std::size_t i{0}; i < mesh->mNumFaces; ++i)
    {
        aiFace face {mesh->mFaces[i]};
        // each face usually has like 3 indices or something
        for (unsigned int j{0}; j < face.mNumIndices; ++j)
        {
            indices.push_back(face.mIndices[j]);
        }
    }

    return ClusterMesh{vertices, indices};
}