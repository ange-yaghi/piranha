#ifndef PIRANHA_NODE_PROGRAM_H
#define PIRANHA_NODE_PROGRAM_H

#include "node_container.h"

#include <vector>

namespace piranha {

    class Node;
    class LanguageRules;
    class IrParserStructure;
    class IrContextTree;

    class NodeProgram {
    public:
        NodeProgram();
        ~NodeProgram();
        void execute();

        void writeAssembly(const std::string &fname) const;

        NodeContainer *getTopLevelContainer() { return &m_topLevelContainer; }

    protected:
        NodeContainer m_topLevelContainer;
    };

} /* namespace piranha */

#endif /* PIRANHA_NODE_PROGRAM_H */
