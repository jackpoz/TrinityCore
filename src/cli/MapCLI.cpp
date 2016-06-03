#include <msclr\marshal_cppstd.h>
#include "MapCLI.h"
#include "VMapCLI.h"
#include "Lock.h"

using namespace msclr::interop;

namespace MapCLI
{
    void Map::Initialize(String^ mapsPath)
    {
        mapsFolderPath = mapsPath;
        if (!mapsFolderPath->EndsWith("\\"))
            mapsFolderPath += "\\";
    }

    GridMap* Map::GetTile(int tileX, int tileY, int mapID)
    {
        auto gridMaps = GridMaps->GetOrAdd(mapID, gcnew Func<uint32, array<GridMap*, 2>^ >(CreateGridMap));

        if (gridMaps[tileX, tileY] == nullptr)
        {
            auto lockObject = GridMapsLocks->GetOrAdd(mapID, gcnew Func<uint32, Object^>(CreateGridMapsLock));
            Lock lock(lockObject);
            if (gridMaps[tileX, tileY] == nullptr)
            {
                String^ tileFilePath = mapsFolderPath + String::Format("{0:D3}{1:D2}{2:D2}.map", mapID, tileX, tileY);
                // loading data
                GridMap* gridMap = new GridMap();
                gridMap->loadData(marshal_as<std::string>(tileFilePath).c_str());
                VMapCLI::VMap::LoadTile(tileX, tileY, mapID);
                gridMaps[tileX, tileY] = gridMap;
            }
        }

        return gridMaps[tileX, tileY];
    }

    float Map::GetHeight(float X, float Y, float Z, int mapID)
    {
        float mapHeight = VMAP_INVALID_HEIGHT_VALUE;
        float gridHeight = GetGrid(X, Y, mapID)->getHeight(X, Y);
        if (Z + VMapCLI::VMap::SAFE_Z_HIGHER_BIAS > gridHeight)
            mapHeight = gridHeight;

        float vmapHeight = VMapCLI::VMap::GetHeight(X, Y, Z, mapID);

        float height = Z;

        if (vmapHeight > INVALID_HEIGHT)
        {
            if (mapHeight > INVALID_HEIGHT)
            {
                // we have mapheight and vmapheight and must select more appropriate

                // vmap height above map height
                // or if the distance of the vmap height is less the land height distance
                if (vmapHeight > mapHeight || std::fabs(mapHeight - Z) > std::fabs(vmapHeight - Z))
                    height = vmapHeight;
                else
                    height = mapHeight;                           // better use .map surface height
            }
            else
                height = vmapHeight;                              // we have only vmapHeight (if have)
        }
        else if (mapHeight > INVALID_HEIGHT)
            height = mapHeight;

        return height;
    }

    void Map::GetXYZFromAreaId(uint32 areaId, int mapID, float& x, float& y, float& z)
    {
        x = 0.f; y = 0.f; z = 0.f;
        bool found = false;

        for (uint32 gridX = 0; gridX < MAX_NUMBER_OF_GRIDS && !found; gridX++)
        {
            for (uint32 gridY = 0; gridY < MAX_NUMBER_OF_GRIDS && !found; gridY++)
            {
                if (GetTile(gridX, gridY, mapID)->getXYFromArea(areaId, gridX, gridY, x, y))
                {
                    found = true;
                    z = GetHeight(x, y, -VMAP_INVALID_HEIGHT_VALUE, mapID);
                }
            }
        }
    }

    GridMap* Map::GetGrid(float x, float y, int mapID)
    {
        // half opt method
        int gx = (int)(CENTER_GRID_ID - x / SIZE_OF_GRIDS);                       //grid x
        int gy = (int)(CENTER_GRID_ID - y / SIZE_OF_GRIDS);                       //grid y

        return GetTile(gx, gy, mapID);
    }

