// test_copilot.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>
#include <string>

class CNewProcess {
private:
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    bool processCreated;

public:
    CNewProcess() : processCreated(false) {
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
    }

    ~CNewProcess() {
        if (processCreated) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        }
    }

    // Create process with path, arguments, and window visibility
    bool Create(const std::wstring& programPath, const std::wstring& arguments = L"", bool hideWindow = false) {
        std::wstring cmdLine = L"\"" + programPath + L"\"";
        if (!arguments.empty()) {
            cmdLine += L" " + arguments;
        }
        // CreateProcess requires a non-const buffer
        std::wstring mutableCmdLine = cmdLine;
        if (hideWindow) {
            si.dwFlags |= STARTF_USESHOWWINDOW;
            si.wShowWindow = SW_HIDE;
        }
        processCreated = CreateProcess(
            NULL,                   // No module name (use command line)
            &mutableCmdLine[0],     // Command line
            NULL,                   // Process handle not inheritable
            NULL,                   // Thread handle not inheritable
            FALSE,                  // Set handle inheritance to FALSE
            0,                      // No creation flags
            NULL,                   // Use parent's environment block
            NULL,                   // Use parent's starting directory 
            &si,                    // Pointer to STARTUPINFO structure
            &pi                     // Pointer to PROCESS_INFORMATION structure
        );
        return processCreated;
    }

    HANDLE GetProcessHandle() const {
        return processCreated ? pi.hProcess : NULL;
    }
    HANDLE GetThreadHandle() const {
        return processCreated ? pi.hThread : NULL;
    }
    DWORD GetExitCode() const {
        if (!processCreated) {
            return -1;
        }
        DWORD exitCode;
        if (GetExitCodeProcess(pi.hProcess, &exitCode)) {
            return exitCode;
        }
        return -1;
    }
    bool Wait(DWORD timeout = INFINITE) const {
        if (!processCreated) {
            return false;
        }
        return WaitForSingleObject(pi.hProcess, timeout) == WAIT_OBJECT_0;
    }
};

int main()
{
    CNewProcess process;
    std::wstring programPath = L"notepad.exe";
    std::wstring arguments = L""; // Example: L"C:\\test.txt"
    bool hideWindow = true;
    if (process.Create(programPath, arguments, hideWindow)) {
        std::cout << "Process created successfully!\n";
        std::cout << "Process handle: " << process.GetProcessHandle() << "\n";
        std::cout << "Thread handle: " << process.GetThreadHandle() << "\n";
        process.Wait();
        std::cout << "Process finished with exit code: " << process.GetExitCode() << std::endl;
    }
    else {
        std::cout << "Failed to create process. Error code: " << GetLastError() << std::endl;
    }
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
