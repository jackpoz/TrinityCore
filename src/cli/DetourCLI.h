// DetourCLI.h

#pragma once

using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

#include "DetourNavMeshQuery.h"
#include "DetourNavMeshBuilder.h"
#include "DetourCommon.h"

namespace DetourCLI
{
    public ref struct Point
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

        property Point^ Direction
        {
            Point^ get()
            {
                float length = Length;
                return gcnew Point(X / length, Y / length, Z / length);
            }
        }

        property float DirectionOrientation
        {
            float get()
            {
                auto dir = Direction;
                double orientation = Math::Atan2(dir->Y, dir->X);
                if (orientation < 0)
                    orientation += 2. * Math::PI;
                return (float)orientation;
            }
        }

        static Point^ operator +(const Point^ a, const Point^ b)
        {
            return gcnew Point(a->X + b->X, a->Y + b->Y, a->Z + b->Z);
        }

        static Point^ operator -(const Point^ a, const Point^ b)
        {
            return gcnew Point(a->X - b->X, a->Y - b->Y, a->Z - b->Z);
        }

        static Point^ operator *(const Point^ point, const float scale)
        {
            return gcnew Point(point->X * scale, point->Y * scale, point->Z * scale);
        }

        float X, Y, Z;
    };

	public ref class Detour
	{
    public:
        static const int MAX_PATH_LENGTH = 74;
        static const int MAX_POINT_PATH_LENGTH = 74;
        static const int VERTEX_SIZE = 3;
        static const float SMOOTH_PATH_STEP_SIZE = 4.0f;
        static const int MAX_MAP_ID = 724 + 1;
        static const float SIZE_OF_GRIDS = 533.3333f;

        bool FindPath(float startX, float startY, float startZ, float endX, float endY, float endZ, int mapID, [Out] List<Point^>^% path);

        Detour();
        ~Detour();

        static void Initialize(String^ mmapsPath);

    private:
        static bool LoadTiles(float position[3], int mapID, dtNavMesh* navMesh);
        static dtNavMesh* LoadNavMesh(int mapID);
        dtNavMeshQuery* navQuery;
        dtQueryFilter* filter;

        static String^ mmapsFolderPath;
        static array<IntPtr^>^ navMeshes;
        static Detour()
        {
            navMeshes = gcnew array<IntPtr^>(MAX_MAP_ID);
        };
	};
}
