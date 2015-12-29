#pragma once
#include "DBCStores.h"
#include "MapCLI.h"

using namespace System;
using namespace MapCLI;

namespace DBCStoresCLI
{
    public ref class AchievementExploreLocation
    {
    public:
        AchievementExploreLocation(Point location, uint32 criteriaID)
        {
            this->Location = location;
            this->CriteriaID = criteriaID;
        }

        uint32 CriteriaID;
        Point Location;
    };

    public ref class DBCStores
    {
    public:
        static void Initialize(String^ dbcsPath);
        static void LoadDBCs();
        static List<AchievementExploreLocation^>^ GetAchievementExploreLocations(float x, float y, float z, int mapID);

    private:
        static String^ dbcsFolderPath;
        static bool loaded;
    };
}
