#include <memory>
#include <sstream>
#include <iostream>
#include <string>
#include <cstdlib>

#include <arrow/io/api.h>
#include <arrow/filesystem/localfs.h>
#include <parquet/arrow/reader.h>
#include <parquet/stream_reader.h>
#include <arrow/table.h>

struct Element
{
    // paper title
    std::string title;
    // whether the study is included or not
    uint64_t included;
    // 2d position
    float pos2Dx;
    float pos2Dy;
    // 3d position
    float pos3Dx;
    float pos3Dy;
    float pos3Dz;
    uint16_t cluster_2_2d;
    uint16_t cluster_2_3d;
    uint16_t cluster_3_2d;
    uint16_t cluster_3_3d;
    uint16_t cluster_4_2d;
    uint16_t cluster_4_3d;
    uint16_t cluster_5_2d;
    uint16_t cluster_5_3d;
    uint16_t cluster_6_2d;
    uint16_t cluster_6_3d;
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

int main()
{
    arrow::MemoryPool* pool {arrow::default_memory_pool()};
    arrow::fs::LocalFileSystem fileSystem;
    // get input file
    std::shared_ptr<arrow::io::RandomAccessFile> input {fileSystem.OpenInputFile("data/papers_with_labels.parquet").ValueOrDie()};

    // open parquet file reader
    std::unique_ptr<parquet::arrow::FileReader> arrowReader;
    arrow::Status status {parquet::arrow::OpenFile(input, pool, &arrowReader)};
    if (!status.ok())
    {
        std::cerr << "Error opening input!" << std::endl;
        return 1;
    }

    // read file as a single arrow table
    std::shared_ptr<arrow::Table> table;
    status = arrowReader->ReadTable(&table);
    if (!status.ok())
    {
        std::cerr << "Error reading table!" << std::endl;
        return 1;
    }

    std::cout << "Successfully read parquet table!\n";

    arrow::TableBatchReader reader {*table};

    // just to check mem. usage
    int x{0};
    std::cin >> x;
    // std::shared_ptr<arrow::io::ReadableFile> infile;

    // PARQUET_ASSIGN_OR_THROW(
    //     infile, arrow::io::ReadableFile::Open("data/papers_with_labels.parquet")
    // );

    // parquet::StreamReader stream {parquet::ParquetFileReader::Open(infile)};

    // Element study;

    // bool first{true};
    // int count{0};
    // while (!stream.eof())
    // {
    //     std::cout << "row: " << count++ << '\n';
    //     if (!first)
    //     {
    //         stream.SkipRows(1);
    //         stream >> study.title;
    //         stream.SkipColumns(1);
    //         stream >> study.pos2Dx >> study.pos2Dy >> study.pos3Dx >> study.pos3Dy >> study.pos3Dz >> study.cluster_2_2d >> study.cluster_2_3d >> study.cluster_3_2d >> study.cluster_3_3d >> study.cluster_4_2d >> study.cluster_4_3d >> study.cluster_5_2d >> study.cluster_5_3d >> study.cluster_6_2d >> study.cluster_6_3d
    //         >> study.cluster_2_2d_label >> study.cluster_3_2d_label >> study.cluster_4_2d_label >> study.cluster_5_2d_label >> study.cluster_6_2d_label >> study.cluster_2_3d_label >> study.cluster_3_3d_label >> study.cluster_4_3d_label >> study.cluster_5_3d_label >> study.cluster_6_3d_label >> parquet::EndRow; 
    //         std::cout << study.title << std::endl;;
    //     }
    //     first = false;
    //     // break;
    // }


    return 0;
}