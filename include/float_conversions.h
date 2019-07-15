#ifndef PIRANHA_FLOAT_CONVERSIONS_H
#define PIRANHA_FLOAT_CONVERSIONS_H

#include "pipe_node.h"
#include "fundamental_output.h"

namespace piranha {

	class StringToFloatConversionOutput : public FloatValueOutput {
	public:
		StringToFloatConversionOutput();
		~StringToFloatConversionOutput();

		virtual void fullCompute(void *target) const;
		virtual void registerInputs();

		pNodeInput *getInputConnection() { return &m_input; }

	protected:
		pNodeInput m_input;
	};
	typedef PipeNode<StringToFloatConversionOutput> StringToFloatConversionNode;

} /* namespace piranha */

#endif /* PIRANHA_FLOAT_CONVERSIONS_H */
