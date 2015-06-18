// Copyright (C) 2015 Krzysztof Stachowiak

#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <allegro5/allegro.h>
#include <glm/glm.hpp>

struct MoonContext;
struct MoonContextDeleter { void operator()(MoonContext*); };

class Config {
protected:
    std::map<std::string, int> m_integers;
    std::map<std::string, double> m_reals;
    std::unique_ptr<MoonContext, MoonContextDeleter> m_context;

public:
    Config(const std::string& script_path);
    int get_integer(const std::string& key);
    double get_real(const std::string& key);
};

class KulkiConfig : public Config {
    std::map<std::string, ALLEGRO_COLOR> m_colors;
    std::map<std::string, std::vector<ALLEGRO_COLOR>> m_color_ranges;
    std::map<std::string, glm::vec3> m_vecs;

public:
    KulkiConfig(const std::string& script_path);
    ALLEGRO_COLOR get_color(const std::string& key);
    std::vector<ALLEGRO_COLOR> get_color_range(const std::string& key);
    glm::vec3 get_vec(const std::string& key);
};

#endif
