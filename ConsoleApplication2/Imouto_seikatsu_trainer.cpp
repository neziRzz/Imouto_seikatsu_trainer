#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>
#include <TlHelp32.h>
using namespace std;

bool MaxTrust = FALSE;
bool MaxInterest = FALSE;
bool MaxLust = FALSE;
bool MaxMoney = FALSE;

DWORD FindProcessId(const wchar_t* processname)
{
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;
    DWORD result = 0;

    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hProcessSnap) return(FALSE);

    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32))
    {
        CloseHandle(hProcessSnap);   
        printf("!!! Failed to gather information on system processes! \n");
        return(0);
    }

    do
    {
        if (0 == _wcsicmp(processname, pe32.szExeFile))
        {
            result = pe32.th32ProcessID;
            break;
        }
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);

    return result;
}
uintptr_t getBaseAddr(DWORD pID, const wchar_t* procname) {
    uintptr_t modBaseAddr = 0; 
    MODULEENTRY32 mod32;
    mod32.dwSize = sizeof(mod32); 
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE32 | TH32CS_SNAPMODULE, pID);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        cout << "Error!INVALID_HANDLE_VALUE" << endl;
        getchar();
    }
    else {
        if (!Module32First(hSnapshot, &mod32)) {
            cout << "Can't find Module!" << endl; 
            getchar();
            exit(-1);
        }
        else {
            do {
                if (0 == _wcsicmp(mod32.szModule, procname)) {
                    modBaseAddr = (uintptr_t)mod32.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnapshot, &mod32));
        }
    }
    return modBaseAddr;
}

uintptr_t FindDMAAddy(HANDLE procHandle, uintptr_t ptr, vector<unsigned int> offsets) {
    uintptr_t addr = ptr; 
    for (unsigned int i = 0; i < offsets.size(); i++) {
        ReadProcessMemory(procHandle, (BYTE*)addr, &addr, sizeof(addr), nullptr);
        addr += offsets[i];
    }
    return addr;
}

int main(int argc, char** argv) {
    DWORD pID = FindProcessId(L"GamePro.exe");
    if (pID == NULL) {
        cout << "Can't find process" << endl;
        getchar();
        exit(-1);
    }
    else {
        cout << "Hooked" << endl;
        HANDLE procHandle = OpenProcess(PROCESS_ALL_ACCESS, NULL, pID);
        if (procHandle == NULL) {
            cout << "Failed" << endl; 
            getchar();
        }
        else {
            cout << "I alone am the honored one" << endl;
            cout << "Usage:F1(Max Trust),F2(Max Interest),F3(Max Lust),F4(Max Money)" << endl;
            uintptr_t BaseAddr = getBaseAddr(pID, L"GamePro.exe");
            uintptr_t dynamicPtrBaseAddImouto = BaseAddr + 0xDAD67C;
            vector<unsigned int> TrustOffsets = {0x0, 0x54};
            vector<unsigned int> InterestOffsets = { 0x0,0x58};
            vector<unsigned int> ImoutoLustOffsets = { 0x0,0x198 };
            vector<unsigned int> MoneyOffsets = { 0x0,0x1C };
            uintptr_t TrustAddr = FindDMAAddy(procHandle, dynamicPtrBaseAddImouto, TrustOffsets);
            uintptr_t InterestAddr = FindDMAAddy(procHandle, dynamicPtrBaseAddImouto, InterestOffsets);
            uintptr_t ImoutoLustAddr = FindDMAAddy(procHandle, dynamicPtrBaseAddImouto, ImoutoLustOffsets);
            uintptr_t MoneyAddr = FindDMAAddy(procHandle, dynamicPtrBaseAddImouto, MoneyOffsets);
            while (MaxTrust==FALSE) {
                if (GetAsyncKeyState(VK_F1)) {
                    int InitTrust = 0;
                    ReadProcessMemory(procHandle, (BYTE*)TrustAddr, &InitTrust, sizeof(InitTrust), nullptr);
                    int NewTrust = 9999;
                    WriteProcessMemory(procHandle, (BYTE*)TrustAddr,&NewTrust,sizeof(NewTrust),nullptr);
                    MaxTrust = TRUE;
                    cout << "Patch applied!" << endl;
                    break;
                }
            }
            while (MaxInterest == FALSE) {
                if (GetAsyncKeyState(VK_F2)) {
                    int InitInterest = 0;
                    ReadProcessMemory(procHandle, (BYTE*)InterestAddr, &InitInterest, sizeof(InitInterest), nullptr);
                    int NewInterest = 9999;
                    WriteProcessMemory(procHandle, (BYTE*)InterestAddr, &NewInterest, sizeof(NewInterest), nullptr);
                    MaxTrust = TRUE;
                    cout << "Patch applied!" << endl;
                    break;
                }
            }
            while (MaxLust == FALSE) {
                if (GetAsyncKeyState(VK_F3)) {
                    int InitLust = 0;
                    ReadProcessMemory(procHandle, (BYTE*)ImoutoLustAddr, &InitLust, sizeof(InitLust), nullptr);
                    int NewLust = 120;
                    WriteProcessMemory(procHandle, (BYTE*)ImoutoLustAddr, &NewLust, sizeof(NewLust), nullptr);
                    MaxLust = TRUE;
                    cout << "Patch applied!" << endl;
                    break;
                }
            }
            while (MaxMoney == FALSE) {
                if (GetAsyncKeyState(VK_F4)) {
                    int InitMoney = 0;
                    ReadProcessMemory(procHandle, (BYTE*)MoneyAddr, &InitMoney, sizeof(InitMoney), nullptr);
                    int NewMoney = 999999;
                    WriteProcessMemory(procHandle, (BYTE*)MoneyAddr, &NewMoney, sizeof(NewMoney), nullptr);
                    MaxMoney = TRUE;
                    cout << "Patch applied!" << endl;
                    break;
                }
            }
        }
    }
    getchar();
	return 0;
}