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
	typedef double			native_float;
	typedef bool			native_bool;
	typedef std::string		native_string;
	typedef int				native_int;

	template <typename BaseType>
	class LiteralType {};

	class LiteralFloatType :	public LiteralType<piranha::native_float> {};
	class LiteralBoolType :		public LiteralType<piranha::native_bool> {};
	class LiteralStringType :	public LiteralType<piranha::native_string> {};
	class LiteralIntType :		public LiteralType<piranha::native_int> {};

	template <typename NativeType>
	struct NativeTypeLookup { };// static_assert(false, "Looking up a type that does not exist");};

	template <> struct NativeTypeLookup<native_float> { static const NodeType *get() { return &FundamentalType::FloatType; } };
	template <> struct NativeTypeLookup<native_bool> { static const NodeType *get() { return &FundamentalType::BoolType; } };
	template <> struct NativeTypeLookup<native_int> { static const NodeType *get() { return &FundamentalType::IntType; } };
	template <> struct NativeTypeLookup<native_string> { static const NodeType *get() { return &FundamentalType::StringType; } };

} /* namespace piranha */

#endif /* PIRANHA_FUNDAMENTAL_TYPES_H */
