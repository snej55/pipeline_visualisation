#include <memory>
#include <sstream>

#include <arrow/io/api.h>
#include <parquet/arrow/reader.h>
#include <parquet/stream_reader.h>

int main()
{
    std::shared_ptr<arrow::io::ReadableFile> infile;

    PARQUET_ASSIGN_OR_THROW(
        infile, arrow::io::ReadableFile::Open("data/papers_with_labels.parquet")
    );

    parquet::StreamReader stream {parquet::ParquetFileReader::Open(infile)};

    

    return 0;
}