    bool GridMap::getXYFromArea(uint16 areaId, uint32 gridX, uint32 gridY, float& x, float& y)
    {
        if (!_areaMap)
            return false;

        for (uint32 index = 0; index < 16 * 16; index++)
        {
            if (_areaMap[index] == areaId)
            {
                int lx = (index / 16) + gridX * 16;
                int ly = (index % 16) + gridY * 16;
                x = SIZE_OF_GRIDS * (CENTER_GRID_ID - (lx + 0.5f) / 16.f);
                y = SIZE_OF_GRIDS * (CENTER_GRID_ID - (ly + 0.5f) / 16.f);
                return true;
            }
        }

        return false;
    }

    #pragma region GridMap code from TC
    u_map_magic MapMagic = { { 'M', 'A', 'P', 'S' } };
    u_map_magic MapVersionMagic = { { 'v', '1', '.', '8' } };
    u_map_magic MapAreaMagic = { { 'A', 'R', 'E', 'A' } };
    u_map_magic MapHeightMagic = { { 'M', 'H', 'G', 'T' } };
    u_map_magic MapLiquidMagic = { { 'M', 'L', 'I', 'Q' } };

    GridMap::GridMap()
    {
        _flags = 0;
        // Area data
        _gridArea = 0;
        _areaMap = NULL;
        // Height level data
        _gridHeight = INVALID_HEIGHT;
        _gridGetHeight = &GridMap::getHeightFromFlat;
        _gridIntHeightMultiplier = 0;
        m_V9 = NULL;
        m_V8 = NULL;
        _maxHeight = nullptr;
        _minHeight = nullptr;
        // Liquid data
        _liquidType = 0;
        _liquidOffX = 0;
        _liquidOffY = 0;
        _liquidWidth = 0;
        _liquidHeight = 0;
        _liquidLevel = INVALID_HEIGHT;
        _liquidEntry = NULL;
        _liquidFlags = NULL;
        _liquidMap = NULL;
    }

    GridMap::~GridMap()
    {
        unloadData();
    }

    bool GridMap::loadData(const char* filename)
    {
        // Unload old data if exist
        unloadData();

        map_fileheader header;
        // Not return error if file not found
        FILE* in = fopen(filename, "rb");
        if (!in)
            return true;

        if (fread(&header, sizeof(header), 1, in) != 1)
        {
            fclose(in);
            return false;
        }

        if (header.mapMagic.asUInt == MapMagic.asUInt && header.versionMagic.asUInt == MapVersionMagic.asUInt)
        {
            // load up area data
            if (header.areaMapOffset && !loadAreaData(in, header.areaMapOffset, header.areaMapSize))
            {
                fclose(in);
                return false;
            }
            // load up height data
            if (header.heightMapOffset && !loadHeightData(in, header.heightMapOffset, header.heightMapSize))
            {
                fclose(in);
                return false;
            }
            // load up liquid data
            if (header.liquidMapOffset && !loadLiquidData(in, header.liquidMapOffset, header.liquidMapSize))
            {
                fclose(in);
                return false;
            }
            fclose(in);
            return true;
        }

        fclose(in);
        return false;
    }

    void GridMap::unloadData()
    {
        delete[] _areaMap;
        delete[] m_V9;
        delete[] m_V8;
        delete[] _maxHeight;
        delete[] _minHeight;
        delete[] _liquidEntry;
        delete[] _liquidFlags;
        delete[] _liquidMap;
        _areaMap = NULL;
        m_V9 = NULL;
        m_V8 = NULL;
        _maxHeight = nullptr;
        _minHeight = nullptr;
        _liquidEntry = NULL;
        _liquidFlags = NULL;
        _liquidMap = NULL;
        _gridGetHeight = &GridMap::getHeightFromFlat;
    }

    bool GridMap::loadAreaData(FILE* in, uint32 offset, uint32 /*size*/)
    {
        map_areaHeader header;
        fseek(in, offset, SEEK_SET);

        if (fread(&header, sizeof(header), 1, in) != 1 || header.fourcc != MapAreaMagic.asUInt)
            return false;

        _gridArea = header.gridArea;
        if (!(header.flags & MAP_AREA_NO_AREA))
        {
            _areaMap = new uint16[16 * 16];
            if (fread(_areaMap, sizeof(uint16), 16 * 16, in) != 16 * 16)
                return false;
        }
        return true;
    }

