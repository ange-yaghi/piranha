#include "../include/node_program.h"

#include "../include/node.h"
#include "../include/ir_context_tree.h"
#include "../include/key_value_lookup.h"
#include "../include/ir_node.h"
#include "../include/ir_node_definition.h"

#include <fstream>

piranha::NodeProgram::NodeProgram() {
    /* void */
}

piranha::NodeProgram::~NodeProgram() {
    /* void */
}

void piranha::NodeProgram::writeAssembly(const std::string &fname) const {
    KeyValueLookup<NodeOutput *, int> m_outputLookup;
    int currentIndex = 0;

    int nodeCount = m_topLevelContainer.getNodeCount();

    for (int i = 0; i < nodeCount; i++) {
        Node *node = m_topLevelContainer.getNode(i);

        int nodeInputs = node->getInputCount();
        for (int i = 0; i < nodeInputs; i++) {
            const Node::NodeInputPort *port = node->getInput(i);
            int *index = m_outputLookup.lookup(*port->input);

            if (index == nullptr) {
                *m_outputLookup.newValue(*port->input) = currentIndex++;
            }
        }

        int nodeOutputs = node->getOutputCount();
        for (int i = 0; i < nodeOutputs; i++) {
            const Node::NodeOutputPort *port = node->getOutput(i);
            int *index = m_outputLookup.lookup(port->output);

            if (index == nullptr) {
                *m_outputLookup.newValue(port->output) = currentIndex++;
            }
        }

        int nodeOutputReferences = node->getOutputReferenceCount();
        for (int i = 0; i < nodeOutputReferences; i++) {
            const Node::NodeOutputPortReference *port = node->getOutputReference(i);
            int *index = m_outputLookup.lookup(*port->output);

            if (index == nullptr) {
                *m_outputLookup.newValue(*port->output) = currentIndex++;
            }
        }
    }

    std::ofstream file(fname);

    for (int i = 0; i < nodeCount; i++) {
        Node *node = m_topLevelContainer.getNode(i);
        std::string builtinName = node->getBuiltinName();

        file << builtinName << "\n";

        int nodeInputs = node->getInputCount();
        if (nodeInputs > 0) {
            file << "    INPUTS  { ";
            for (int i = 0; i < nodeInputs; i++) {
                const Node::NodeInputPort *port = node->getInput(i);
                int index = *m_outputLookup.lookup(*port->input);

                file << port->name << ": &" << index << "; ";
            }
            file << "}\n";
        }

        int nodeOutputs = node->getOutputCount();
        int nodeOutputReferences = node->getOutputReferenceCount();

        if (nodeOutputReferences > 0 || nodeOutputs > 0) {
            file << "    OUTPUTS { ";
            for (int i = 0; i < nodeOutputs; i++) {
                const Node::NodeOutputPort *port = node->getOutput(i);
                int index = *m_outputLookup.lookup(port->output);

                file << port->name << ": @" << index << "; ";
            }

            for (int i = 0; i < nodeOutputReferences; i++) {
                const Node::NodeOutputPortReference *port = node->getOutputReference(i);
                int index = *m_outputLookup.lookup(*port->output);

                file << port->name << ": &" << index << "; ";
            }
            file << "}\n";
        }
    }

    file.close();
}

void piranha::NodeProgram::addNode(Node *node) {
    m_nodeCache.push_back(node);
}

void piranha::NodeProgram::addContainer(IrContextTree *context, NodeContainer *container) {
    *m_containers.newValue(context) = container;
}

piranha::NodeContainer *piranha::NodeProgram::getContainer(IrContextTree *context) {
     NodeContainer **container = m_containers.lookup(context);
     if (container == nullptr) return nullptr;
     else return *container;
}

piranha::Node *piranha::NodeProgram::getCachedInstance(IrParserStructure *ir, IrContextTree *context) {
    int nodeCount = getNodeCount();
    for (int i = 0; i < nodeCount; i++) {
        if (m_nodeCache[i]->getIrStructure() == ir) {
            if (m_nodeCache[i]->getContext()->isEqual(context)) {
                return m_nodeCache[i];
            }
        }
    }

    return nullptr;
}

void piranha::NodeProgram::execute() {
    int nodeCount = m_topLevelContainer.getNodeCount();

    // Initialize all nodes
    for (int i = 0; i < nodeCount; i++) {
        Node *node = m_topLevelContainer.getNode(i);
        node->initialize();
    }

    for (int i = 0; i < nodeCount; i++) {
        Node *node = m_topLevelContainer.getNode(i);
        node->evaluate();
    }
}
