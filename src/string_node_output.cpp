#include <string_node_output.h>

#include <string>

const piranha::NodeType piranha::StringNodeOutput::StringType("StringNodeType");

piranha::StringNodeOutput::StringNodeOutput() : NodeOutput(&StringType) {
	/* void */
}

piranha::StringNodeOutput::~StringNodeOutput() {
	/* void */
}
