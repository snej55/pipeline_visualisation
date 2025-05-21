#include "paper_loader.h"

#include <iostream>
#include <fstream>

PaperLoader::PaperLoader()
{
    m_clusters.resize(5);
}

// load papers from csv file
void PaperLoader::loadFromFile(const std::string& filename)
{
    // clear previous papers
    m_papers.clear();
    // wifstream for "wide" character encoding
    std::wifstream file;
    // set locale
    file.imbue(std::locale("en_GB.UTF-8"));

    // handle exceptions
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        file.open(filename);
        
        bool firstRow{true};
        int count{0}; // row counter
        int included{0}; // num papers included
        int lastIncluded{0}; // index of last paper included
        std::wstring line; // the current row
        do
        {
            // read the next line from the file
            std::getline(file, line);
            if (firstRow)
            {
                firstRow = false;
                continue;
            }
            // store the fields
            std::vector<std::wstring> fields;
            
            // load fields from row
            std::size_t start {0};
            bool quote{false};
            for (std::size_t i{0}; i < line.length(); ++i)
            {
                if (line[i] == '"') { // we either started or hit a quote
                    quote = !quote; // toggle quote
                }
                // if we hit a comma and we're not in a quote
                if (!quote && line[i] == ',')
                {
                    // get substring from (start -> i)
                    fields.push_back(line.substr(start, i - start));
                    start = i + 1; // update start
                }
            }
            // get final field
            fields.push_back(line.substr(start));
            
            // each paper needs exactly 27 fields
            if (std::size(fields) == 27)
            {
                // Create the paper
                Paper paper;
                createPaper(fields, paper);
                m_papers.push_back(paper);
                ++count; // update row counter
                std::cout << "Paper No." << count << " | " << included << " included | " << lastIncluded << " LII" << '\r';

                if (paper.included) {
                    ++included;
                    lastIncluded = count;
                }
            }
        } while (file.peek() != EOF); // loop until we reach end of file

        std::cout << std::endl;
        std::cout << "Loaded csv from `" << filename << "`. Rows: " << count << " (" << std::size(m_papers) * sizeof(Paper) / 1000000 << " MB)" << '\n';

        // update stats
        m_numIncluded = included;
        m_lastIndex = lastIncluded;
        m_papersSize = std::size(m_papers) * sizeof(Paper);
    } catch ([[maybe_unused]] std::ifstream::failure& e)
    {
        // clear data and return it (nothing)
        std::cout << "Error: " << e.what() << '\n';
        std::cerr << "Error: Failed to read file from path: `" << filename << "`" << std::endl;
        m_papers.clear(); // clear papers
        if (file.is_open()) {
            file.close();
        }
        return;
    }

    file.close();
}

// create paper from list of fields
void PaperLoader::createPaper(const std::vector<std::wstring>& fields, Paper& paper) const
{
    paper.title = fields[0]; // paper title
    wstrconv<int>(fields[1], &paper.included); // whether the study is included or not
    // paper 2D space coordinates
    wstrconv<double>(fields[2], &paper.pos2Dx);
    wstrconv<double>(fields[3], &paper.pos2Dy);
    // paper 3D space coordinates
    wstrconv<double>(fields[4], &paper.pos3Dx);
    wstrconv<double>(fields[5], &paper.pos3Dy);
    wstrconv<double>(fields[6], &paper.pos3Dz);
    // cluster coordinates
    wstrconv<int>(fields[7], &paper.cluster_2_2d); // cluster 2
    wstrconv<int>(fields[8], &paper.cluster_2_3d);
    wstrconv<int>(fields[9], &paper.cluster_3_2d); // cluster 3
    wstrconv<int>(fields[10], &paper.cluster_3_3d);
    wstrconv<int>(fields[11], &paper.cluster_4_2d); // cluster 4
    wstrconv<int>(fields[12], &paper.cluster_4_3d);
    wstrconv<int>(fields[13], &paper.cluster_5_2d); // cluster 5
    wstrconv<int>(fields[14], &paper.cluster_5_3d);
    wstrconv<int>(fields[15], &paper.cluster_6_2d); // cluster 6
    wstrconv<int>(fields[16], &paper.cluster_6_3d);
    // cluster labels
    paper.cluster_2_2d_label = fields[17]; // 2D labels
    paper.cluster_3_2d_label = fields[18];
    paper.cluster_4_2d_label = fields[19];
    paper.cluster_5_2d_label = fields[20];
    paper.cluster_6_2d_label = fields[21];
    paper.cluster_2_3d_label = fields[22]; // 3D labels
    paper.cluster_2_3d_label = fields[23];
    paper.cluster_2_3d_label = fields[24];
    paper.cluster_2_3d_label = fields[25];
    paper.cluster_2_3d_label = fields[26];
}

