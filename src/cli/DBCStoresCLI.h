#pragma once
#include "DBCStores.h"
#include "MapCLI.h"

using namespace System;
using namespace MapCLI;

namespace DBCStoresCLI
{
    public ref class DBCStores
    {
    public:
        static void Initialize(String^ dbcsPath);
        static void LoadDBCs();
        static List<Point^>^ GetAchievementExploreLocations(float x, float y, float z, int mapID);

    private:
        static String^ dbcsFolderPath;
        static bool loaded;
    };
}
