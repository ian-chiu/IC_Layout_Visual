#pragma once

#include <memory>
#include <filesystem>
#include <string>
#include <vector>
#include "LayoutProps.h"
#include "sqlite_modern_cpp.h"

namespace fs = std::filesystem;

class LayoutDatabase
{
public:
    LayoutDatabase() = default;

    void create_database(const std::string &db_name);

    void connect(const std::string &db_name);

    void get_all_polygons(std::vector<Polygon> &polygons);

    void get_chip_boundary(Boundary &boundary);

    void filter_layer(std::vector<Polygon> &buffer, int layerId);

    void write_from_layout(const fs::path &layout_file_path);

    bool exists(const std::string &db_name);

private:
    std::unique_ptr<sqlite::database> m_database_ptr;
};