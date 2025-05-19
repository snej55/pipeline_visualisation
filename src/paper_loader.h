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

    // load papers data from csv file
    void loadFromFile(const std::string& filename);

    // create paper from list of fields
    void createPaper(const std::vector<std::wstring>& fields, Paper& paper) const;

    // convert from std::wstring to template type
    template <typename T>
    static void wstrconv(const std::wstring& wstr, T* x)
    {
        std::wstringstream wss{wstr};
        wss >> *x;
    }

    // gets list of 3D vertices from paper list
    void getVertices(std::vector<float>& vertices, double scale = 1.0f) const;

    // papers getter
    [[nodiscard]] const std::vector<Paper>& getPapers() const {return m_papers;}
    // stats getters
    [[nodiscard]] unsigned int getNumPapers() const {return std::size(m_papers);}
    [[nodiscard]] unsigned int getNumIncluded() const {return m_numIncluded;}
    [[nodiscard]] unsigned int getLastIndex() const {return m_lastIndex;}

private:
    // papers data
    std::vector<Paper> m_papers{};

    // stats
    unsigned int m_numIncluded{0}; // number of included papers
    unsigned int m_lastIndex{0}; // last index explored
};

#endif