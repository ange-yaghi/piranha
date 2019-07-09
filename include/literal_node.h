#ifndef PIRANHA_LITERAL_NODE_H
#define PIRANHA_LITERAL_NODE_H

#include "node.h"

#include "literal_node_output.h"

namespace piranha {

	class LiteralNode : public Node {
	public:
		LiteralNode() {}
		~LiteralNode() {}

		virtual void setData(double) {}
		virtual void setData(int) {}
		virtual void setData(const std::string &) {}
		virtual void setData(bool) {}
	};

	template <typename LiteralType>
	class SpecializedLiteralNode : public LiteralNode {
	public:
		SpecializedLiteralNode() {}
		~SpecializedLiteralNode() {}

		virtual void setData(LiteralType data) { m_literalData = data; }

	protected:
		virtual void _initialize() {}
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

} /* namespace piranha */

#endif /* PIRANHA_LITERAL_NODE_H */
