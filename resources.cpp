#include <cstdlib>
#include <iostream>

#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

#include "resources.h"

void Resources::m_load_bitmap(const std::string& filename)
{
    ALLEGRO_BITMAP *bitmap = al_load_bitmap(filename.c_str());
    if (!bitmap) {
        std::cerr << "Failed loading bitmap \"" << filename << "\"" << std::endl;
        exit(1);
    }
    m_bitmaps[filename].reset(bitmap);
}

void Resources::m_load_font(const std::string& filename, int size)
{
    ALLEGRO_FONT *font = al_load_font(filename.c_str(), -size, 0);
    if (!font) {
        std::cerr << "Failed loading font \"" << filename << "\"" << std::endl;
        exit(1);
    }

    m_fonts[std::make_pair(filename, size)].reset(font);
}

ALLEGRO_BITMAP* Resources::get_bitmap(const std::string& filename)
{
    auto it = m_bitmaps.find(filename);
    if (it == end(m_bitmaps)) {
        m_load_bitmap(filename);
    }
    return m_bitmaps.at(filename).get();
}

ALLEGRO_FONT* Resources::get_font(const std::string& filename, int size)
{
    auto it = m_fonts.find(std::make_pair(filename, size));
    if (it == end(m_fonts)) {
        m_load_font(filename, size);
    }
    return m_fonts.at(std::make_pair(filename, size)).get();
}
