#include <node.h>

#include <node_output.h>
#include <node_program.h>

#include <assert.h>

piranha::Node::Node() {
	m_id = -1;
	m_name = "";

	m_initialized = false;
	m_evaluated = false;
	m_primaryOutput = nullptr;
	m_primaryReference = nullptr;
}

piranha::Node::~Node() {
	/* void */
}

void piranha::Node::initialize() {
	if (isInitialized()) return;

	// Set initialized flag
	m_initialized = true;

	// Initialize
	_initialize();

	registerInputs();
	registerOutputs();
}

void piranha::Node::evaluate() {
	if (isEvaluated()) return;

	// Set evaluated flag
	m_evaluated = true;

	// First evaluate all dependencies
	int inputCount = getInputCount();
	for (int i = 0; i < inputCount; i++) {
		pNodeInput *node = m_inputs[i].input;
		if (node != nullptr && *node != nullptr) {
			(*node)->evaluate();
		}
	}

	/*
	if (requiresMaterials()) {
		getProgram()
			->getMaterialManager()
			->evaluateAllMaterialNodes();
	}*/

	// Node can now self-evaluate
	_evaluate();

	int outputReferenceCount = getOutputReferenceCount();
	for (int i = 0; i < outputReferenceCount; i++) {
		NodeOutput *node = *m_outputReferences[i].output;
		node->evaluate();
	}
}

void piranha::Node::destroy() {
	m_initialized = false;
	m_evaluated = false;

	_destroy();
}

const piranha::ChannelType *piranha::Node::getConversion(pNodeInput input, const std::string &name) {
	int inputCount = getInputCount();

	for (int i = 0; i < inputCount; i++) {
		if (name == m_inputs[i].name) {
			const ChannelType *requiredType = m_inputs[i].requiredType;

			// Check if the input doesn't care about input type
			if (requiredType == nullptr) continue;

			if (!input->getType()->isCompatibleWith(*requiredType)) {
				return requiredType;
			}

			// Warning: do not break here! There could potentially be multiple
			// inputs with the same name referencing different endpoints
		}
	}

	return nullptr;
}

void piranha::Node::connectInput(pNodeInput input, const std::string &name) {
	int inputCount = getInputCount();

	for (int i = 0; i < inputCount; i++) {
		if (name == m_inputs[i].name) {
			*m_inputs[i].input = input;
			
			// Warning: do not break here! There could potentially be multiple
			// inputs with the same name referencing different endpoints
		}
	}
}

void piranha::Node::connectDefaultInput(pNodeInput input) {
	assert(getInputCount() == 1); // Use of this function is reserved with single input nodes

	*m_inputs[0].input = input;
}

piranha::NodeOutput *piranha::Node::getPrimaryOutput() const {
	return (m_primaryOutput != nullptr) ? m_primaryOutput
		: (m_primaryReference != nullptr) ? *m_primaryReference
			: nullptr;
}

piranha::NodeOutput *piranha::Node::getOutput(const std::string &name) const {
	int outputCount = getOutputCount();

	for (int i = 0; i < outputCount; i++) {
		if (name == m_outputs[i].name) {
			return m_outputs[i].output;
		}
	}

	int outputReferenceCount = getOutputReferenceCount();
	for (int i = 0; i < outputReferenceCount; i++) {
		if (name == m_outputReferences[i].name) {
			return *m_outputReferences[i].output;
		}
	}

	return nullptr;
}

void piranha::Node::_initialize() {
	/* void */
}

void piranha::Node::_evaluate() {
	/* void */
}

void piranha::Node::_destroy() {
	/* void */
}

void piranha::Node::registerInputs() {
	/* void */
}

void piranha::Node::registerInput(pNodeInput *node, const std::string &name, const ChannelType *requiredNodeType) {
	m_inputs.push_back({ node, name, requiredNodeType });
}

void piranha::Node::registerOutput(NodeOutput *node, const std::string &name) {
	m_outputs.push_back({ node, name });
	node->setParentNode(this);
}

void piranha::Node::setPrimaryOutput(NodeOutput *node) {
	m_primaryOutput = node;
}

void piranha::Node::setPrimaryOutputReference(NodeOutput **node) {
	m_primaryReference = node;
}

void piranha::Node::registerOutputReference(NodeOutput *const *node, const std::string &name) {
	m_outputReferences.push_back({ node, name });
}

void piranha::Node::registerOutputs() {
	/* void */
}
