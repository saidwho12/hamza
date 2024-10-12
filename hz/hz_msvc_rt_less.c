#ifdef HZ_BUILD_SHARED

#include <corecrt.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <minwinbase.h>
#include <processthreadsapi.h>

BOOL APIENTRY _DllMainCRTStartup(HANDLE hDllHandle, DWORD dwReason, LPVOID lpReserved) {
    return TRUE;
}

#endif /* HZ_BUILD_SHARED */

#define attrib_used __attribute__((used))

// Don't actually know if you need the value, since looks like it works even without it, I myself leave it with the value.
attrib_used int _fltused = 0x9875;
#ifdef _WIN64
attrib_used __cdecl void __chkstk(void) {
    __noop();
}
#elif _WIN32
attrib_used __declspec(naked) void __chkstk(void) {
    __asm { ret };
}
#endif

void* __cdecl memset(void* dest, int val, size_t n)
{
    char* retDest = (char*)dest;

    while (n--)
        *retDest++ = (char)val;

    return retDest;
}

#if 0
#define HZ_MAX_THREADS 64
#define HZ_PER_THREAD_STACK_SIZE 8192

struct HzRange {
    int min_index, max_index; 
};

struct HzMarker {
    unsigned long user_id;
    int x1;
    int y1;
    int x2;
    int y2;
    short x2_shift;
    short y2_shift;
};

typedef void (*HzApplyLookupFn) (HzLookupTable *table);
struct HzShaper {
    int num_threads;
    _Atomic int joined_threads;

};

struct HzProcessLookupThreadData {
    struct HzBuffer *A, *B;
    struct HzRange range;
};


DWORD hz_apply_gsub_lookup_thread_proc(_In_ LPVOID lpParameter) {
    struct HzProcessLookupThreadData* threadData = (struct HzProcessLookupThreadData*)lpParameter;

    threadData->
}

void process_gsub_lookup_threaded_win(int num_threads,
                                      struct HzBuffer* in_buffer, struct HzBuffer* out_buffer,
                                      struct HzMarker* markers,
                                      size_t num_markers,
                                      struct HzRange r,
                                      int depth)
{
    unsigned char stk[4096];
    struct HzMemoryArena arena = hz_memory_arena_create(stk, sizeof stk);

    if (depth >= HZ_MAX_RECURSE_DEPTH) {
        // hit max recurse depth, wind back up the call stack
        return;
    }

    struct HzProcessLookupThreadData threads_data[] = hz_memory_arena_alloc(&arena, num_threads * sizeof(*threads_data));
    HANDLE 

    int glyphs_per_thread = ((r.max_index - r.min_index) + 1) / num_threads;

    for (int i = 0; i < num_threads; ++i) {
        threads_data[i].A = in_buffer;
        threads_data[i].B = out_buffer;
        threads_data[i].range = (struct HzRange){
            i*glyphs_per_thread,
            HZ_MIN((i+1)*glyphs_per_thread-1, r.max_index)
        };

        CreateThread(
            NULL,
            HZ_PER_THREAD_STACK_SIZE,
            (LPTHREAD_START_ROUTINE) &hz_apply_gsub_lookup_thread_proc,
            (LPVOID) &threads_data[i],
            0, NULL);

        SetThreadAffinityMask();
    }

}

#endif