    bool GridMap::loadHeightData(FILE* in, uint32 offset, uint32 /*size*/)
    {
        map_heightHeader header;
        fseek(in, offset, SEEK_SET);

        if (fread(&header, sizeof(header), 1, in) != 1 || header.fourcc != MapHeightMagic.asUInt)
            return false;

        _gridHeight = header.gridHeight;
        if (!(header.flags & MAP_HEIGHT_NO_HEIGHT))
        {
            if ((header.flags & MAP_HEIGHT_AS_INT16))
            {
                m_uint16_V9 = new uint16[129 * 129];
                m_uint16_V8 = new uint16[128 * 128];
                if (fread(m_uint16_V9, sizeof(uint16), 129 * 129, in) != 129 * 129 ||
                    fread(m_uint16_V8, sizeof(uint16), 128 * 128, in) != 128 * 128)
                    return false;
                _gridIntHeightMultiplier = (header.gridMaxHeight - header.gridHeight) / 65535;
                _gridGetHeight = &GridMap::getHeightFromUint16;
            }
            else if ((header.flags & MAP_HEIGHT_AS_INT8))
            {
                m_uint8_V9 = new uint8[129 * 129];
                m_uint8_V8 = new uint8[128 * 128];
                if (fread(m_uint8_V9, sizeof(uint8), 129 * 129, in) != 129 * 129 ||
                    fread(m_uint8_V8, sizeof(uint8), 128 * 128, in) != 128 * 128)
                    return false;
                _gridIntHeightMultiplier = (header.gridMaxHeight - header.gridHeight) / 255;
                _gridGetHeight = &GridMap::getHeightFromUint8;
            }
            else
            {
                m_V9 = new float[129 * 129];
                m_V8 = new float[128 * 128];
                if (fread(m_V9, sizeof(float), 129 * 129, in) != 129 * 129 ||
                    fread(m_V8, sizeof(float), 128 * 128, in) != 128 * 128)
                    return false;
                _gridGetHeight = &GridMap::getHeightFromFloat;
            }
        }
        else
            _gridGetHeight = &GridMap::getHeightFromFlat;

        if (header.flags & MAP_HEIGHT_HAS_FLIGHT_BOUNDS)
        {
            _maxHeight = new int16[3 * 3];
            _minHeight = new int16[3 * 3];
            if (fread(_maxHeight, sizeof(int16), 3 * 3, in) != 3 * 3 ||
                fread(_minHeight, sizeof(int16), 3 * 3, in) != 3 * 3)
                return false;
        }
        return true;
    }

    bool GridMap::loadLiquidData(FILE* in, uint32 offset, uint32 /*size*/)
    {
        map_liquidHeader header;
        fseek(in, offset, SEEK_SET);

        if (fread(&header, sizeof(header), 1, in) != 1 || header.fourcc != MapLiquidMagic.asUInt)
            return false;

        _liquidType = header.liquidType;
        _liquidOffX = header.offsetX;
        _liquidOffY = header.offsetY;
        _liquidWidth = header.width;
        _liquidHeight = header.height;
        _liquidLevel = header.liquidLevel;

        if (!(header.flags & MAP_LIQUID_NO_TYPE))
        {
            _liquidEntry = new uint16[16 * 16];
            if (fread(_liquidEntry, sizeof(uint16), 16 * 16, in) != 16 * 16)
                return false;

            _liquidFlags = new uint8[16 * 16];
            if (fread(_liquidFlags, sizeof(uint8), 16 * 16, in) != 16 * 16)
                return false;
        }
        if (!(header.flags & MAP_LIQUID_NO_HEIGHT))
        {
            _liquidMap = new float[uint32(_liquidWidth) * uint32(_liquidHeight)];
            if (fread(_liquidMap, sizeof(float), _liquidWidth*_liquidHeight, in) != (uint32(_liquidWidth) * uint32(_liquidHeight)))
                return false;
        }
        return true;
    }

