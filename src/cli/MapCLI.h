// MapCLI.h

#pragma once

using namespace System;
using namespace System::IO;
using namespace System::Collections::Concurrent;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

#include "Define.h"
#include <cstdio>

namespace MapCLI
{
    union u_map_magic
    {
        char asChar[4]; ///> Non-null terminated string
        uint32 asUInt;  ///> uint32 representation
    };

    struct map_fileheader
    {
        u_map_magic mapMagic;
        u_map_magic versionMagic;
        u_map_magic buildMagic;
        uint32 areaMapOffset;
        uint32 areaMapSize;
        uint32 heightMapOffset;
        uint32 heightMapSize;
        uint32 liquidMapOffset;
        uint32 liquidMapSize;
        uint32 holesOffset;
        uint32 holesSize;
    };

    #define MAP_AREA_NO_AREA        0x0001

    struct map_areaHeader
    {
        uint32 fourcc;
        uint16 flags;
        uint16 gridArea;
    };

    #define MAP_HEIGHT_NO_HEIGHT    0x0001
    #define MAP_HEIGHT_AS_INT16     0x0002
    #define MAP_HEIGHT_AS_INT8      0x0004

    struct map_heightHeader
    {
        uint32 fourcc;
        uint32 flags;
        float  gridHeight;
        float  gridMaxHeight;
    };

    #define MAP_LIQUID_NO_TYPE      0x0001
    #define MAP_LIQUID_NO_HEIGHT    0x0002

    struct map_liquidHeader
    {
        uint32 fourcc;
        uint16 flags;
        uint16 liquidType;
        uint8  offsetX;
        uint8  offsetY;
        uint8  width;
        uint8  height;
        float  liquidLevel;
    };

    enum ZLiquidStatus
    {
        LIQUID_MAP_NO_WATER = 0x00000000,
        LIQUID_MAP_ABOVE_WATER = 0x00000001,
        LIQUID_MAP_WATER_WALK = 0x00000002,
        LIQUID_MAP_IN_WATER = 0x00000004,
        LIQUID_MAP_UNDER_WATER = 0x00000008
    };

    struct LiquidData
    {
        uint32 type_flags;
        uint32 entry;
        float  level;
        float  depth_level;
    };

    class GridMap
    {
        uint32  _flags;
        union{
            float* m_V9;
            uint16* m_uint16_V9;
            uint8* m_uint8_V9;
        };
        union{
            float* m_V8;
            uint16* m_uint16_V8;
            uint8* m_uint8_V8;
        };
        // Height level data
        float _gridHeight;
        float _gridIntHeightMultiplier;

        // Area data
        uint16* _areaMap;

        // Liquid data
        float _liquidLevel;
        uint16* _liquidEntry;
        uint8* _liquidFlags;
        float* _liquidMap;
        uint16 _gridArea;
        uint16 _liquidType;
        uint8 _liquidOffX;
        uint8 _liquidOffY;
        uint8 _liquidWidth;
        uint8 _liquidHeight;


        bool loadAreaData(FILE* in, uint32 offset, uint32 size);
        bool loadHeightData(FILE* in, uint32 offset, uint32 size);
        bool loadLiquidData(FILE* in, uint32 offset, uint32 size);

        // Get height functions and pointers
        typedef float (GridMap::*GetHeightPtr) (float x, float y) const;
        GetHeightPtr _gridGetHeight;
        float getHeightFromFloat(float x, float y) const;
        float getHeightFromUint16(float x, float y) const;
        float getHeightFromUint8(float x, float y) const;
        float getHeightFromFlat(float x, float y) const;

    public:
        GridMap();
        ~GridMap();
        bool loadData(const char* filename);
        void unloadData();
        bool getXYFromArea(uint16 areaId, uint32 gridX, uint32 gridY, float& x, float& y);

        inline float getHeight(float x, float y) const { return (this->*_gridGetHeight)(x, y); }
    };

    #define INVALID_HEIGHT          -100000.0f
    #define MAX_NUMBER_OF_GRIDS     64
    #define SIZE_OF_GRIDS           533.3333f
    #define CENTER_GRID_ID          (MAX_NUMBER_OF_GRIDS/2)
    #define MAP_RESOLUTION          128

    public ref class Map abstract sealed
    {
    public:
        static void Initialize(String^ mapsPath);
        static GridMap* GetTile(int tileX, int tileY, int mapID);
        static float GetHeight(float X, float Y, float Z, int mapID);
        static void GetXYZFromAreaId(uint32 areaId, int mapID, float& x, float& y, float& z);

    private:
        static String^ mapsFolderPath;
        static ConcurrentDictionary<uint32, array<GridMap*, 2>^>^ GridMaps;
        static ConcurrentDictionary<uint32, Object^>^ GridMapsLocks;
        static GridMap* GetGrid(float X, float Y, int mapID);

        static Map()
        {
            GridMaps = gcnew ConcurrentDictionary<uint32, array<GridMap*, 2>^>();
            GridMapsLocks = gcnew ConcurrentDictionary<uint32, Object^>();
        };

        static array<GridMap*, 2>^ CreateGridMap(uint32 mapID)
        {
            return gcnew array<GridMap*, 2>(MAX_NUMBER_OF_GRIDS, MAX_NUMBER_OF_GRIDS);
        }

        static Object^ CreateGridMapsLock(uint32 mapID)
        {
            return gcnew Object();
        }
    };

    public value struct Point
    {
    public:
        Point(float x, float y, float z) : X(x), Y(y), Z(z)
        {}

        property float Length
        {
            float get()
            {
                return (float)Math::Sqrt(X * X + Y * Y + Z * Z);
            }
        }

        property Point Direction
        {
            Point get()
            {
                float length = Length;
                return Point(X / length, Y / length, Z / length);
            }
        }

        property float DirectionOrientation
        {
            float get()
            {
                auto dir = Direction;
                double orientation = Math::Atan2(dir.Y, dir.X);
                if (orientation < 0)
                    orientation += 2. * Math::PI;
                return (float)orientation;
            }
        }

        static Point operator +(const Point a, const Point b)
        {
            return Point(a.X + b.X, a.Y + b.Y, a.Z + b.Z);
        }

        static Point operator -(const Point a, const Point b)
        {
            return Point(a.X - b.X, a.Y - b.Y, a.Z - b.Z);
        }

        static Point operator *(const Point point, const float scale)
        {
            return Point(point.X * scale, point.Y * scale, point.Z * scale);
        }

        static bool operator ==(const Point a, const Point b)
        {
            return a.X == b.X && a.Y == b.Y && a.Z == b.Z;
        }

        String^ ToString() override
        {
            return "X: " + X + " | Y: " + Y + " | Z: " + Z;
        }

        float X, Y, Z;
    };
}