// scale is double because paper coordinates are double
void PaperLoader::getVertices(std::vector<float>& vertices, const double scale) {
    vertices.clear();
    int included{0};
    int not_included{0};
    for (std::size_t i{0}; i < std::size(m_papers); ++i)
    {
        Paper* paper {&m_papers[i]};
        paper->counter = i; // update counter
        vertices.push_back(static_cast<float>(paper->pos3Dx * scale)); // x
        vertices.push_back(static_cast<float>(paper->pos3Dy * scale)); // y
        vertices.push_back(static_cast<float>(paper->pos3Dz * scale)); // z
        vertices.push_back(static_cast<float>(paper->included));
        vertices.push_back(static_cast<float>(i)); // counter
        // for debug info
        if (paper->included)
            ++included;
        else
            not_included++;
    }
    // get info
    std::cout << "Loaded " << m_papers.size() << " vertices (" << vertices.size() * sizeof(float) / 1000 << " KB)" << '\n';
    std::cout << included << " papers included, " << not_included << " papers not included\n";
    // update stats
    m_verticesSize = vertices.size() * sizeof(vertices[0]);
}

void PaperLoader::generateClusters()
{
    std::cout << "Generating clusters...\n";
    for (int i{0}; i < 5; ++i)
    {
        generateClusterLevel(i);
    }
    std::cout << "-----------------------\n";
    std::cout << "Generated clusters!\n";
}

// generates clusters for a given level from papers
void PaperLoader::generateClusterLevel(const int idx)
{
    for (const Paper& paper : m_papers)
    {
        int clusterID {getClusterID(paper, idx + 2)};
        // if cluster doesn't exist yet create it
        if (!m_clusters[idx].contains(clusterID))
        {
            // create new cluster
            m_clusters[idx].insert(std::pair{clusterID, Cluster{}});
            // set correct label
            m_clusters[idx][getClusterID(paper, idx + 2)].label = getClusterLabel(paper, idx + 2);
        }
        // add another paper to cluster
        ++m_clusters[idx][clusterID].num_papers;
        // add paper vertices
        m_clusters[idx][clusterID].vertices.push_back({paper.pos3Dx, paper.pos3Dy, paper.pos3Dz});
    }
    // print clusters at level idx + 2
    std::cout << "--- Level " << idx + 2 << " Clusters ---\n";
    // const int since keys cannot be changed
    for (std::pair<const int, Cluster>& cluster : m_clusters[idx])
    {
        // calculate cluster centroid
        cluster.second.pos = getAvgPos(cluster.second.vertices);
        std::wcout << "\tCluster No." << cluster.first << " (" << cluster.second.num_papers << " papers, under `" << cluster.second.label << "`)\n";
        std::cout << "\tCluster centroid: " << cluster.second.pos.x << " " << cluster.second.pos.y << " " << cluster.second.pos.z << "\n";
    }
}

glm::vec3 PaperLoader::getAvgPos(const std::vector<glm::vec3>& papers) const
{
    glm::vec3 avg{0.0f};
    for (const glm::vec3& pos : papers)
    {
        avg += pos;
    }
    // convert to int from unsigned int first to avoid *weird* errors
    avg /= static_cast<float>(papers.size());
    return avg;
}

// return cluster id for paper at given depth (2-6) default depth is 2
int PaperLoader::getClusterID(const Paper& paper, int depth) const
{
    depth = std::max(2, std::min(6, depth));
    switch (depth)
    {
        case 2:
            return paper.cluster_2_2d;
        case 3:
            return paper.cluster_3_2d;
        case 4:
            return paper.cluster_4_2d;
        case 5:
            return paper.cluster_5_2d;
        case 6:
            return paper.cluster_6_2d;
        default:
            return paper.cluster_2_2d;
    }
}

// return cluster label for paper at given depth (2-6) default depth is 2
std::wstring PaperLoader::getClusterLabel(const Paper& paper, int depth) const
{
    depth = std::max(2, std::min(6, depth));
    switch (depth)
    {
        case 2:
            return paper.cluster_2_2d_label;
        case 3:
            return paper.cluster_3_2d_label;
        case 4:
            return paper.cluster_4_2d_label;
        case 5:
            return paper.cluster_5_2d_label;
        case 6:
            return paper.cluster_6_2d_label;
        default:
            return paper.cluster_2_2d_label;
    }
}

Cluster* PaperLoader::getCluster(int id, int depth)
{
    depth = std::max(2, std::min(6, depth));
    // avoid copying large cluster
    std::map<int, Cluster>* cluster {&m_clusters[depth]};
    return &(*cluster)[id];
}

// return map of clusters for given depth (2-6)
const std::map<int, Cluster>& PaperLoader::getClusters(const int depth) const
{
    const std::size_t index {static_cast<std::size_t>(std::max(2, std::min(6, depth)))};
    return m_clusters[index - 2];
}

const Paper& PaperLoader::getPaper(float progress) const
{
    return m_papers[std::max(static_cast<unsigned int>(0), std::min(static_cast<unsigned int>(m_papers.size() - 1), static_cast<unsigned int>(progress)))];
}