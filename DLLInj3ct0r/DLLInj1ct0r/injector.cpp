#include <Windows.h>
#include <stdio.h>

HANDLE hProcess, hThread = NULL;
DWORD PID, TID = NULL;
LPVOID rBuffer = NULL;
HMODULE hKernel32 = NULL;

//wchar_t dllPath[MAX_PATH] = L"C:\\Users\\boxhead\\source\\repos\\DLLInjection\\x64\\Debug\\DLLInjection.dll";
wchar_t dllPath[MAX_PATH];

void banner() {
	printf("-------------- DLL InJ3ct0r v 1.0 --------------\n               Created by b0xH34d\n\n");
}

int main() {

	banner();

	printf("PID of the target process: ");
	scanf_s("%ld", &PID);

	printf("Trying to get a handle for the process: %ld...\n", PID);

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	if (hProcess == NULL) {
		printf("Failed to get a handle for the given PID - Error: %ld\n", GetLastError());
		exit(1);
	}

	printf("Successfully got a handle for process %ld --> 0x%p\n", PID, hProcess);


	printf("Enter path to the DLL to be injected: ");
	wscanf_s(L"%ls", dllPath, (unsigned)_countof(dllPath));

	printf("Selected DLL: %S\n", dllPath);

	rBuffer = VirtualAllocEx(hProcess, NULL, sizeof(dllPath), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	printf("Allocated buffer to process memory\n");

	WriteProcessMemory(hProcess, rBuffer, dllPath, sizeof(dllPath), NULL);

	printf("Wrote [%S] to the memory\n", dllPath);

	hKernel32 = GetModuleHandleW(L"Kernel32");
	if (hKernel32 == NULL) {
		printf("Faild to get a handle to kernel32 module\n");
		exit(1);
	}
	printf("Got a handle to the Kernel32 module --> %x\n", hKernel32);
	LPTHREAD_START_ROUTINE start = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "LoadLibraryW");
	printf("Got address of LoadLibraryW() -> %x\n", start);

	hThread = CreateRemoteThread(hProcess, NULL, 0, start, rBuffer, 0, &TID);

	if (hThread == NULL) {
		printf("Failed to get a handle to thread -- Error: %ld\n", GetLastError());
	}

	printf("Got a handle of thread %ld --> %x\n", TID, hThread);
	printf("Waiting for the thread to finish execution...\n");

	WaitForSingleObject(hThread, INFINITE);

	printf("Execution completed\nCleaning up...");
	
	CloseHandle(hThread);
	CloseHandle(hProcess);

	printf("\n\nDLL has been injected!!!\n");

	getchar();

	return EXIT_SUCCESS;
}
