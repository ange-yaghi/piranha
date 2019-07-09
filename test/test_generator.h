#ifndef TEST_GENERATOR_H
#define TEST_GENERATOR_H

#include <generator.h>

class TestGenerator : public piranha::Generator {
public:
	TestGenerator();
	~TestGenerator();

	virtual void registerBuiltinNodeTypes();
};

#endif /* TEST_GENERATOR_H */
