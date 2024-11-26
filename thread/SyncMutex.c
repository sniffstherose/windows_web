#include <stdio.h>
#include <windows.h>
#include <process.h>

#define THREAD_NUM 50
unsigned WINAPI threadInc(void* arg);
unsigned WINAPI threadDes(void* arg);

long long num = 0;
HANDLE hMutex;

int main() {
    HANDLE tHandles[THREAD_NUM];

    hMutex = CreateMutex(NULL, FALSE, NULL);
    for (int i = 0; i < THREAD_NUM; ++i) {
        if (i % 2) {
            tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, threadInc, NULL, 0, NULL);
        } else {
            tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, threadDes, NULL, 0, NULL);
        }
    }

    WaitForMultipleObjects(THREAD_NUM, tHandles, TRUE, INFINITE);
    printf("result: %lld\n", num);
    CloseHandle(hMutex);
    return 0;
}

unsigned WINAPI threadInc(void* arg) {

    WaitForSingleObject(hMutex, INFINITE);
    for (int i = 0; i < 50000000; ++i) {
        ++num;
    }
    ReleaseMutex(hMutex);
    return 0;
}

unsigned WINAPI threadDes(void* arg) {
    WaitForSingleObject(hMutex, INFINITE);
    for (int i = 0; i < 50000000; ++i) {
        --num;
    }
    ReleaseMutex(hMutex);
    return 0;
}