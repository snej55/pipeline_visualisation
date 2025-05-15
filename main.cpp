#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

struct Element
{
    // paper title
    std::string title;
    // whether the study is included or not
    int included;
    // 2d position
    float pos2Dx;
    float pos2Dy;
    // 3d position
    float pos3Dx;
    float pos3Dy;
    float pos3Dz;
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
    std::string cluster_2_2d_label;
    std::string cluster_3_2d_label;
    std::string cluster_4_2d_label;
    std::string cluster_5_2d_label;
    std::string cluster_6_2d_label;
    std::string cluster_2_3d_label;
    std::string cluster_3_3d_label;
    std::string cluster_4_3d_label;
    std::string cluster_5_3d_label;
    std::string cluster_6_3d_label;
};

std::vector<Element> getElements(const std::string& filename)
{
    std::vector<Element> data;
    std::ifstream file;

    // handle exceptions
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        file.open(filename);
        int count{0};
        std::string line;
        while (std::getline(file, line))
        {
            Element element;
            std::vector<std::string> fields;
            std::size_t start {0};
            bool quote{false};
            for (std::size_t i{0}; i < line.length(); ++i)
            {
                if (line[i] == '"') {
                    quote = !quote; // toggle quote
                }
                if (!quote && line[i] == ',')
                {
                    fields.push_back(line.substr(start, i - start));
                    start = i + 1;
                }
            }
    
            for (const std::string_view& f : fields) {
                std::cout << f << '\n';
            }
            ++count;
            // if (count > 3)
            //     break;
        }
    } catch ([[maybe_unused]] std::ifstream::failure& e)
    {
        // clear data and return it (nothing)
        std::cerr << "Error: Failed to read file from path: `" << filename << "`" << std::endl;
        data.clear();
        return data;
    }


    file.close();

    return data;
}

std::vector<std::string_view> parseCSVRow(std::string_view row) {
    std::vector<std::string_view> fields;
    size_t start = 0;
    bool inQuotes = false;
    
    for (size_t i = 0; i < row.length(); ++i) {
        if (!inQuotes && row[i] == ',') {
            fields.emplace_back(row.substr(start, i - start));
            start = i + 1;
        } else if (row[i] == '"') {
            inQuotes = !inQuotes;
        }
    }
    fields.emplace_back(row.substr(start));
    
    return fields;
}

std::vector<std::vector<std::string_view>> readCSV(const std::string& filename) {
    std::vector<std::vector<std::string_view>> data;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        data.push_back(parseCSVRow(line));
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

    getElements("data/papers_with_labels.csv");

    return 0;
}

// #include <arrow/csv/api.h>

// #include <arrow/io/api.h>
// #include <arrow/filesystem/localfs.h>
// #include <arrow/table.h>

// #include <memory>
// #include <iostream>

// int main()
// {
//     arrow::io::IOContext io {arrow::io::default_io_context()};
//     // std::shared_ptr<arrow::io::InputStream> input {arrow::io::ReadableFile::Open("data/papers_with_labels.csv")};

//     std::shared_ptr<arrow::io::ReadableFile> infile;
//     arrow::Result<std::shared_ptr<arrow::io::ReadableFile>> result {arrow::io::ReadableFile::Open("data/papers_with_labels.csv")};
//     if (!result.ok())
//     {
//         std::cerr << "Error: Failed to read csv file!" << std::endl;
//         return 1;
//     }

//     auto maybeReader {arrow::csv::StreamingReader::Make(io, infile, arrow::csv::ReadOptions::Defaults(),
//                                                                     arrow::csv::ParseOptions::Defaults(),
//                                                                     arrow::csv::ConvertOptions::Defaults())};
//     if (!maybeReader.ok())
//     {
//         std::cerr << "Error: Failed to create Streaming Reader!" << std::endl;
//         return 1;
//     }

//     return 0;
// }