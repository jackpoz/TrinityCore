// VMapCLI.h

#pragma once

using namespace System;
using namespace System::IO;
using namespace System::Collections::Generic;
using namespace System::Runtime::InteropServices;

#include "VMapManager2.h"
using namespace VMAP;

namespace VMapCLI
{
    public ref class VMap abstract sealed
    {
    public:
        static void Initialize(String^ vmapsPath);
        static void LoadTiles(float X, float Y, float Z, int mapID);
        static float GetHeight(float X, float Y, float Z, int mapID);

    private:
        static IVMapManager* vmapManager;
        static String^ vmapsFolderPath;
        static VMap()
        {
            vmapManager = new VMapManager2();
        };
    };
}
