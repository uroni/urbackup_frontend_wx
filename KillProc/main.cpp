#include <windows.h>
#include <stdio.h>
#include "getpid.h"

HRESULT ModifyPrivilege(
    IN LPCTSTR szPrivilege,
    IN BOOL fEnable)
{
    HRESULT hr = S_OK;
    TOKEN_PRIVILEGES NewState;
    LUID             luid;
    HANDLE hToken    = NULL;

    // Open the process token for this process.
    if (!OpenProcessToken(GetCurrentProcess(),
                          TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY,
                          &hToken ))
    {
        printf("Failed OpenProcessToken");
        return ERROR_FUNCTION_FAILED;
    }

    // Get the local unique ID for the privilege.
    if ( !LookupPrivilegeValue( NULL,
                                szPrivilege,
                                &luid ))
    {
        CloseHandle( hToken );
        printf("Failed LookupPrivilegeValue");
        return ERROR_FUNCTION_FAILED;
    }

    // Assign values to the TOKEN_PRIVILEGE structure.
    NewState.PrivilegeCount = 1;
    NewState.Privileges[0].Luid = luid;
    NewState.Privileges[0].Attributes = 
              (fEnable ? SE_PRIVILEGE_ENABLED : 0);

    // Adjust the token privilege.
    if (!AdjustTokenPrivileges(hToken,
                               FALSE,
                               &NewState,
                               0,
                               NULL,
                               NULL))
    {
        printf("Failed AdjustTokenPrivileges");
        hr = ERROR_FUNCTION_FAILED;
    }

    // Close the handle.
    CloseHandle(hToken);

    return hr;
}

int main(int argc, char *argv[])
{
	if(argc==1)
	{
		printf("No process name specified");
		return 2;
	}

	if(! SUCCEEDED(ModifyPrivilege(SE_DEBUG_NAME, TRUE)) )
	{
		printf("Error obtaining debug priviledge");
	}

	if(! SUCCEEDED(ModifyPrivilege(SE_TAKE_OWNERSHIP_NAME, TRUE)) )
	{
		printf("Error obtaining take ownership priviledge");
	}

	DWORD pids[100];
	int npids=getpid(argv[1], pids);

	if(npids==0)
	{
		printf("Cannot find process");
		return 3;
	}

	bool killed_one=false;
	for(int i=0;i<npids;++i)
	{
		HANDLE p=OpenProcess(PROCESS_TERMINATE, FALSE, pids[i]);
		if(p)
		{
			if(!TerminateProcess(p,0) )
			{
				printf("Could not terminate process");
			}
			CloseHandle(p);
			killed_one=true;
		}
		else
		{
			printf("Cannot open process");
		}
	}

	if(killed_one)
		return 1;
	else
		return 0;
}