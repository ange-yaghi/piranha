#ifndef PIRANHA_NODE_BUILDER_H
#define PIRANHA_NODE_BUILDER_H

#include <string>

namespace piranha {

	class Node;

	template <typename KeyType, typename NodeBase>
	class NodeBuilder {
	public:
		virtual NodeBase *buildNode() const = 0;
		virtual bool checkKey(const KeyType &key) const = 0;
		virtual bool checkCompiler(const std::string &compiler) const = 0;
	};

	template <typename NodeType, typename NodeBase, typename KeyType>
	class SpecializedNodeBuilder : public NodeBuilder<KeyType, NodeBase> {
	public:
		SpecializedNodeBuilder(const KeyType &key, const std::string &compiler) {
			m_key = key;
			m_compiler = compiler;
		}

		virtual NodeBase *buildNode() const {
			return new NodeType();
		}

		virtual bool checkKey(const KeyType &key) const {
			return key == m_key;
		}

		virtual bool checkCompiler(const std::string &compiler) const {
			return m_compiler.empty() || (compiler == m_compiler);
		}

	protected:
		KeyType m_key;
		std::string m_compiler;
	};

} /* namespace piranha */

#endif /* PIRANHA_NODE_BUILDER_H */
