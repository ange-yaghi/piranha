#ifndef SINGLE_FLOAT_NODE_OUTPUT_H
#define SINGLE_FLOAT_NODE_OUTPUT_H

#include <float_node_output.h>

namespace piranha {

	class SingleFloatNodeOutput : public FloatNodeOutput {
	public:
		SingleFloatNodeOutput();
		virtual ~SingleFloatNodeOutput();

		virtual void fullOutput(void *target) const;

		double getValue() const { return m_value; }
		void setValue(double v) { m_value = v; }

	protected:
		double m_value;
	};

} /* namespace piranha */

#endif /* SINGLE_FLOAT_NODE_OUTPUT_H */
