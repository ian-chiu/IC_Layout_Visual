#pragma once

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

    void createDataBase(const std::string &db_name);

    void Connect(const std::string &db_name);

    void GetAllPolygons(std::vector<Polygon> &polygons);

    void GetChipBoundary(Boundary &boundary);

    int GetLayerCount();

    void FilterLayers(std::vector<Polygon> &buffer, const std::vector<bool> &layerIdSet);

    void WriteFromLayout(const fs::path &layout_file_path);

    bool Exists(const std::string &db_name);

private:
    fs::path m_DatabasePath;
};