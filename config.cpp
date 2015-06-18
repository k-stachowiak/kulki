#include <iostream>

#include "config.h"
#include "moon.h"

// Common config implementation

namespace {

    double load_real(MoonContext *ctx, const std::string& symbol)
    {
        MoonValue *value = mn_exec_command(ctx, symbol.c_str());
        if (value->type != MN_REAL) {
            std::cerr << "Type error while reading config value " << symbol << std::endl;
            exit(1);
        }
        double result = value->data.real;
        mn_dispose(value);
        return result;
    }

    int load_int(MoonContext *ctx, const std::string& symbol)
    {
        MoonValue *value = mn_exec_command(ctx, symbol.c_str());
        if (value->type != MN_INT) {
            std::cerr << "Type error while reading config value " << symbol << std::endl;
            exit(1);
        }
        int result = value->data.integer;
        mn_dispose(value);
        return result;
    }

}

void MoonContextDeleter::operator()(MoonContext *ctx)
{
    mn_destroy(ctx);
}

Config::Config(const std::string& script_path) :
    m_context { mn_create() }
{
    if (!mn_exec_file(m_context.get(), script_path.c_str())) {
        std::cerr << "Failed loading configuration file." << std::endl;
        std::cerr << "Error message: " << mn_error_message() << std::endl;
        exit(1);
    }
}

int Config::get_integer(const std::string& key)
{
    auto it = m_integers.find(key);
    if (it != end(m_integers)) {
        return it->second;
    }

    int value = load_int(m_context.get(), key);
    m_integers.insert({ key, value });
    return value;
}

double Config::get_real(const std::string& key)
{
    auto it = m_reals.find(key);
    if (it != end(m_reals)) {
        return it->second;
    }

    double value = load_real(m_context.get(), key);
    m_reals.insert({ key, value });
    return value;
}

// Kulki specific config implementation.

namespace {

    ALLEGRO_COLOR parse_color(MoonValue *value, const std::string& symbol)
    {
        if ((value->type != MN_ARRAY) ||
            (value->data.compound == NULL || value->data.compound->type != MN_REAL) ||
            (value->data.compound->next == NULL || value->data.compound->next->type != MN_REAL) ||
            (value->data.compound->next->next == NULL || value->data.compound->next->next->type != MN_REAL)) {
                std::cerr << "Type error while reading config value " << symbol << std::endl;
                exit(1);
        }
        return al_map_rgb_f(
            value->data.compound->data.real,
            value->data.compound->next->data.real,
            value->data.compound->next->next->data.real);
    }

    ALLEGRO_COLOR load_color(MoonContext *ctx, const std::string& symbol)
    {
        MoonValue *value = mn_exec_command(ctx, symbol.c_str());
        ALLEGRO_COLOR result = parse_color(value, symbol);
        mn_dispose(value);
        return result;
    }

    std::vector<ALLEGRO_COLOR> load_color_range(MoonContext *ctx, const std::string& symbol)
    {
        MoonValue *value = mn_exec_command(ctx, symbol.c_str());
        if (value->type != MN_ARRAY) {
            std::cerr << "Type error while reading config value " << symbol << std::endl;
            exit(1);
        }
        MoonValue *item = value->data.compound;
        std::vector<ALLEGRO_COLOR> result;
        while (item) {
            result.push_back(parse_color(item, symbol));
            item = item->next;
        }
        mn_dispose(value);
        return result;
    }

    glm::vec3 load_vec3(MoonContext *ctx, const std::string& symbol)
    {
        MoonValue *value = mn_exec_command(ctx, symbol.c_str());
        if ((value->type != MN_ARRAY) ||
            (value->data.compound == NULL || value->data.compound->type != MN_REAL) ||
            (value->data.compound->next == NULL || value->data.compound->next->type != MN_REAL) ||
            (value->data.compound->next->next == NULL || value->data.compound->next->next->type != MN_REAL)) {
                std::cerr << "Type error while reading config value " << symbol << std::endl;
                exit(1);
        }
        glm::vec3 result {
            value->data.compound->data.real,
            value->data.compound->next->data.real,
            value->data.compound->next->next->data.real };
        mn_dispose(value);
        return result;
    }

}

KulkiConfig::KulkiConfig(const std::string& script_path) :
    Config { script_path }
{
    m_integers["EMPTY"] = -1;
}

ALLEGRO_COLOR KulkiConfig::get_color(const std::string& key)
{
    auto it = m_colors.find(key);
    if (it != end(m_colors)) {
        return it->second;
    }

    ALLEGRO_COLOR value = load_color(m_context.get(), key);
    m_colors.insert({ key, value });
    return value;
}

std::vector<ALLEGRO_COLOR> KulkiConfig::get_color_range(const std::string& key)
{
    auto it = m_color_ranges.find(key);
    if (it != end(m_color_ranges)) {
        return it->second;
    }

    std::vector<ALLEGRO_COLOR> value = load_color_range(m_context.get(), key);
    m_color_ranges.insert({ key, value });
    return value;
}

glm::vec3 KulkiConfig::get_vec(const std::string& key)
{
    auto it = m_vecs.find(key);
    if (it != end(m_vecs)) {
        return it->second;
    }

    glm::vec3 value = load_vec3(m_context.get(), key);
    m_vecs.insert({ key, value });
    return value;
}
