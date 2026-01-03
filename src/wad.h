#pragma once

namespace WAD {
    typedef struct
    {
            char identification[4]; // should be WAD2/WAD3 or 2DAW/3DAW
            int numlumps;
            int infotableofs;
    } wadinfo_t;
} // namespace WAD
