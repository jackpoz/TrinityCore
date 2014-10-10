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
    public ref class VMap
    {
    public:
        VMap();
        ~VMap();

    private:
        static IVMapManager* vmapManager;
        static VMap()
        {
            vmapManager = new VMapManager2();
        };
    };
}
