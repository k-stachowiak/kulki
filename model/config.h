// Copyright (C) 2015 Krzysztof Stachowiak

#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <vector>
#include <string>
#include <memory>
#include <allegro5/allegro.h>
#include <glm/glm.hpp>

#include "dick.h"

struct MoonContext;
struct MoonValue;

struct MoonContextDeleter { void operator()(MoonContext*); };

class KulkiConfig {
    std::string m_var_filename;
    std::unique_ptr<MoonContext, MoonContextDeleter> m_context;

    std::map<std::string, int> m_integers;
    std::map<std::string, double> m_reals;

    std::map<std::string, dick::Color> m_colors;
    std::map<std::string, std::vector<dick::Color>> m_color_ranges;
    std::map<std::string, glm::vec3> m_vecs;

    MoonValue *m_load_value(const std::string& symbol) const;

    double m_load_real(const std::string& symbol) const;
    int m_load_int(const std::string& symbol) const;

    dick::Color m_load_color(const std::string& symbol) const;
    std::vector<dick::Color> m_load_color_range(const std::string& symbol) const;
    glm::vec3 m_load_vec3(const std::string& symbol) const;

public:
    KulkiConfig(
            const std::string& script_path,
            const std::string& variables_path);
    int get_integer(const std::string& key);
    double get_real(const std::string& key);
    dick::Color get_color(const std::string& key);
    std::vector<dick::Color> get_color_range(const std::string& key);
    glm::vec3 get_vec(const std::string& key);
    void save_variables(int board_w, int board_h, int default_color_count);
};

#endif
