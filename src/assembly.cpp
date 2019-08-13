#include "../include/assembly.h"

piranha::Assembly::Assembly() {
    m_currentIndex = 0;
}

piranha::Assembly::~Assembly() {
    /* void */
}

int piranha::Assembly::getOutputLabel(NodeOutput *output) {
    int *lookup = m_outputLookup.lookup(output);
    if (lookup != nullptr) return *lookup;
    
    int *newValue = m_outputLookup.newValue(output);
    *newValue = m_currentIndex++;

    return *newValue;
}
