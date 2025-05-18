#include "paper_loader.h"

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

        int count{0}; // row counter
        int included{0}; // num papers included
        std::wstring line; // the current row
        do
        {
            // read the next line from the file
            std::getline(file, line);
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
                std::cout << "Paper No." << count << " | " << included << " included" << '\r';

                if (paper.included) {
                    ++included;
                }
            }

            // // for testing
            // if (count > 20)
            //     break;
        } while (file.peek() != EOF); // loop until we reach end of file
        std::cout << std::endl;
        std::cout << "Loaded csv from `" << filename << "`. Rows: " << count << '\n';
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
void PaperLoader::getVertices(std::vector<float>& vertices, const double scale) const {
    vertices.clear();
    int included{0};
    int not_included{0};
    for (const Paper& paper : m_papers)
    {
        vertices.push_back(static_cast<float>(paper.pos3Dx * scale)); // x
        vertices.push_back(static_cast<float>(paper.pos3Dy * scale)); // y
        vertices.push_back(static_cast<float>(paper.pos3Dz * scale)); // z
        vertices.push_back(static_cast<float>(paper.included));
        if (paper.included)
            ++included;
        else
            not_included++;
    }
    // get info
    std::cout << "Loaded " << m_papers.size() << " vertices (" << vertices.size() * sizeof(float) << " bytes)" << '\n';
    std::cout << included << " papers included, " << not_included << " papers not included\n";
}