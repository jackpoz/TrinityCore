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
        static const int DEFAULT_HEIGHT_SEARCH = 50.0f;
        static const float SAFE_Z_HIGHER_BIAS = 2.0f;

        static void Initialize(String^ vmapsPath);
        static void LoadTile(int tileX, int tileY, int mapID);
        static float GetHeight(float X, float Y, float Z, int mapID);

    private:
        static IVMapManager* vmapManager;
        static property String^ vmapsFolderPath
        {
            String^ get()
            {
                return _vmapsFolderPath;
            }
            void set(String^ value)
            {
                _vmapsFolderPath = value;
            }
        }
        static String^ _vmapsFolderPath;
        static Object^ _vmapManagerLock;

        static VMap()
        {
            vmapManager = new VMapManager2();
        };
    };
}
