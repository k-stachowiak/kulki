#ifndef RESOURCES_H
#define RESOURCES_H

#include <memory>
#include <string>
#include <map>

#include <allegro5/allegro_font.h>

class Resources {

    struct AlFontDeleter { void operator()(ALLEGRO_FONT *font) { al_destroy_font(font); } };
    struct AlBitmapDeleter { void operator()(ALLEGRO_BITMAP *bitmap) { al_destroy_bitmap(bitmap); } };

    using AlFont = std::unique_ptr<ALLEGRO_FONT, AlFontDeleter>;
    using AlBitmap = std::unique_ptr<ALLEGRO_BITMAP, AlBitmapDeleter>;

    std::map<std::string, AlBitmap> m_bitmaps;
    std::map<std::pair<std::string, int>, AlFont> m_fonts;

    void m_load_bitmap(const std::string& filename);
    void m_load_font(const std::string& filename, int size);

public:
    ALLEGRO_BITMAP* get_bitmap(const std::string& filename);
    ALLEGRO_FONT* get_font(const std::string& filename, int size);
};

#endif
