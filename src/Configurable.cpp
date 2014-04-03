
#include "Configurable.h"

std::vector<Configurer*>&
Configurable::getConfigurersEmpty() {
    static std::vector<Configurer*>& retval = *new std::vector<Configurer*>;
    return retval;
}
