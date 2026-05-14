// WinToWSL.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>

typedef HRESULT(WINAPI* WslLaunch)(
    PCWSTR  distributionName,
    PCWSTR  command,
    BOOL    useCurrentWorkingDirectory,
    HANDLE  stdIn,
    HANDLE  stdOut,
    HANDLE  stdErr,
    PHANDLE process
);

int main(int argc, char* argv[])
{
    HMODULE hLib = LoadLibrary(L"wslapi.dll");
    auto pWslLaunch = reinterpret_cast<WslLaunch>(GetProcAddress(hLib, "WslLaunch"));

    if (pWslLaunch == nullptr) {
        std::cerr << "Failed to get WslLaunch function address.\n";
        return 1;
	}

    for (int i = 1; i < argc; ++i) {
        size_t newsize = strlen(argv[i]) + 1;
        wchar_t* wcstring = new wchar_t[newsize];
        if (wcstring == nullptr) {
            std::cerr << "Memory allocation failed.\n";
            continue;
		}

        size_t convertedChars = 0;
        if (mbstowcs_s(&convertedChars, wcstring, newsize, argv[i], _TRUNCATE) != 0) {
            std::cerr << "String conversion failed for argument: " << argv[i] << "\n";
            delete[] wcstring;
			continue;
        }

	    HANDLE hProcess = nullptr;
        if (pWslLaunch(L"Ubuntu", wcstring, TRUE, GetStdHandle(STD_INPUT_HANDLE), GetStdHandle(STD_OUTPUT_HANDLE), GetStdHandle(STD_ERROR_HANDLE), &hProcess) != S_OK) {
		    std::cerr << "Failed to launch WSL process.\n";
            return 1;
        }

        if (hProcess == nullptr) {
            std::cerr << "Failed to launch WSL process.\n";
            return 1;
	    }

		delete[] wcstring;
	    CloseHandle(hProcess);
	}

    std::cout << "Hello World!\n";
}

