#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <locale>

struct Element
{
    // paper title
    std::wstring title;
    // whether the study is included or not
    int included;
    // 2d position
    double pos2Dx;
    double pos2Dy;
    // 3d position
    double pos3Dx;
    double pos3Dy;
    double pos3Dz;
    int cluster_2_2d;
    int cluster_2_3d;
    int cluster_3_2d;
    int cluster_3_3d;
    int cluster_4_2d;
    int cluster_4_3d;
    int cluster_5_2d;
    int cluster_5_3d;
    int cluster_6_2d;
    int cluster_6_3d;
    std::wstring cluster_2_2d_label;
    std::wstring cluster_3_2d_label;
    std::wstring cluster_4_2d_label;
    std::wstring cluster_5_2d_label;
    std::wstring cluster_6_2d_label;
    std::wstring cluster_2_3d_label;
    std::wstring cluster_3_3d_label;
    std::wstring cluster_4_3d_label;
    std::wstring cluster_5_3d_label;
    std::wstring cluster_6_3d_label;
};

template <typename T>
void wstrconv(const std::wstring& wstr, T* x)
{
    std::wstringstream wss{wstr};
    wss >> *x;
}

Element createElement(const std::vector<std::wstring>& fields)
{
    // element to create
    Element element;

    element.title = fields[0]; // paper title
    wstrconv<int>(fields[1], &element.included); // whether the study is included or not
    // element 2D space coordinates
    wstrconv<double>(fields[2], &element.pos2Dx);
    wstrconv<double>(fields[3], &element.pos2Dy);
    // element 3D space coordinates
    wstrconv<double>(fields[4], &element.pos3Dx);
    wstrconv<double>(fields[5], &element.pos3Dy);
    wstrconv<double>(fields[6], &element.pos3Dz);
    // cluster coordinates
    wstrconv<int>(fields[7], &element.cluster_2_2d); // cluster 2
    wstrconv<int>(fields[8], &element.cluster_2_3d);
    wstrconv<int>(fields[9], &element.cluster_3_2d); // cluster 3
    wstrconv<int>(fields[10], &element.cluster_3_3d);
    wstrconv<int>(fields[11], &element.cluster_4_2d); // cluster 4
    wstrconv<int>(fields[12], &element.cluster_4_3d);
    wstrconv<int>(fields[13], &element.cluster_5_2d); // cluster 5
    wstrconv<int>(fields[14], &element.cluster_5_3d);
    wstrconv<int>(fields[15], &element.cluster_6_2d); // cluster 6
    wstrconv<int>(fields[16], &element.cluster_6_3d);
    // cluster labels
    element.cluster_2_2d_label = fields[17]; // 2D labels
    element.cluster_3_2d_label = fields[18];
    element.cluster_4_2d_label = fields[19];
    element.cluster_5_2d_label = fields[20];
    element.cluster_6_2d_label = fields[21];
    element.cluster_2_3d_label = fields[22]; // 3D labels
    element.cluster_2_3d_label = fields[23];
    element.cluster_2_3d_label = fields[24];
    element.cluster_2_3d_label = fields[25];
    element.cluster_2_3d_label = fields[26];
    return element;
}

std::vector<Element> getElements(const std::string& filename)
{
    std::vector<Element> data;
    // wifstream for "wide" character encoding
    std::wifstream file;
    // set locale
    file.imbue(std::locale("en_GB.UTF-8"));

    // handle exceptions
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        file.open(filename);
        int count{0};
        std::wstring line;
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
                // if we hit a comment and we're not in a quote
                if (!quote && line[i] == ',')
                {
                    // get substring from (start -> i)
                    fields.push_back(line.substr(start, i - start));
                    start = i + 1; // update start
                }
            }
            // get final field
            fields.push_back(line.substr(start));
            
            // each element needs exactly 27 fields
            if (std::size(fields) == 27)
            {
                // Create the element
                Element element {createElement(fields)};
                data.push_back(element);
                ++count; // update row counter
                std::cout << "Element No." << count << '\r';
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
        data.clear();
        return data;
    }

    file.close();

    return data;
}

int main() {
    // auto data = readCSV("data/papers_with_labels.csv");
    
    // for (const auto& row : data) {
    //     for (const auto& cell : row) {
    //         std::cout << cell << "\n";
    //     }
    //     std::cout << std::endl;
    // }

    std::vector<Element> elements {getElements("data/papers_with_labels.csv")};

    return 0;
}