    float GridMap::getHeightFromFlat(float /*x*/, float /*y*/) const
    {
        return _gridHeight;
    }

    float GridMap::getHeightFromFloat(float x, float y) const
    {
        if (!m_V8 || !m_V9)
            return _gridHeight;

        x = MAP_RESOLUTION * (CENTER_GRID_ID - x / SIZE_OF_GRIDS);
        y = MAP_RESOLUTION * (CENTER_GRID_ID - y / SIZE_OF_GRIDS);

        int x_int = (int)x;
        int y_int = (int)y;
        x -= x_int;
        y -= y_int;
        x_int &= (MAP_RESOLUTION - 1);
        y_int &= (MAP_RESOLUTION - 1);

        // Height stored as: h5 - its v8 grid, h1-h4 - its v9 grid
        // +--------------> X
        // | h1-------h2     Coordinates is:
        // | | \  1  / |     h1 0, 0
        // | |  \   /  |     h2 0, 1
        // | | 2  h5 3 |     h3 1, 0
        // | |  /   \  |     h4 1, 1
        // | | /  4  \ |     h5 1/2, 1/2
        // | h3-------h4
        // V Y
        // For find height need
        // 1 - detect triangle
        // 2 - solve linear equation from triangle points
        // Calculate coefficients for solve h = a*x + b*y + c

        float a, b, c;
        // Select triangle:
        if (x + y < 1)
        {
            if (x > y)
            {
                // 1 triangle (h1, h2, h5 points)
                float h1 = m_V9[(x_int)* 129 + y_int];
                float h2 = m_V9[(x_int + 1) * 129 + y_int];
                float h5 = 2 * m_V8[x_int * 128 + y_int];
                a = h2 - h1;
                b = h5 - h1 - h2;
                c = h1;
            }
            else
            {
                // 2 triangle (h1, h3, h5 points)
                float h1 = m_V9[x_int * 129 + y_int];
                float h3 = m_V9[x_int * 129 + y_int + 1];
                float h5 = 2 * m_V8[x_int * 128 + y_int];
                a = h5 - h1 - h3;
                b = h3 - h1;
                c = h1;
            }
        }
        else
        {
            if (x > y)
            {
                // 3 triangle (h2, h4, h5 points)
                float h2 = m_V9[(x_int + 1) * 129 + y_int];
                float h4 = m_V9[(x_int + 1) * 129 + y_int + 1];
                float h5 = 2 * m_V8[x_int * 128 + y_int];
                a = h2 + h4 - h5;
                b = h4 - h2;
                c = h5 - h4;
            }
            else
            {
                // 4 triangle (h3, h4, h5 points)
                float h3 = m_V9[(x_int)* 129 + y_int + 1];
                float h4 = m_V9[(x_int + 1) * 129 + y_int + 1];
                float h5 = 2 * m_V8[x_int * 128 + y_int];
                a = h4 - h3;
                b = h3 + h4 - h5;
                c = h5 - h4;
            }
        }
        // Calculate height
        return a * x + b * y + c;
    }

