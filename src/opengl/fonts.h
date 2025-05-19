#ifndef FONTS_H
#define FONTS_H

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>

class FontManager
{
public:
    explicit FontManager() = default;
    ~FontManager();

    bool init(const std::string& font);

    // getters & setters for font face and library
    [[nodiscard]] const FT_Face& getFace() const {return m_face;}
    [[nodiscard]] const FT_Library& getLibrary() const {return m_FT;}

    // font path and loaded flag
    [[nodiscard]] std::string_view getFontPath() const {return m_fontPath;}

private:
    FT_Face m_face{}; // font face
    FT_Library m_FT{}; // freetype library

    std::string m_fontPath{""}; // path of the current font
    bool m_loaded{false}; // whether a font has been loaded or not
};

#endif