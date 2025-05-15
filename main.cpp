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
void wstringconv(const std::wstring& wstr, T* x)
{
    std::wstringstream wss{wstr};
    wss >> *x;
}

Element createElement(const std::vector<std::wstring>& fields)
{
    // element to create
    Element element;

    element.title = fields[0]; // paper title
    wstringconv<int>(fields[1], &element.included);

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
        while (file.peek() != EOF) // loop until we reach end of file
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
        }
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