#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <cstdint>
#include <fstream>

using namespace std;
DWORD pid;
DWORD Temp = (int)0;
int NumValue;

int main() {
	HWND hWnd = FindWindowA(0, ("WannaCry V1.0"));
	GetWindowThreadProcessId(hWnd, &pid);
	HANDLE pHandle = OpenProcess(PROCESS_VM_READ, FALSE, pid);
	int n;

	/* Prompt the user to enter the value of n, obtained in Step1 */
	cout << "Please enter value of n: ";
	/* Assign n to a variable*/
	cin >> n;

	//"140737488355327" is the end of memory address.
	while (Temp != 140737488355327) {
		ReadProcessMemory(pHandle, (LPVOID)Temp, &NumValue, sizeof(NumValue), 0);

		/* NumValue is the value read from a memory address*/
		if (NumValue > 1 && NumValue < n) {
			/* Check and print out the values that can be p or q such that n = pq */
			if (n % NumValue == 0) {
				cout << "Found! p = "<< NumValue <<", q = "<< n/NumValue << endl;
				break;
			}
		}
		Temp += 1;
	}
	system("pause");
}
