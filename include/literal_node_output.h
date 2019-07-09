#ifndef PIRANHA_LITERAL_NODE_OUTPUT_H
#define PIRANHA_LITERAL_NODE_OUTPUT_H

#include "node_output.h"

#include <string>

namespace piranha {

	class LiteralNodeOutputBase : public NodeOutput {
	public:
		static const NodeType FloatType;
		static const NodeType IntType;
		static const NodeType StringType;
		static const NodeType BoolType;

	public:
		LiteralNodeOutputBase(const NodeType *type) : NodeOutput(type) {
			/* void */
		}

		virtual ~LiteralNodeOutputBase() {
			/* void */
		}
	};

	template <typename LiteralType>
	class LiteralNodeOutputSpecialized : public LiteralNodeOutputBase {
	public:
		LiteralNodeOutputSpecialized(const NodeType *type) : LiteralNodeOutputBase(type) {
			/* void */
		}

		virtual ~LiteralNodeOutputSpecialized() {
			/* void */
		}

		void setData(const LiteralType &data) {
			m_data = data;
		}

		LiteralType getData() const {
			return m_data;
		}

		virtual void fullCompute(void *_target) const {
			LiteralType *target = reinterpret_cast<LiteralType *>(_target);
			*target = m_data;
		}

	protected:
		LiteralType m_data;
	};

	template <typename LiteralType>
	class LiteralNodeOutput : public LiteralNodeOutputSpecialized<LiteralType> {};

	template <>
	class LiteralNodeOutput<int> : public LiteralNodeOutputSpecialized<int>{
	public:
		LiteralNodeOutput() : LiteralNodeOutputSpecialized<int>(&IntType) {}
		~LiteralNodeOutput() {}
	};

	template <>
	class LiteralNodeOutput<double> : public LiteralNodeOutputSpecialized<double> {
	public:
		LiteralNodeOutput() : LiteralNodeOutputSpecialized<double>(&FloatType) {}
		~LiteralNodeOutput() {}
	};

	template <>
	class LiteralNodeOutput<bool> : public LiteralNodeOutputSpecialized<bool> {
	public:
		LiteralNodeOutput() : LiteralNodeOutputSpecialized<bool>(&BoolType) {}
		~LiteralNodeOutput() {}
	};

	template <>
	class LiteralNodeOutput<std::string> : public LiteralNodeOutputSpecialized<std::string> {
	public:
		LiteralNodeOutput() : LiteralNodeOutputSpecialized<std::string>(&StringType) {}
		~LiteralNodeOutput() {}
	};

} /* namespace piranha */

#endif /* PIRANHA_LITERAL_NODE_OUTPUT_H */
