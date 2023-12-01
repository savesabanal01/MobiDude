
#include <vector>
#include <fstream>
#include <string>
#include "include/shell.h"

void launchProgrammer(const TCHAR* fullPath, const char* use_programmer, const char* use_mcu, const char* use_prog, const char* use_speed, const char* use_port, const char* filepath, bool* running, DWORD* exitcode){
	
	char loaderCommand[dudecmdlen] = {0};
	if (!strcmp(use_programmer, "AVRDude")) {
		sprintf_s(loaderCommand, "%sAVRDude\\avrdude.exe -v -p%s -c%s -P %s -b%s -D -U flash:w:\"%s\":a", fullPath, use_mcu, use_prog, use_port, use_speed, filepath);
	}
	else if (!strcmp(use_programmer, "ESP32tool")) {
		sprintf_s(loaderCommand, "%sESP-tool\\Scripts\\python.exe %sESP-tool\\tool-esptoolpy\\esptool.py write_flash 0x0000 %s", fullPath, fullPath, filepath);
	}

	//MessageBox(NULL, loaderCommand, "About...", 0);		// undefine this to get message window with commands for AVRdude

	STARTUPINFO stinf = {0};
	stinf.cb = sizeof(stinf);
	PROCESS_INFORMATION prcinf = {0};
	
	//if(CreateProcess(NULL, loaderCommand, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &stinf, &prcinf)){
	if (CreateProcess(NULL, loaderCommand, NULL, NULL, FALSE, CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &stinf, &prcinf)) {

      if(WaitForSingleObject(prcinf.hProcess, dude_run_timeout) == WAIT_TIMEOUT){
      	TerminateProcess(prcinf.hProcess, EC_DUDE_TIMEOUT);
	  }

      GetExitCodeProcess(prcinf.hProcess, exitcode);

      CloseHandle(prcinf.hThread);
      CloseHandle(prcinf.hProcess);
	}
	else{
		*exitcode = EC_DUDE_NOEXEC;
	}
								
	*running = false;
	return;
}

void killProcessByName(const char *filename){
	
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
    PROCESSENTRY32 pEntry;
	
    pEntry.dwSize = sizeof (pEntry);
    BOOL hRes = Process32First(hSnapShot, &pEntry);
	
    while(hRes){
        if(strcmp(pEntry.szExeFile, filename) == 0){
			
            HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0, (DWORD) pEntry.th32ProcessID);
            if (hProcess != NULL){
				
                TerminateProcess(hProcess, 9);
                CloseHandle(hProcess);
            }
        }
        hRes = Process32Next(hSnapShot, &pEntry);
    }
	
    CloseHandle(hSnapShot);
}
