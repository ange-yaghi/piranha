#include "../include/vector_constructor.h"

#include <sstream>

piranha::VectorConstructorOutput::VectorConstructorOutput() {
    /* void */
}

piranha::VectorConstructorOutput::~VectorConstructorOutput() {
    /* void */
}

void piranha::VectorConstructorOutput::fullCompute(void *_target) const {
    piranha::native_float x, y, z, w;
    m_inputX->fullCompute((void *)&x);
    m_inputY->fullCompute((void *)&y);
    m_inputZ->fullCompute((void *)&z);
    m_inputW->fullCompute((void *)&w);

    vector *target = reinterpret_cast<vector *>(_target);
    *target = {x, y, z, w};
}

void piranha::VectorConstructorOutput::registerInputs() {
    registerInput(&m_inputX);
    registerInput(&m_inputY);
    registerInput(&m_inputZ);
    registerInput(&m_inputW);
}
