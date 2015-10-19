#include <stdio.h>
#include <string>
#include <windows.h>
#include <assert.h>

#include "../../Project1/Project1/test_dll.h"	// TODO: move this header to the shared folder?

#ifdef _MSC_VER
#pragma comment(lib, "user32.lib")	// For SendMessage() function
#endif

typedef CTestObject* (__cdecl *f_create_test_obj)();
typedef void (__cdecl *f_delete_test_obj)(CTestObject*);

FARPROC LoadFunctionFromDll(const wchar_t* dll_name, const char* f_name, HINSTANCE& dll);
void SendMessageToParent(HWND hwnd, int x);

int main(int argn, char** argv) {
	if (argn < 4) {
		printf("please, use three arguments: dll loads count, objects count and target hwnd\n");
		return 0;
	}

	int dll_loads_c = std::stoi(argv[1]);
	int objects_c = std::stoi(argv[2]);
	HWND hwnd = (HWND)std::stoi(argv[3]);

	int objects_created = 0;

	for (int i = 0; i < dll_loads_c; i++) {
		HINSTANCE dll = 0;
		f_create_test_obj create_obj_f = (f_create_test_obj)LoadFunctionFromDll(L"Project1.dll", "CreateTestObject", dll);
		if (!create_obj_f)
			return -1;

		f_delete_test_obj delete_test_obj = (f_delete_test_obj)LoadFunctionFromDll(L"", "DeleteTestObject", dll);
		if (!delete_test_obj)
			return -1;

		SendMessageToParent(hwnd, i + 1);

		for (int j = 0; j < objects_c; j++) {
			CTestObject* test_obj = create_obj_f();
			objects_created++;
			delete_test_obj(test_obj);
		}
		FreeLibrary(dll);
	}

	return objects_created;
}

FARPROC LoadFunctionFromDll(const wchar_t* dll_name, const char* f_name, HINSTANCE& dll) {
	if (!dll)
		dll = LoadLibrary(dll_name);

	if (!dll) {
		printf("could not load the dynamic library\n");
		return nullptr;
	}

	FARPROC func_adress = GetProcAddress(dll, f_name);
	if (!func_adress) {
		printf("could not locate the function\n");
		FreeLibrary(dll);
		return nullptr;
	}

	return func_adress;
}

void SendMessageToParent(HWND hwnd, int x) {
	if (hwnd == 0) {
		assert(hwnd);
		return;
	}
	COPYDATASTRUCT cd;
	cd.dwData = 1; // can be anything
	cd.cbData = sizeof(int);
	cd.lpData = &x;                //msg additional data
	SendMessage(hwnd, WM_COPYDATA, 0, (LPARAM)(LPVOID)(&cd));
}