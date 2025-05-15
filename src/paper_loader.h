/*
 * Object to load paper data from csv file.
 */

#ifndef PAPER_LOADER_H
#define PAPER_LOADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <locale>

struct Paper
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

class PaperLoader
{
public:
    PaperLoader() = default;
    ~PaperLoader() = default;

    void loadFromFile(const std::string& filename);

    void createPaper(const std::vector<std::wstring>& fields, Paper& paper) const;

    template <typename T>
    static void wstrconv(const std::wstring& wstr, T* x)
    {
        std::wstringstream wss{wstr};
        wss >> *x;
    }

    [[nodiscard]] const std::vector<Paper>& getPapers() const {return m_papers;}

private:
    std::vector<Paper> m_papers{};
};

#endif