    float GridMap::getHeightFromUint8(float x, float y) const
    {
        if (!m_uint8_V8 || !m_uint8_V9)
            return _gridHeight;

        x = MAP_RESOLUTION * (CENTER_GRID_ID - x / SIZE_OF_GRIDS);
        y = MAP_RESOLUTION * (CENTER_GRID_ID - y / SIZE_OF_GRIDS);

        int x_int = (int)x;
        int y_int = (int)y;
        x -= x_int;
        y -= y_int;
        x_int &= (MAP_RESOLUTION - 1);
        y_int &= (MAP_RESOLUTION - 1);

        int32 a, b, c;
        uint8 *V9_h1_ptr = &m_uint8_V9[x_int * 128 + x_int + y_int];
        if (x + y < 1)
        {
            if (x > y)
            {
                // 1 triangle (h1, h2, h5 points)
                int32 h1 = V9_h1_ptr[0];
                int32 h2 = V9_h1_ptr[129];
                int32 h5 = 2 * m_uint8_V8[x_int * 128 + y_int];
                a = h2 - h1;
                b = h5 - h1 - h2;
                c = h1;
            }
            else
            {
                // 2 triangle (h1, h3, h5 points)
                int32 h1 = V9_h1_ptr[0];
                int32 h3 = V9_h1_ptr[1];
                int32 h5 = 2 * m_uint8_V8[x_int * 128 + y_int];
                a = h5 - h1 - h3;
                b = h3 - h1;
                c = h1;
            }
        }
        else
        {
            if (x > y)
            {
                // 3 triangle (h2, h4, h5 points)
                int32 h2 = V9_h1_ptr[129];
                int32 h4 = V9_h1_ptr[130];
                int32 h5 = 2 * m_uint8_V8[x_int * 128 + y_int];
                a = h2 + h4 - h5;
                b = h4 - h2;
                c = h5 - h4;
            }
            else
            {
                // 4 triangle (h3, h4, h5 points)
                int32 h3 = V9_h1_ptr[1];
                int32 h4 = V9_h1_ptr[130];
                int32 h5 = 2 * m_uint8_V8[x_int * 128 + y_int];
                a = h4 - h3;
                b = h3 + h4 - h5;
                c = h5 - h4;
            }
        }
        // Calculate height
        return (float)((a * x) + (b * y) + c)*_gridIntHeightMultiplier + _gridHeight;
    }

    float GridMap::getHeightFromUint16(float x, float y) const
    {
        if (!m_uint16_V8 || !m_uint16_V9)
            return _gridHeight;

        x = MAP_RESOLUTION * (CENTER_GRID_ID - x / SIZE_OF_GRIDS);
        y = MAP_RESOLUTION * (CENTER_GRID_ID - y / SIZE_OF_GRIDS);

        int x_int = (int)x;
        int y_int = (int)y;
        x -= x_int;
        y -= y_int;
        x_int &= (MAP_RESOLUTION - 1);
        y_int &= (MAP_RESOLUTION - 1);

        int32 a, b, c;
        uint16 *V9_h1_ptr = &m_uint16_V9[x_int * 128 + x_int + y_int];
        if (x + y < 1)
        {
            if (x > y)
            {
                // 1 triangle (h1, h2, h5 points)
                int32 h1 = V9_h1_ptr[0];
                int32 h2 = V9_h1_ptr[129];
                int32 h5 = 2 * m_uint16_V8[x_int * 128 + y_int];
                a = h2 - h1;
                b = h5 - h1 - h2;
                c = h1;
            }
            else
            {
                // 2 triangle (h1, h3, h5 points)
                int32 h1 = V9_h1_ptr[0];
                int32 h3 = V9_h1_ptr[1];
                int32 h5 = 2 * m_uint16_V8[x_int * 128 + y_int];
                a = h5 - h1 - h3;
                b = h3 - h1;
                c = h1;
            }
        }
        else
        {
            if (x > y)
            {
                // 3 triangle (h2, h4, h5 points)
                int32 h2 = V9_h1_ptr[129];
                int32 h4 = V9_h1_ptr[130];
                int32 h5 = 2 * m_uint16_V8[x_int * 128 + y_int];
                a = h2 + h4 - h5;
                b = h4 - h2;
                c = h5 - h4;
            }
            else
            {
                // 4 triangle (h3, h4, h5 points)
                int32 h3 = V9_h1_ptr[1];
                int32 h4 = V9_h1_ptr[130];
                int32 h5 = 2 * m_uint16_V8[x_int * 128 + y_int];
                a = h4 - h3;
                b = h3 + h4 - h5;
                c = h5 - h4;
            }
        }
        // Calculate height
        return (float)((a * x) + (b * y) + c)*_gridIntHeightMultiplier + _gridHeight;
    }
    #pragma endregion

}