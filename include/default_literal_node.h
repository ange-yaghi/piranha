#ifndef PIRANHA_DEFAULT_LITERAL_NODE_H
#define PIRANHA_DEFAULT_LITERAL_NODE_H

#include <literal_node.h>

namespace piranha {

	template <typename LiteralType>
	class DefaultLiteralNode : public LiteralNode<LiteralType> {
	public:
		DefaultLiteralNode() {
			/* void */
		}

		~DefaultLiteralNode() {
			/* void */
		}

		virtual void setData(LiteralType data) { m_literalData = data; }

	protected:
		virtual void _initialize() {
			/* void */
		}

		virtual void _evaluate() {
			m_output.setData(m_literalData);
		}

		virtual void registerOutputs() {
			setPrimaryOutput(&m_output);
			registerOutput(&m_output, "$primary");
		}

	protected:
		LiteralNodeOutput<LiteralType> m_output;
		LiteralType m_literalData;
	};

	typedef DefaultLiteralNode<std::string> DefaultLiteralStringNode;
	typedef DefaultLiteralNode<int> DefaultLiteralIntNode;
	typedef DefaultLiteralNode<double> DefaultLiteralFloatNode;
	typedef DefaultLiteralNode<bool> DefaultLiteralBoolNode;

} /* namespace piranha */

#endif /* PIRANHA_DEFAULT_LITERAL_NODE_H */
