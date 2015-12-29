// DetourCLI.h

#pragma once

using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

#include "DetourNavMeshQuery.h"
#include "DetourNavMeshBuilder.h"
#include "DetourCommon.h"
#include "MapCLI.h"

using namespace MapCLI;

namespace DetourCLI
{
    public enum class PathType
    {
        None,
        Complete,
        Partial
    };

	public ref class Detour
	{
    public:
        static const int MAX_PATH_LENGTH = 74;
        static const int MAX_POINT_PATH_LENGTH = 74;
        static const int VERTEX_SIZE = 3;
        static const float SMOOTH_PATH_STEP_SIZE = 3.0f;
        static const int MAX_MAP_ID = 724 + 1;

        PathType FindPath(float startX, float startY, float startZ, float endX, float endY, float endZ, int mapID, [Out] List<Point>^% path);

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
