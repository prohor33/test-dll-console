#include "test_dll.h"

CTestObject* CreateTestObject() {
	CTestObject* test_obj = new CTestObject();
	test_obj->test_member = 3;
	return test_obj;
}

void DeleteTestObject(CTestObject* obj) {
	if (obj)
		delete obj;
}