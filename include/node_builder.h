#ifndef NODE_BUILDER_H
#define NODE_BUILDER_H

#include <string>

namespace piranha {

	class Node;

	class NodeBuilder {
	public:
		virtual Node *buildNode() const = 0;
		virtual bool checkName(const std::string &name) const = 0;
		virtual bool checkCompiler(const std::string &compiler) const = 0;
	};

	template <
		typename NodeType, 
		const std::string &BuiltinName,
		const std::string &Compiler = ""
	>
	class SpecializedNodeBuilder : public NodeBuilder {
		virtual Node *buildNode() const {
			return new NodeType();
		}

		virtual bool checkName(const std::string &name) const {
			return name == BuiltinName;
		}

		virtual bool checkCompiler(const std::string &compiler) const {
			return Compiler.empty() || (compiler == Compiler);
		}
	};

} /* namespace piranha */

#endif /* NODE_BUILDER_H */
