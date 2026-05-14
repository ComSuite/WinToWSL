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

typedef BOOL(WINAPI* WslIsDistributionRegistered)(
    PCWSTR distributionName
);

wchar_t* to_wide_string(const char* str) {
    size_t newsize = strlen(str) + 1;
    wchar_t* wcstring = new wchar_t[newsize];
    if (wcstring == nullptr) {
        std::cerr << "Memory allocation failed.\n";
        return nullptr;
    }

    size_t convertedChars = 0;
    if (mbstowcs_s(&convertedChars, wcstring, newsize, str, _TRUNCATE) != 0) {
        std::cerr << "String conversion failed for argument: " << str << "\n";
        delete[] wcstring;
        return nullptr;
    }

    return wcstring;
}

int main(int argc, char* argv[])
{
    HMODULE hLib = LoadLibrary(L"wslapi.dll");
    auto pWslLaunch = reinterpret_cast<WslLaunch>(GetProcAddress(hLib, "WslLaunch"));
    if (pWslLaunch == nullptr) {
        std::cerr << "Failed to get WslLaunch function address.\n";
        return 1;
	}

	auto pWslIsDistributionRegistered = reinterpret_cast<WslIsDistributionRegistered>(GetProcAddress(hLib, "WslIsDistributionRegistered"));
    if (pWslIsDistributionRegistered == nullptr) {
        std::cerr << "Failed to get WslIsDistributionRegistered function address.\n";
		return 1;
	}

	if (argc < 3) {
        std::cerr << "No command provided to execute in WSL.\n";
        return 1;
    }

	wchar_t* distro = to_wide_string(argv[1]);
    if (distro == nullptr) {
		std::cerr << "Failed to convert distribution name to wide string.\n";
        return 1;
	}

    if (!pWslIsDistributionRegistered(distro)) {
        std::cerr << "The specified WSL distribution is not registered: " << argv[1] << "\n";
		delete[] distro;
        return 1;
	}

    for (int i = 2; i < argc; i++) {
        wchar_t* cmd = to_wide_string(argv[i]);
        if (cmd == nullptr) {
            continue;
		}

	    HANDLE hProcess = nullptr;
        if (pWslLaunch(distro, cmd, TRUE, GetStdHandle(STD_INPUT_HANDLE), GetStdHandle(STD_OUTPUT_HANDLE), GetStdHandle(STD_ERROR_HANDLE), &hProcess) != S_OK) {
		    std::cerr << "Failed to launch WSL process.\n";
            return 1;
        }

        if (hProcess == nullptr) {
            std::cerr << "Failed to launch WSL process.\n";
            return 1;
	    }

		delete[] cmd;
	    CloseHandle(hProcess);
	}

    delete[] distro;
	return 0;
}

