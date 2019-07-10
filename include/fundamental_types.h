#ifndef PIRANHA_FUNDAMENTAL_TYPES_H
#define PIRANHA_FUNDAMENTAL_TYPES_H

#include "node_type.h"

#include <string>

namespace piranha {

	struct FundamentalType {
		static const NodeType FloatType;
		static const NodeType IntType;
		static const NodeType StringType;
		static const NodeType BoolType;
	};

	// Native types
	typedef double			literal_float;
	typedef bool			literal_bool;
	typedef std::string		literal_string;
	typedef int				literal_int;

	template <typename BaseType>
	class LiteralType {};

	class LiteralFloatType :	public LiteralType<piranha::literal_float> {};
	class LiteralBoolType :		public LiteralType<piranha::literal_bool> {};
	class LiteralStringType :	public LiteralType<piranha::literal_string> {};
	class LiteralIntType :		public LiteralType<piranha::literal_int> {};

} /* namespace piranha */

#endif /* PIRANHA_FUNDAMENTAL_TYPES_H */
