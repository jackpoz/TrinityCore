#include "DetourCLI.h"
#include "MapCLI.h"
#include "SharedDefines.h"
#include <memory>

using namespace MapCLI;

namespace DetourCLI
{
    void Detour::Initialize(String^ mmapsPath)
    {
        mmapsFolderPath = mmapsPath;
        if (!mmapsFolderPath->EndsWith("\\"))
            mmapsFolderPath += "\\";
    }

    Detour::Detour()
    {
        navQuery = dtAllocNavMeshQuery();
        filter = new dtQueryFilter();
    }

    Detour::~Detour()
    {
        dtFreeNavMeshQuery(navQuery);
        delete filter;
    }

    bool Detour::FindPath(float startX, float startY, float startZ, float endX, float endY, float endZ, int mapID, [Out] List<Point^>^% path)
    {
        if (String::IsNullOrEmpty(mmapsFolderPath))
            return false;

        // X,Y,z -> Y,Z,X
        float start[] = { startY, startZ, startX };
        float end[] = { endY, endZ, endX };

        // get the navMesh or initialize it
        dtNavMesh* navMesh = LoadNavMesh(mapID);
        if (navMesh == nullptr)
            return false;

        navQuery->init(navMesh, 2048);

        // check if the tiles have been added already or load them
        if (!LoadTiles(start, mapID, navMesh))
            return false;

        if (!LoadTiles(end, mapID, navMesh))
            return false;

        // check if there is a path
        dtPolyRef startRef;
        dtPolyRef endRef;

        float polyPickExt[] = { 3.0f, 5.0f, 3.0f };

        if (dtStatusFailed(navQuery->findNearestPoly(start, polyPickExt, filter, &startRef, 0)))
            return false;
        if (dtStatusFailed(navQuery->findNearestPoly(end, polyPickExt, filter, &endRef, 0)))
            return false;
        if (startRef == 0 || endRef == 0)
            return false;

        dtPolyRef m_polys[MAX_PATH_LENGTH];
        int m_npolys;
        if (dtStatusFailed(navQuery->findPath(startRef, endRef, start, end, filter, m_polys, &m_npolys, MAX_PATH_LENGTH)))
            return false;
        if (m_npolys == 0)
            return false;

        float pathPoints[MAX_POINT_PATH_LENGTH*VERTEX_SIZE];
        int pointCount;
        if (dtStatusFailed(navQuery->findStraightPath(start, end, m_polys, m_npolys, pathPoints, nullptr, nullptr, &pointCount, MAX_POINT_PATH_LENGTH)))
            return false;

        if (pointCount < 2)
            return false;

        // clean the path up
        path = gcnew List<Point^>();

        Point^ lastPoint = nullptr;
        for (int index = 0; index < pointCount; index++)
        {
            // Y,Z,X -> X,Y,Z
            Point^ currentPoint = gcnew Point(pathPoints[2 + index * VERTEX_SIZE],
                                       pathPoints[0 + index * VERTEX_SIZE],
                                       pathPoints[1 + index * VERTEX_SIZE]);

            if (lastPoint != nullptr)
            {
                Point^ distance = currentPoint - lastPoint;
                Point^ direction = distance->Direction * SMOOTH_PATH_STEP_SIZE;
                int steps = Math::Floor(distance->Length / SMOOTH_PATH_STEP_SIZE);
                for (int step = 0; step < steps; step++)
                {
                    Point^ stepPoint = lastPoint + direction;
                    stepPoint->Z = Map::GetHeight(stepPoint->X, stepPoint->Y, stepPoint->Z, mapID);
                    path->Add(stepPoint);
                    lastPoint = stepPoint;
                }
            }

            currentPoint->Z = Map::GetHeight(currentPoint->X, currentPoint->Y, currentPoint->Z, mapID);
            path->Add(currentPoint);

            lastPoint = currentPoint;
        }

        return true;
    }

    bool Detour::LoadTiles(float position[3], int mapID, dtNavMesh* navMesh)
    {
        // check if the tiles have been added already or load them
        int X, Y;
        int tcX = 32 - position[2] / SIZE_OF_GRIDS;
        int tcY = 32 - position[0] / SIZE_OF_GRIDS;

        navMesh->calcTileLoc(position, &X, &Y);
        for (int row = -1; row <= 1; row++)
        {
            for (int col = -1; col <= 1; col++)
            {
                Map::LoadTile(tcX, tcY, mapID);

                if (navMesh->getTileRefAt(X + row, Y + col, 0) != 0)
                    continue;

                // swap col and row due to different coordinate systems
                String^ tileFilePath = mmapsFolderPath + String::Format("{0:D3}{1:D2}{2:D2}.mmtile", mapID, tcX - col, tcY - row);
                if (!File::Exists(tileFilePath))
                    continue;

                array<unsigned char>^ fileData = File::ReadAllBytes(tileFilePath);
                if (fileData->Length < sizeof(MmapTileHeader))
                    return false;

                pin_ptr<unsigned char> tileDataPinned = &fileData[0];

                MmapTileHeader tileHeader;
                memcpy(&tileHeader, tileDataPinned, sizeof(MmapTileHeader));
                if (tileHeader.mmapMagic != MMAP_MAGIC )
                    return false;
                if (sizeof(MmapTileHeader) + tileHeader.size < fileData->Length)
                    return false;

                unsigned char* data = (unsigned char*)dtAlloc(tileHeader.size, DT_ALLOC_PERM);
                memcpy(data, tileDataPinned + sizeof(MmapTileHeader), tileHeader.size);

                dtTileRef tileRef = 0;
                // memory allocated for data is now managed by detour, and will be deallocated when the tile is removed
                if (dtStatusFailed(navMesh->addTile(data, tileHeader.size, DT_TILE_FREE_DATA, 0, &tileRef)))
                {
                    dtFree(data);
                    return false;
                }
            }
        }

        return true;
    }

    dtNavMesh* Detour::LoadNavMesh(int mapID)
    {
        if (navMeshes[mapID] == nullptr)
        {
            String^ mmapFilePath = mmapsFolderPath + String::Format("{0:D3}.mmap", mapID);
            if (!File::Exists(mmapFilePath))
                return nullptr;
            array<unsigned char>^ mmapData = File::ReadAllBytes(mmapFilePath);
            if (mmapData->Length < sizeof(dtNavMeshParams))
                return nullptr;

            dtNavMeshParams params;
            pin_ptr<unsigned char> mmapDataPinned = &mmapData[0];
            memcpy(&params, mmapDataPinned, sizeof(dtNavMeshParams));

            dtNavMesh* mesh = dtAllocNavMesh();
            if (dtStatusFailed(mesh->init(&params)))
            {
                dtFreeNavMesh(mesh);
                return nullptr;
            }

            navMeshes[mapID] = mesh;
        }

        return navMeshes[mapID];
    }
}