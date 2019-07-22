#ifndef PIRANHA_RULE_H
#define PIRANHA_RULE_H

#include <string>

namespace piranha {

    class Node;

    template <typename ValueType, typename NodeBase>
    class Rule {
    public:
        Rule(const ValueType &value) {
            m_value = value;
        }

        Rule() {
            /* void */
        }

        virtual ~Rule() {
            /* void */
        }

        virtual NodeBase *buildNode() const = 0;

        void setValue(const ValueType &value) { m_value = value; }
        const ValueType &getValue() const { return m_value; }

    protected:
        ValueType m_value;
    };

    template <typename ValueType, typename NodeType, typename NodeBase>
    class SpecializedRule : public Rule<ValueType, NodeBase> {
    public:
        SpecializedRule(const ValueType &value) : Rule(value) {
            /* void */
        }

        SpecializedRule() {
            /* void */
        }

        virtual ~SpecializedRule() {
            /* void */
        }

        virtual NodeBase *buildNode() const {
            return new NodeType();
        }
    };

} /* namespace piranha */

#endif /* PIRANHA_RULE_H */
