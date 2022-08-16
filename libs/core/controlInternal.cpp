#include "pxtbase.h"


namespace control {

    /**
    * Used internally
    */
    //%
    void internalOnEvent(int src, int value, Action handler, int flags = 16) {
        registerWithDal(src, value, handler, flags);
    }
}
