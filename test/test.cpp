#include <stdio.h>
#include <windows.h>

int main(void) {
    SYSTEM_INFO si;
    GetSystemInfo(&si);

    printf("The page size for this system is 0x%lx bytes.\n", si.dwPageSize);

    return 0;
}