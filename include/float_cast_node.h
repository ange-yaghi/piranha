#ifndef PIRANHA_FLOAT_CAST_NODE_H
#define PIRANHA_FLOAT_CAST_NODE_H

#include "node.h"

#include <sstream>

namespace piranha {

    class FloatCastNode : public Node {
    public:
        FloatCastNode();
        ~FloatCastNode();

        virtual void _initialize();
        virtual void _evaluate();
        virtual void _destroy();

        virtual void registerOutputs();
        virtual void registerInputs();

    protected:
        pNodeInput m_input;
    };

} /* namespace piranha */

#endif /* PIRANHA_FLOAT_CAST_NODE_H */

