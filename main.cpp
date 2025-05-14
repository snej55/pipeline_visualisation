#include <arrow/csv/api.h>

#include <arrow/io/api.h>
#include <arrow/filesystem/localfs.h>
#include <arrow/table.h>

#include <memory>
#include <iostream>

int main()
{
    arrow::io::IOContext io {arrow::io::default_io_context()};
    // std::shared_ptr<arrow::io::InputStream> input {arrow::io::ReadableFile::Open("data/papers_with_labels.csv")};

    std::shared_ptr<arrow::io::ReadableFile> infile;
    arrow::Result<std::shared_ptr<arrow::io::ReadableFile>> result {arrow::io::ReadableFile::Open("data/papers_with_labels.csv")};
    if (!result.ok())
    {
        std::cerr << "Error: Failed to read csv file!" << std::endl;
        return 1;
    }

    auto maybeReader {arrow::csv::StreamingReader::Make(io, infile, arrow::csv::ReadOptions::Defaults(),
                                                                    arrow::csv::ParseOptions::Defaults(),
                                                                    arrow::csv::ConvertOptions::Defaults())};
    if (!maybeReader.ok())
    {
        std::cerr << "Error: Failed to create Streaming Reader!" << std::endl;
        return 1;
    }

    return 0;
}