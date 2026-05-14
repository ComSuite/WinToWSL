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

int main()
{
    HMODULE hLib = LoadLibrary(L"wslapi.dll");
    auto pWslLaunch = reinterpret_cast<WslLaunch>(
        GetProcAddress(hLib, "WslLaunch"));

    if (pWslLaunch == nullptr) {
        std::cerr << "Failed to get WslLaunch function address.\n";
        return 1;
	}

	HANDLE hProcess = nullptr;
    if (pWslLaunch(L"Ubuntu", L"ls", TRUE, GetStdHandle(STD_INPUT_HANDLE), GetStdHandle(STD_OUTPUT_HANDLE), GetStdHandle(STD_ERROR_HANDLE), &hProcess) != S_OK) {
		std::cerr << "Failed to launch WSL process.\n";
        return 1;
    }

    if (hProcess == nullptr) {
        std::cerr << "Failed to launch WSL process.\n";
        return 1;
	}

	CloseHandle(hProcess);

    std::cout << "Hello World!\n";
}

