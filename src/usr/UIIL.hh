//
// User Implemented Interface Layer (UIIL)
// File: UIIL.hh
// Author: Dylan R. Wagner (drw6528@rit.edu)
// Description:
//      The User Implemented Interface Layer provides additional functionality to the
//      PsyCron system without platform dependence.
//

#ifndef PROJECT_UIIL_HH
#define PROJECT_UIIL_HH

namespace psycron {

typedef struct UIIL{
    uint32_t(*sys_micro_second)();
    uint32_t(*sys_milli_second)();

    void*(*sys_drop_core_clock)();
    void*(*sys_raise_core_clock)();

    // Used to display a message to the user, should be used for debugging
    void(*sys_send_msg)(char* msg);

    // Init these values to zero so we know they havent been implemented yet.
    UIIL()
        : sys_micro_second{0}
        , sys_milli_second{0}
        , sys_drop_core_clock{0}
        , sys_raise_core_clock{0}
        , sys_send_msg{0}
    {
    }
} UIIL;

}

#endif //PROJECT_UIIL_H
