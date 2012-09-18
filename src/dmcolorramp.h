#ifndef DM_DMCOLORRAMP_H
#define DM_DMCOLORRAMP_H

namespace DM {

enum ColorRamp {
    BLACK_TO_RED,
    WHITE_TO_RED,
    
    BLACK_TO_GREEN,
    WHITE_TO_GREEN,
    
    BLACK_TO_BLUE,
    WHITE_TO_BLUE,
    
    BLACK_TO_YELLOW,
    WHITE_TO_YELLOW,
    
    BLACK_TO_ORANGE,
    WHITE_TO_ORANGE,
    
    WHITE_TO_BLACK,
    RAINBOW
};

unsigned int get_color_ramp(ColorRamp ramp);

} // namespace DM

#endif // DM_DMCOLORRAMP_H
