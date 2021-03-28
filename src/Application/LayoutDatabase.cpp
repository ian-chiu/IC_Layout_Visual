#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include "Base/Core.h"
#include "LayoutDatabase.h"
#include "sqlite_modern_cpp.h"

void LayoutDatabase::createDataBase(const std::string &db_name)
{
    m_DatabasePath.assign("../../database/" + db_name);
    sqlite::database db(m_DatabasePath.string());
    db  <<  "CREATE TABLE POLYGONS("
                    "ID INT PRIMARY   KEY      NOT NULL,"
                    "BL_X             INT      NOT NULL,"
                    "BL_Z             INT      NOT NULL,"
                    "TR_X             INT      NOT NULL,"
                    "TR_Z             INT      NOT NULL,"
                    "NET_ID           INT      NOT NULL,"
                    "LAYER_ID         INT      NOT NULL,"
                    "TYPE             TEXT     NOT NULL,"
                    "UNIQUE(ID)"
                ");";

    db  <<  "CREATE TABLE CHIP_BOUNDARY("
                    "BL_X             INT      NOT NULL,"
                    "BL_Z             INT      NOT NULL,"
                    "TR_X             INT      NOT NULL,"
                    "TR_Z             INT      NOT NULL"
                ");";   
}

void LayoutDatabase::Connect(const std::string &db_name)
{
    m_DatabasePath.assign("../../database/" + db_name);
}

void LayoutDatabase::GetAllPolygons(std::vector<Polygon> &polygons)
{
    polygons.clear();
    sqlite::database db(m_DatabasePath.string());
    int id, blx, blz, trx, trz, netid, layerid;
    std::string poly_type;
    db << u"SELECT * FROM POLYGONS;" >>  
        [&](int id, int blx, int blz, int trx, int trz, int netid, int layerid, const std::string &poly_type) {
            Polygon polygon;
            polygon.NetId = netid;
            polygon.LayerId = layerid;
            polygon.PolyType = poly_type;
            glm::vec2 bl(blx, blz);
            glm::vec2 tr(trx, trz);
            polygon.Boundary.reset(bl, tr);
            polygons.push_back(polygon);
        };
}

void LayoutDatabase::GetChipBoundary(Boundary &boundary) 
{
    sqlite::database db(m_DatabasePath.string());
    int blx, blz, trx, trz;
    std::string poly_type;
    db << u"SELECT * FROM CHIP_BOUNDARY;" >>  
        [&](int blx, int blz, int trx, int trz) {
            glm::vec2 bl(blx, blz);
            glm::vec2 tr(trx, trz);
            boundary.reset(bl, tr);
        };
}

int LayoutDatabase::GetLayerCount() 
{
    sqlite::database db(m_DatabasePath.string());
    int result;
    db << "SELECT MAX(LAYER_ID) FROM POLYGONS;" >> result;
    return result;
}

void LayoutDatabase::FilterLayers(std::vector<Polygon> &polygons, const std::vector<bool> &layerIdSet)
{
    polygons.clear();

    std::vector<int> filterIds{};
    for (int i = 0; i < layerIdSet.size(); i++)
        if (layerIdSet[i])
            filterIds.push_back(i + 1);

    if (filterIds.empty())
        return;
    
    sqlite::database db(m_DatabasePath.string());
    int id, blx, blz, trx, trz, netId, layerId;
    std::string poly_type;

    std::string query = "SELECT * FROM POLYGONS WHERE LAYER_ID IN (?";
    for (int i = 0; i < filterIds.size() - 1; i++)
        query += ",?";
    query += ");";

    auto ps = db << query;
    for (int id : filterIds)
        ps << id;

    ps >> [&](int id, int blx, int blz, int trx, int trz, int netId, int layerId, const std::string &poly_type) {
            Polygon polygon;
            polygon.NetId = netId;
            polygon.LayerId = layerId;
            polygon.PolyType = poly_type;
            glm::vec2 bl(blx, blz);
            glm::vec2 tr(trx, trz);
            polygon.Boundary.reset(bl, tr);
            polygons.push_back(polygon);
        };
}

void LayoutDatabase::WriteFromLayout(const fs::path &layout_file_path)
{
    sqlite::database db(m_DatabasePath.string());

    db << "DELETE FROM POLYGONS;";
    db << "DELETE FROM CHIP_BOUNDARY;";

    std::ifstream inputFile(layout_file_path);
    if (!inputFile) 
    {
        std::cout << "Cannot open input file.\n"; 
        exit(EXIT_FAILURE);
    }

    std::string line;

    try
    {
        db << "begin;";

        while (std::getline(inputFile, line))
        {
            std::istringstream iss{ line };
            std::string token;
            iss >> token;
            if (token[0] == ';')
                continue;
            else if (token == "0")
            {
                int blx, blz, trx, trz;
                iss >> blx >> blz >> trx >> trz;
                db << u"INSERT INTO CHIP_BOUNDARY VALUES (?,?,?,?);" // utf16 query string
                    << blx << blz << trx << trz;
            }
            else 
            {
                int id, blx, blz, trx, trz, netid, layerid;
                id = std::stoi(token);
                std::string poly_type;
                iss >> blx >> blz >> trx >> trz >> netid >> layerid >> poly_type;

                db << u"INSERT INTO POLYGONS VALUES (?,?,?,?,?,?,?,?);" // utf16 query string
                    << id << blx << blz << trx << trz << netid << layerid << poly_type;
                
                // std::cout << " " << id << " " << blx << " " << blz << " " << trx << " " 
                //     << trz << " " << netid << " " << layerid << " " << poly_type << "\n";
            }
        }

        db << "commit;";
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        LOG_ERROR("Fail to write layout!");
    }
}

bool LayoutDatabase::Exists(const std::string &db_name)
{
    for (const auto &entry : fs::directory_iterator("../../database")) 
        if (entry.path().filename().string() == db_name)
            return true;
        
    return false;
}