#include "../include/node_program.h"

#include "../include/node.h"
#include "../include/ir_context_tree.h"
#include "../include/key_value_lookup.h"
#include "../include/ir_node.h"
#include "../include/ir_node_definition.h"

#include <fstream>

piranha::NodeProgram::NodeProgram() {
    m_rules = nullptr;
}

piranha::NodeProgram::~NodeProgram() {
    /* void */
}

void piranha::NodeProgram::addNode(Node *node) {
    node->setProgram(this);
    m_nodes.push_back(node);
}

piranha::Node *piranha::NodeProgram::
	getCachedInstance(IrParserStructure *ir, IrContextTree *context)
{
	// TODO: this lookup could be made faster by having the lookup table be a tree
	// with the first level being a lookup by node and the second by context

	int nodeCount = getNodeCount();
	for (int i = 0; i < nodeCount; i++) {
		if (m_nodes[i]->getIrStructure() == ir) {
			if (m_nodes[i]->getContext()->isEqual(context)) {
				return m_nodes[i];
			}
		}
	}

	return nullptr;
}

void piranha::NodeProgram::writeAssembly(const std::string &fname) const {
	KeyValueLookup<NodeOutput *, int> m_outputLookup;
	int currentIndex = 0;

	int nodeCount = getNodeCount();

	for (int i = 0; i < nodeCount; i++) {
		Node *node = getNode(i);

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
		Node *node = getNode(i);
		std::string builtinName = node->getBuiltinName();

		file << builtinName << " ";

		int nodeInputs = node->getInputCount();
		if (nodeInputs > 0) {
			file << "INPUTS { ";
			for (int i = 0; i < nodeInputs; i++) {
				const Node::NodeInputPort *port = node->getInput(i);
				int index = *m_outputLookup.lookup(*port->input);

				file << port->name << ": @" << index << "; ";
			}
			file << "} ";
		}

		int nodeOutputs = node->getOutputCount();
		int nodeOutputReferences = node->getOutputReferenceCount();

		if (nodeOutputReferences > 0 || nodeOutputs > 0) {
			file << "OUTPUTS { ";
			for (int i = 0; i < nodeOutputs; i++) {
				const Node::NodeOutputPort *port = node->getOutput(i);
				int index = *m_outputLookup.lookup(port->output);

				file << port->name << ": @" << index << "; ";
			}

			for (int i = 0; i < nodeOutputReferences; i++) {
				const Node::NodeOutputPortReference *port = node->getOutputReference(i);
				int index = *m_outputLookup.lookup(*port->output);

				file << port->name << ": @" << index << "; ";
			}
			file << "}";
		}

		file << "\n";
	}

	file.close();
}

void piranha::NodeProgram::execute() {
    int nodeCount = getNodeCount();

    // Initialize all nodes
    for (int i = 0; i < nodeCount; i++) {
        Node *node = getNode(i);
        node->initialize();
    }

    for (int i = 0; i < nodeCount; i++) {
        Node *node = getNode(i);
        node->evaluate();
    }
}
