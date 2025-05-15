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

std::vector<std::vector<std::string>> readCSV(const std::string& filename) {
    std::vector<std::vector<std::string>> data;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Failed to open file: " << filename << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::vector<std::string> row;
        std::stringstream ss(line);
        std::string cell;

        while (std::getline(ss, cell, ',')) {
            row.push_back(cell);
        }

        data.push_back(row);
    }

    file.close();
    return data;
}

int main() {
    auto data = readCSV("data/papers_with_labels.csv");
    
    for (const auto& row : data) {
        for (const auto& cell : row) {
            std::cout << cell << "\n";
        }
        std::cout << std::endl;
    }

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