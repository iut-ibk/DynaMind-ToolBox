#ifndef DM_DMCOLORRAMP_H
#define DM_DMCOLORRAMP_H

namespace DM {

enum ColorRamp {
#define X(a,b) a,
#include "colorramps.def"
#undef X
};

const char* const ColorRampNames[] = {
#define X(a,b) b,
#include "colorramps.def"
#undef X
};

unsigned int get_color_ramp(ColorRamp ramp, int rgbadata[256][4]);

} // namespace DM

#endif // DM_DMCOLORRAMP_H
