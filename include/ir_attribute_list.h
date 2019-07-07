#ifndef IR_ATTRIBUTE_LIST_H
#define IR_ATTRIBUTE_LIST_H

#include "ir_parser_structure.h"

#include <vector>

namespace piranha {

	class IrAttribute;
	class IrAttributeDefinition;

	class IrAttributeList : public IrParserStructure {
	public:
		IrAttributeList();
		~IrAttributeList();

		void addAttribute(IrAttribute *attribute);
		IrAttribute *getAttribute(int index) const { return m_attributes[index]; }
		IrAttribute *getAttribute(IrAttributeDefinition *definition) const;
		int getAttributeCount() const { return (int)m_attributes.size(); }

	protected:
		std::vector<IrAttribute *> m_attributes;
	};

} /* namespace piranha */

#endif /* IR_ATTRIBUTE_LIST_H */
