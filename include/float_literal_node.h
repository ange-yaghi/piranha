#ifndef PIRANHA_FLOAT_LITERAL_NODE_H
#define PIRANHA_FLOAT_LITERAL_NODE_H

#include "node.h"

#include "single_float_node_output.h"

namespace piranha {

	class FloatLiteralNode : public Node {
	public:
		FloatLiteralNode();
		virtual ~FloatLiteralNode();

		void setValue(double value) { m_value = value; }
		double getValue() const { return m_value; }

	protected:
		virtual void _initialize();
		virtual void _evaluate();
		virtual void _destroy();

		virtual void registerOutputs();

	protected:
		SingleFloatNodeOutput m_output;
		double m_value;
	};

} /* namespace piranha */

#endif /* PIRANHA_FLOAT_LITERAL_NODE_H */
