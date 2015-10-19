#define EXTERN_DLL_EXPORT extern "C" __declspec(dllexport)

class CTestObject {
public:
	int test_member;
};

EXTERN_DLL_EXPORT CTestObject* CreateTestObject();
EXTERN_DLL_EXPORT void DeleteTestObject(CTestObject*);