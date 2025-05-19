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

    bool init();
};

#endif