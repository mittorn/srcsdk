#include <jni.h>
#include <android/log.h>
#define HIJACK_SIZE 12
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>


void simple_hook( void *target, void *func )
{
    unsigned char o_code[HIJACK_SIZE], n_code[HIJACK_SIZE];
	size_t pagesize = sysconf(_SC_PAGESIZE);

    if ( (unsigned long)target % 4 == 0 )
    {
        // ldr pc, [pc, #0]; .long addr; .long addr
        memcpy(n_code, "\x00\xf0\x9f\xe5\x00\x00\x00\x00\x00\x00\x00\x00", HIJACK_SIZE);
        *(unsigned long *)&n_code[4] = (unsigned long)func;
        *(unsigned long *)&n_code[8] = (unsigned long)func;
    }
    else // Thumb
    {
        // add r0, pc, #4; ldr r0, [r0, #0]; mov pc, r0; mov pc, r0; .long addr
        memcpy(n_code, "\x01\xa0\x00\x68\x87\x46\x87\x46\x00\x00\x00\x00", HIJACK_SIZE);
        *(unsigned long *)&n_code[8] = (unsigned long)func;
        target--;
    }

	mprotect((char *)(((int) target - (pagesize-1)) & ~(pagesize-1)), pagesize * 2, PROT_READ | PROT_WRITE );

    memcpy(o_code, target, HIJACK_SIZE);

    memcpy(target, n_code, HIJACK_SIZE);
	mprotect((char *)(((int) target - (pagesize-1)) & ~(pagesize-1)), pagesize * 2, PROT_READ | PROT_EXEC );
	cacheflush((long)target, (long)target + HIJACK_SIZE, 0);
}

// _SpewMessage hook hook
int DefaultSpewFunc( int type, char *msg );
void SpewOutputFunc(int (*func)( int, char*));
void *GetSpewOutputFunc();
extern void *g_pVCR;

int _NewSpewMessage( int type, const char *fmt, va_list ap )
{
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* vm, void* x)
{
	// offset seems to be correct in 23-44
	simple_hook( DefaultSpewFunc + 0x50, _NewSpewMessage );

	// force enable text input (no need in SDL patching now)
	SDL_StartTextInput();

	return JNI_VERSION_1_4;
}
