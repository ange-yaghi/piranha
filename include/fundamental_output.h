#ifndef PIRANHA_FUNDAMENTAL_OUTPUT_H
#define PIRANHA_FUNDAMENTAL_OUTPUT_H

#include "node_output.h"

#include "fundamental_types.h"

#include <string>

namespace piranha {

	template <typename FundamentalType>
	class FundamentalOutput : public NodeOutput {};

	template <>
	class FundamentalOutput<double> : public NodeOutput {
	public:
		FundamentalOutput() : NodeOutput(&FundamentalType::FloatType) {};
		~FundamentalOutput() {}
	};

	template <>
	class FundamentalOutput<int> : public NodeOutput {
	public:
		FundamentalOutput() : NodeOutput(&FundamentalType::IntType) {};
		~FundamentalOutput() {}
	};

	template <>
	class FundamentalOutput<bool> : public NodeOutput {
	public:
		FundamentalOutput() : NodeOutput(&FundamentalType::BoolType) {};
		~FundamentalOutput() {}
	};

	template <>
	class FundamentalOutput<std::string> : public NodeOutput {
	public:
		FundamentalOutput() : NodeOutput(&FundamentalType::StringType) {};
		~FundamentalOutput() {}
	};

	typedef FundamentalOutput<double> FloatValueOutput;
	typedef FundamentalOutput<int> IntValueOutput;
	typedef FundamentalOutput<bool> BoolValueOutput;
	typedef FundamentalOutput<std::string> StringValueOutput;

} /* namespace piranha */

#endif /* PIRANHA_FUNDAMENTAL_OUTPUT_H */
