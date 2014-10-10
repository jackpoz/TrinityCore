#include "VMapCLI.h"

namespace VMapCLI
{
    void VMap::Initialize(String^ vmapsPath)
    {
        vmapsFolderPath = vmapsPath;
        if (!vmapsFolderPath->EndsWith("\\"))
            vmapsFolderPath += "\\";
    }

    void VMap::LoadTiles(float X, float Y, float Z, int mapID)
    {
        throw gcnew NotImplementedException();
    }

    float VMap::GetHeight(float X, float Y, float Z, int mapID)
    {
        throw gcnew NotImplementedException();
    }
}
