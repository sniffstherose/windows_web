#include <stdio.h>
#include <windows.h>
#include <process.h>

#define THREAD_NUM 50
unsigned WINAPI threadInc(void* arg);
unsigned WINAPI threadDes(void* arg);

long long num = 0;
CRITICAL_SECTION cs;

int main() {
    HANDLE tHandles[THREAD_NUM];

    InitializeCriticalSection(&cs);
    for (int i = 0; i < THREAD_NUM; ++i) {
        if (i % 2) {
            tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, threadInc, NULL, 0, NULL);
        } else {
            tHandles[i] = (HANDLE)_beginthreadex(NULL, 0, threadDes, NULL, 0, NULL);
        }
    }

    WaitForMultipleObjects(THREAD_NUM, tHandles, TRUE, INFINITE);
    printf("result: %lld\n", num);
    DeleteCriticalSection(&cs);
    return 0;
}

unsigned WINAPI threadInc(void* arg) {

    EnterCriticalSection(&cs);
    for (int i = 0; i < 50000000; ++i) {
        ++num;
    }
    LeaveCriticalSection(&cs);
    return 0;
}

unsigned WINAPI threadDes(void* arg) {
    EnterCriticalSection(&cs);
    for (int i = 0; i < 50000000; ++i) {
        --num;
    }
    LeaveCriticalSection(&cs);
    return 0;
}