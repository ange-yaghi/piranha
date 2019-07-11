#ifndef PIRANHA_RULE_H
#define PIRANHA_RULE_H

#include <string>

namespace piranha {

	class Node;

	template <typename KeyType, typename ValueType, typename NodeBase>
	class Rule {
	public:
		Rule(const KeyType &key, const ValueType &value) {
			m_key = key;
			m_value = value;
		}
		virtual ~Rule() {
			/* void */
		}

		virtual NodeBase *buildNode() const = 0;

		virtual bool checkKey(const KeyType &key) const {
			return key == m_key;
		}

		virtual bool checkCompiler(const std::string &compiler) const {
			return m_compiler.empty() || (compiler == m_compiler);
		}

	protected:
		KeyType m_key;
		ValueType m_value;
	};

	template <typename KeyType, typename ValueType, typename NodeType, typename NodeBase>
	class SpecializedRule : public NodeBuilder<KeyType, NodeBase> {
	public:
		SpecializedRule(const KeyType &key, const ValueType &value)
			: Rule(key, value) {
			m_key = key;
			m_compiler = compiler;
		}

		virtual NodeBase *buildNode() const {
			return new NodeType();
		}
	};

} /* namespace piranha */

#endif /* PIRANHA_RULE_H */
