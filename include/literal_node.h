#ifndef PIRANHA_LITERAL_NODE_H
#define PIRANHA_LITERAL_NODE_H

#include "node.h"

#include "literal_node_output.h"

namespace piranha {

	template <typename LiteralType>
	class LiteralNode : public Node {
	public:
		LiteralNode() {}
		~LiteralNode() {}

		virtual void setData(LiteralType data) = 0;
	};

	typedef LiteralNode<piranha::literal_string> StringLiteralNode;
	typedef LiteralNode<piranha::literal_int> IntLiteralNode;
	typedef LiteralNode<piranha::literal_float> FloatLiteralNode;
	typedef LiteralNode<piranha::literal_bool> BoolLiteralNode;

} /* namespace piranha */

#endif /* PIRANHA_LITERAL_NODE_H */
