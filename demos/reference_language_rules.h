#ifndef PIRANHA_REFERENCE_COMPILER_REFERENCE_LANGUAGE_RULES_H
#define PIRANHA_REFERENCE_COMPILER_REFERENCE_LANGUAGE_RULES_H

#include "../include/language_rules.h"

namespace piranha_demo {

    class ReferenceLanguageRules : public piranha::LanguageRules {
    public:
        ReferenceLanguageRules();
        ~ReferenceLanguageRules();

    protected:
        virtual void registerBuiltinNodeTypes();
    };

} /* namespace piranha_demo */

#endif /* PIRANHA_REFERENCE_COMPILER_REFERENCE_LANGUAGE_RULES_H */
