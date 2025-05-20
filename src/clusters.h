// header file with necessary components to render clusters
#ifndef CLUSTERS_H
#define CLUSTERS_H

// opengl rendering
#include <glad/glad.h>

// for assimp model loading
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

#include "paper_loader.h"
#include "opengl/shader.h"

// namespace for rendering clusters
namespace Clusters
{
    struct ConvexHull
    {
        int numVertices{};
        int* faceIndices{nullptr};
        int numFaces{};
    };

    struct ClusterData
    {
        ConvexHull* hull{nullptr};
    };

    // loads convex hull for clusters and generates EBO, VBO & VAO
    class ClusterRenderer
    {
    public:
        ClusterRenderer();
        ~ClusterRenderer();
    
        int generateClusters(const std::vector<std::map<int, Cluster>>& clusters, float scale);
        void free();

        // not const because std::map[] isn't const
        ClusterData* getClusterData(int depth, int idx);

        // same here
        void renderCluster(const Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::vec3& color, int depth, int idx);
        void renderClusterLevel(const Shader& shader, const glm::mat4& projection, const glm::mat4& view, const glm::vec3& color, int depth);
    
    private:
        // flag to know if we need to free or not
        bool m_loaded{false};
        // contains cluster data for rendering
        std::vector<std::map<int, ClusterData>> m_clusters{};
    };

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
    };

    class ClusterMesh
    {
    public:
        ClusterMesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);
        void render(const Shader& shader) const;

    private:
        unsigned int VAO{}, VBO{}, EBO{};

        std::vector<Vertex> m_vertices;
        std::vector<unsigned int> m_indices;

        void setupMesh();
    };;

    class ClusterModel
    {
    public:
        explicit ClusterModel(std::string  path);

        void render(const Shader& shader);

    private:
        std::string m_path;
        std::vector<ClusterMesh> m_meshes{};
        std::string m_directory{};

        void loadModel(const std::string& path);
        void processNode(aiNode* node, const aiScene* scene);
        ClusterMesh processMesh(aiMesh* mesh, const aiScene* scene);
    };
};

#endif