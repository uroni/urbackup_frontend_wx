#include <windows.h>
#include "getpid.h"

int main(int argc, char *argv[])
{
	if(argc==1)
		return 2;

	DWORD pid=getpid(argv[1] );

    HANDLE p=OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if(p)
    {
            TerminateProcess(p,0);
            CloseHandle(p);
            return 1;
    }
    else
    {
            return 0;
    }
}