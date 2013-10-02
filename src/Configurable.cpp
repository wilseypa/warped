
#include "Configurable.h"

vector<Configurer*>&
Configurable::getConfigurersEmpty() {
    static vector<Configurer*>& retval = *new vector<Configurer*>;
    return retval;
}
