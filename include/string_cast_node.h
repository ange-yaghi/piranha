#ifndef PIRANHA_STRING_CAST_NODE_H
#define PIRANHA_STRING_CAST_NODE_H

#include "node.h"

#include <sstream>

namespace piranha {

    class StringCastNode : public Node {
    public:
        StringCastNode();
        ~StringCastNode();

        virtual void _initialize();
        virtual void _evaluate();
        virtual void _destroy();

        virtual void registerOutputs();
        virtual void registerInputs();

    protected:
        pNodeInput m_input;
    };

} /* namespace piranha */

#endif /* PIRANHA_STRING_CAST_NODE_H */