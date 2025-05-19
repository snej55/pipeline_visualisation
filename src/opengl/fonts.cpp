#include "fonts.h"

#include <iostream>

FontManager::~FontManager()
{
    free();
}

bool FontManager::init(const std::string& path, const int height)
{
    m_loaded = false;
    // initialize freetype2 library
    if (FT_Init_FreeType(&m_FT))
    {
        std::cout << "ERROR::FONT_MANAGER: Could not init FreeType library" << std::endl;
        return -1;
    }

    // load font
    if (FT_New_Face(m_FT, path.c_str(), 0, &m_face))
    {
        std::cout << "ERROR::FONT_MANAGER: Failed to load font at `" << path << "`" << std::endl;
        return -1;
    }

    // set font size
    FT_Set_Pixel_Sizes(m_face, 0, height);
    
    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // first 128 ascii characters
    for (unsigned char c{0}; c < 128; c++)
    {
        // load character glyph
        if (FT_Load_Char(m_face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FONT_MANAGER: Failed to load glyph" << std::endl;
            continue; // go to next character
        }

        // generate the texture
        unsigned int tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, m_face->glyph->bitmap.buffer);
        // texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // store the character in character map
        Character character {
            tex,
            glm::ivec2{m_face->glyph->bitmap.width, m_face->glyph->bitmap.rows},
            glm::ivec2{m_face->glyph->bitmap_left, m_face->glyph->bitmap_top},
            static_cast<unsigned int>(m_face->glyph->advance.x)
        };
        // insert character into map
        m_characters.insert(std::pair<char, Character>{c, character});
    }

    // generate vertex arrays & vbo
    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glBindVertexArray(m_VAO);
    
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    // enough memory for rendering characters
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), reinterpret_cast<void*>(0));
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // all good
    m_loaded = true;
    std::cout << "Successfully loaded font from `" << path << "`\n";
    return 0;
}

void FontManager::free()
{
    if (m_loaded)
    {
        FT_Done_Face(m_face);
        FT_Done_FreeType(m_FT);
    }
}

void FontManager::updateProjection(const float width, const float height)
{
    // update projection matrix with new framebuffer dimensions
    m_projection = glm::ortho(0.0f, width, 0.0f, height);
}