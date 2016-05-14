#include <iostream>
#include <cassert>
#include <sstream>
#include "test_util.hpp"

#include "kaguya/kaguya.hpp"

int main()
{
    // tzxnote: other cpp files will register test functions
	bool test_result = kaguya_test_util::TestRunner::instance().execute();
	return test_result ? 0 : -1;
}
