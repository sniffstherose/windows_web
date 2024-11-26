#include <stdio.h>
#include <windows.h>
#include <process.h>

unsigned WINAPI ThreadFunc(void* arg);

int main() {
    HANDLE hThread;
    unsigned threadID;
    DWORD wr;
    int param = 5;

    hThread = (HANDLE)_beginthreadex(NULL, 0, ThreadFunc, (void*)&param, 0, &threadID);
    if (hThread == 0) {
        puts("_beginthreadex error.");
        return -1;
    }

    if ((wr = WaitForSingleObject(hThread, INFINITE)) == WAIT_FAILED) {
        puts("thread wait error.");
        return -1;
    }

    printf("wait result: %s\n", (wr == WAIT_OBJECT_0) ? "signaled" : "time-out");
    puts("end of main.");
    return 0;
}

unsigned WINAPI ThreadFunc(void* arg) {
    int cnt = *((int*)arg);
    for (int i = 0; i < cnt; ++i) {
        Sleep(1000);
        puts("running thread");
    }
    return 0;
}
