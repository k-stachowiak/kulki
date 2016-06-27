#include <iostream>
#include <fstream>

#include "config.h"
#include "moon.h"

// Common config implementation

void MoonContextDeleter::operator()(MoonContext *ctx)
{
    mn_destroy(ctx);
}

// Kulki specific config implementation.

namespace {

    dick::Color parse_color(MoonValue *value, const std::string& symbol)
    {
        if ((value->type != MN_ARRAY) ||
            (value->data.compound == NULL || value->data.compound->type != MN_REAL) ||
            (value->data.compound->next == NULL || value->data.compound->next->type != MN_REAL) ||
            (value->data.compound->next->next == NULL || value->data.compound->next->next->type != MN_REAL)) {
                std::cerr << "Type error while reading config value " << symbol << std::endl;
                exit(1);
        }
        return {
            value->data.compound->data.real,
            value->data.compound->next->data.real,
            value->data.compound->next->next->data.real
        };
    }

}

MoonValue *KulkiConfig::m_load_value(const std::string& symbol) const
{
    MoonValue *value = mn_exec_command(m_context.get(), symbol.c_str());
    if (!value) {
        std::cerr << "Error while reading config value " << symbol << std::endl;
        exit(1);
    }

    return value;
}

double KulkiConfig::m_load_real(const std::string& symbol) const
{
    MoonValue *value = m_load_value(symbol.c_str());
    if (value->type != MN_REAL) {
        std::cerr << "Type error while reading config value " << symbol << std::endl;
        exit(1);
    }
    double result = value->data.real;
    mn_dispose(value);
    return result;
}

int KulkiConfig::m_load_int(const std::string& symbol) const
{
    MoonValue *value = m_load_value(symbol.c_str());
    if (value->type != MN_INT) {
        std::cerr << "Type error while reading config value " << symbol << std::endl;
        exit(1);
    }
    int result = value->data.integer;
    mn_dispose(value);
    return result;
}

dick::Color KulkiConfig::m_load_color(const std::string& symbol) const
{
    MoonValue *value = m_load_value(symbol.c_str());
    dick::Color result = parse_color(value, symbol);
    mn_dispose(value);
    return result;
}

std::vector<dick::Color> KulkiConfig::m_load_color_range(const std::string& symbol) const
{
    MoonValue *value = m_load_value(symbol.c_str());
    if (value->type != MN_ARRAY) {
        std::cerr << "Type error while reading config value " << symbol << std::endl;
        exit(1);
    }
    MoonValue *item = value->data.compound;
    std::vector<dick::Color> result;
    while (item) {
        result.push_back(parse_color(item, symbol));
        item = item->next;
    }
    mn_dispose(value);
    return result;
}

glm::vec3 KulkiConfig::m_load_vec3(const std::string& symbol) const
{
    MoonValue *value = m_load_value(symbol.c_str());
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

KulkiConfig::KulkiConfig(
        const std::string& script_path,
        const std::string& variables_path) :
    m_var_filename { variables_path },
    m_context { mn_create() }
{
    // Note: main config script may depend on the configurable "variable" config
    // script, which cannot occur the opposite way as the variables must be
    // overwriteable, thus bare constants, which don't depend on anything

    if (!mn_exec_file(m_context.get(), variables_path.c_str())) {
        std::cerr << "Failed loading configuration file " << variables_path << std::endl;
        std::cerr << "Error message: " << mn_error_message() << std::endl;
        exit(1);
    }

    if (!mn_exec_file(m_context.get(), script_path.c_str())) {
        std::cerr << "Failed loading configuration file " << script_path << std::endl;
        std::cerr << "Error message: " << mn_error_message() << std::endl;
        exit(1);
    }

    m_integers["EMPTY"] = -1;
}

int KulkiConfig::get_integer(const std::string& key)
{
    auto it = m_integers.find(key);
    if (it != end(m_integers)) {
        return it->second;
    }

    int value = m_load_int(key);
    m_integers.insert({ key, value });
    return value;
}

double KulkiConfig::get_real(const std::string& key)
{
    auto it = m_reals.find(key);
    if (it != end(m_reals)) {
        return it->second;
    }

    double value = m_load_real(key);
    m_reals.insert({ key, value });
    return value;
}

dick::Color KulkiConfig::get_color(const std::string& key)
{
    auto it = m_colors.find(key);
    if (it != end(m_colors)) {
        return it->second;
    }

    dick::Color value = m_load_color(key);
    m_colors.insert({ key, value });
    return value;
}

std::vector<dick::Color> KulkiConfig::get_color_range(const std::string& key)
{
    auto it = m_color_ranges.find(key);
    if (it != end(m_color_ranges)) {
        return it->second;
    }

    std::vector<dick::Color> value = m_load_color_range(key);
    m_color_ranges.insert({ key, value });
    return value;
}

glm::vec3 KulkiConfig::get_vec(const std::string& key)
{
    auto it = m_vecs.find(key);
    if (it != end(m_vecs)) {
        return it->second;
    }

    glm::vec3 value = m_load_vec3(key);
    m_vecs.insert({ key, value });
    return value;
}

void KulkiConfig::save_variables(int board_w, int board_h, int default_color_count)
{
    std::ofstream out(m_var_filename, std::ios_base::trunc);
    out << "(bind BOARD_W " << board_w << ")" << std::endl;
    out << "(bind BOARD_H " << board_h << ")" << std::endl;
    out << "(bind DEFAULT_COLOR_COUNT " << default_color_count << ")" << std::endl;
}

