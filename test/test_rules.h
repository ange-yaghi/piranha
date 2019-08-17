#ifndef TEST_GENERATOR_H
#define TEST_GENERATOR_H

#include "../include/language_rules.h"

class TestRules : public piranha::LanguageRules {
public:
    TestRules();
    ~TestRules();

protected:
    virtual void registerBuiltinNodeTypes();
};

#endif /* TEST_GENERATOR_H */
