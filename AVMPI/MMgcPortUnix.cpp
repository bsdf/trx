/* -*- Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
/* vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
/* ***** BEGIN LICENSE BLOCK *****
* Version: MPL 1.1/GPL 2.0/LGPL 2.1
*
* The contents of this file are subject to the Mozilla Public License Version
* 1.1 (the "License"); you may not use this file except in compliance with
* the License. You may obtain a copy of the License at
* http://www.mozilla.org/MPL/
*
* Software distributed under the License is distributed on an "AS IS" basis,
* WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
* for the specific language governing rights and limitations under the
* License.
*
* The Original Code is [Open Source Virtual Machine.].
*
* The Initial Developer of the Original Code is
* Adobe System Incorporated.
* Portions created by the Initial Developer are Copyright (C) 2004-2006
* the Initial Developer. All Rights Reserved.
*
* Contributor(s):
*   Adobe AS3 Team
*
* Alternatively, the contents of this file may be used under the terms of
* either the GNU General Public License Version 2 or later (the "GPL"), or
* the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
* in which case the provisions of the GPL or the LGPL are applicable instead
* of those above. If you wish to allow use of your version of this file only
* under the terms of either the GPL or the LGPL, and not to allow others to
* use your version of this file under the terms of the MPL, indicate your
* decision by deleting the provisions above and replace them with the notice
* and other provisions required by the GPL or the LGPL. If you do not delete
* the provisions above, a recipient may use your version of this file under
* the terms of any one of the MPL, the GPL or the LGPL.
*
* ***** END LICENSE BLOCK ***** */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>

#include "MMgc.h"

#if !defined(MAP_ANONYMOUS) && defined(MAP_ANON)
    #define MAP_ANONYMOUS MAP_ANON
#endif // !defined(MAP_ANONYMOUS) && defined(MAP_ANON)

#if defined(MMGC_MEMORY_INFO) && defined(AVMPLUS_UNIX)
    #ifndef _GNU_SOURCE
        #define _GNU_SOURCE
    #endif
    #include <dlfcn.h>
#endif

#ifdef SOLARIS
    #include <procfs.h>
    #include <sys/stat.h>
    #include <ucontext.h>
    #include <sys/frame.h>
    #include <sys/stack.h>
    typedef caddr_t maddr_ptr;
#else
    typedef void *maddr_ptr;
#endif

#include <fcntl.h>

// It's possible to use the flushw instruction on sparc9, but this should always work

#if defined SOLARIS && defined MMGC_SPARC
    #define FLUSHWIN() asm("ta 3");
#else
    #define FLUSHWIN()
#endif

#if defined linux && !defined VMCFG_MIPS && !defined VMCFG_ARM
#include <execinfo.h>
#define HAVE_BACKTRACE
#endif


bool AVMPI_canMergeContiguousRegions()
{
    return true;
}

bool AVMPI_canCommitAlreadyCommittedMemory()
{
    return true;
}

bool AVMPI_useVirtualMemory()
{
#ifdef MMGC_SPARC
    return false;
#endif
    return true;
}

bool AVMPI_areNewPagesDirty()
{
    return false;
}

void* AVMPI_reserveMemoryRegion(void* address, size_t size)
{
    char *addr = (char*)mmap((maddr_ptr)address,
                             size,
                             PROT_NONE,
                             MAP_PRIVATE | MAP_ANONYMOUS,
                             -1, 0);
    if (addr == MAP_FAILED) {
        return NULL;
    }
    if(address && address != addr) {
        // behave like windows and fail if we didn't get the right address
        AVMPI_releaseMemoryRegion(addr, size);
        return NULL;
    }
    return addr;
}

bool AVMPI_releaseMemoryRegion(void* address, size_t size)
{
    int result = munmap((maddr_ptr)address, size);
    return (result == 0);
}

bool AVMPI_commitMemory(void* address, size_t size)
{
    char *addr = (char*)mmap((maddr_ptr)address,
                             size,
                             PROT_READ | PROT_WRITE,
                             MAP_PRIVATE | MAP_FIXED | MAP_ANONYMOUS,
                             -1, 0);

    size_t pageSize = VMPI_getVMPageSize();
    char* temp_addr = addr;
    while( temp_addr < (addr+size))
    {
        // Touch each page
        *temp_addr = 0;
        temp_addr += pageSize; //verify rishit
    }
    return addr == address;
}

bool AVMPI_decommitMemory(char *address, size_t size)
{
    // re-map it as PROT_NONE

    // NOTE: we don't release it hear like Mac does, if we do another thread in the process
    // could reserve it after we munmap it and even worse if that happened the mmap call would
    // still work causing both mmap callers to think they mapped the memory.  Mac does have
    // to release first but it can tell that the following reserve succeeded or not.
    char *addr = (char*)mmap((maddr_ptr)address,
                             size,
                             PROT_NONE,
                             MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                             -1, 0);
    return addr == address;
}

void* AVMPI_allocateAlignedMemory(size_t size)
{
    return valloc(size);
}

void AVMPI_releaseAlignedMemory(void* address)
{
    free(address);
}

#define state_newline  1
#define state_skipline  2
#define state_P 3
#define state_Pr 4
#define state_Private 6
#define state_size 7

// Note: the linux #define provided by the compiler.

#ifdef linux

size_t AVMPI_getPrivateResidentPageCount()
{
    char buff[32];
    VMPI_snprintf(buff, sizeof(buff), "/proc/%d/smaps", getpid());
    int smap_hndl = open(buff, O_RDONLY);
    if (smap_hndl == -1)
        return 0;

    size_t priv_pages = 0;
    size_t pageSize = VMPI_getVMPageSize();
    uint32_t state = state_newline;
    char size_buff[16];
    uint32_t size_idx = 0;
    int read_size = 0;
    while( (read_size = read(smap_hndl, buff, 32)) )
    {
        int i = 0;
        while(i < read_size )
        {
            char c = buff[i++];
            switch( state )
            {
            case state_newline:
                if( c == 'P' )
                    state = state_P;
                else
                    state = state_skipline;
                break;

            case state_skipline:
                if( c == '\n' )
                    state = state_newline;
                break;

            case state_P:
                if( c == 'r' )
                    state = state_Pr;
                else
                    state = state_skipline;
                break;

            case state_Pr:
                if( c == 'i' )  // Good enough, nothing else in smaps starts with Pr
                    state = state_Private;
                else
                    state = state_skipline;
                break;

            case state_Private:
                if ( c >= '0' && c <= '9' )
                {
                    state = state_size;
                    size_buff[size_idx++] = c;
                }
                else if ( c == '\n')
                    state = state_newline;
                break;

            case state_size:
                if( c >= '0' && c <= '9' )
                    size_buff[size_idx++] = c;
                else
                {
                    size_buff[size_idx] = 0;
                    size_idx = 0;
                    uint32_t size = VMPI_atoi(size_buff)*1024;
                    uint32_t blocks = size/pageSize;
                    if( size % pageSize != 0 )
                        ++blocks;
                    priv_pages += blocks;

                    if ( c == '\n' )
                        state = state_newline;
                    else
                        state = state_skipline;
                }
                break;
            }
        }
    }
    close(smap_hndl);

    return priv_pages;
}

#elif defined SOLARIS

size_t AVMPI_getPrivateResidentPageCount()
{
    char mapname[32];
    int mapfd, nmap, i;
    prxmap_t *prmapp, *pmp;
    struct stat st;
    ssize_t n;
    size_t priv_pages = 0;

    VMPI_snprintf(mapname, sizeof (mapname), "/proc/%d/xmap", getpid());

    if ((mapfd = open(mapname, O_RDONLY)) < 0 || fstat(mapfd, &st) != 0) {
        if (mapfd >= 0)
            (void) close(mapfd);
        return 0;
    }

    nmap = st.st_size / sizeof (prxmap_t);
    nmap *= 2;
again:
    prmapp = (prxmap_t *)malloc((nmap + 1) * sizeof (prxmap_t));

    if ((n = pread(mapfd, prmapp, (nmap + 1) * sizeof (prxmap_t), 0)) < 0) {
        (void) close(mapfd);
        free(prmapp);
        return 0;
    }

    if (nmap < n / sizeof (prxmap_t)) {
        free(prmapp);
        nmap *= 2;
        goto again;
    }

    (void) close(mapfd);
    nmap = n / sizeof (prxmap_t);

    for (i = 0, pmp = prmapp; i < nmap; i++, pmp++) {
        priv_pages += pmp->pr_anon;
    }
    free(prmapp);
    return priv_pages;
}

#else

size_t AVMPI_getPrivateResidentPageCount()
{
    return 0;
}

#endif

#ifdef SOLARIS

pthread_key_t stackTopKey = NULL;

uintptr_t AVMPI_getThreadStackBase()
{
    FLUSHWIN();

    // FIXME: race condition
    if(stackTopKey == NULL)
    {
        int res = pthread_key_create(&stackTopKey, NULL);
        GCAssert(res == 0);
    }

    void *stackTop = pthread_getspecific(stackTopKey);
    if(stackTop)
        return (uintptr_t)stackTop;

    struct frame *sp;
    int i;
    int *iptr;

    stack_t st;
    stack_getbounds(&st);
    uintptr_t stack_base = (uintptr_t)st.ss_sp + st.ss_size;
    pthread_setspecific(stackTopKey, (void*)stack_base);
    return (uintptr_t)stack_base;
}

#elif defined AVMPLUS_UNIX

pthread_key_t stackTopKey = 0;

uintptr_t AVMPI_getThreadStackBase()
{
    // FIXME: race condition
    if(stackTopKey == 0)
    {
#ifdef DEBUG
        int res =
#endif
        pthread_key_create(&stackTopKey, NULL);
        GCAssert(res == 0);
    }

    void *stackTop = pthread_getspecific(stackTopKey);
    if(stackTop)
        return (uintptr_t)stackTop;

    size_t sz;
    void *s_base;
    pthread_attr_t attr;

    pthread_attr_init(&attr);
    // WARNING: stupid expensive function, hence the TLS
    int res = pthread_getattr_np(pthread_self(),&attr);
    GCAssert(res == 0);

    if(res)
    {
        // not good
        return 0;
    }

    res = pthread_attr_getstack(&attr,&s_base,&sz);
    GCAssert(res == 0);
    pthread_attr_destroy(&attr);

    stackTop = (void*) ((size_t)s_base + sz);
    // stackTop has to be greater than current stack pointer
    GCAssert(stackTop > &sz);
    pthread_setspecific(stackTopKey, stackTop);
    return (uintptr_t)stackTop;

}

#elif defined(linux) && defined(MMGC_ARM)

uintptr_t AVMPI_getThreadStackBase()
{
    void* sp;
    pthread_attr_t attr;
    pthread_getattr_np(pthread_self(), &attr);
    pthread_attr_getstackaddr(&attr, &sp);
    return (uintptr_t)sp;
}

#endif


#ifdef MMGC_MEMORY_PROFILER


    #ifdef MMGC_SPARC
        bool AVMPI_captureStackTrace(uintptr_t* buffer, size_t bufferSize, uint32_t framesToSkip)
        {
          // TODO for sparc.
            GCAssert(false);
            return false;
        }
    #endif

    #ifdef MMGC_PPC
        bool AVMPI_captureStackTrace(uintptr_t* buffer, size_t bufferSize, uint32_t framesToSkip)
        {
          register int stackp;
          sintptr pc;
          asm("mr %0,%%r1" : "=r" (stackp));
          while(framesToSkip--) {
            stackp = *(int*)stackp;
          }
          int i=0;
          // save space for 0 terminator
          bufferSize--;
          while(i<bufferSize && stackp) {
            pc = *((sintptr*)stackp+2);
            buffer[i++]=pc;
            stackp = *(int*)stackp;
          }
          buffer[i] = 0;

          return true;
        }
    #endif

    #ifdef MMGC_IA32
        bool AVMPI_captureStackTrace(uintptr_t* buffer, size_t bufferSize, uint32_t framesToSkip)
        {
            void **ebp;
            asm("mov %%ebp, %0" : "=r" (ebp));

            while(framesToSkip-- && *ebp)
            {
                ebp = (void**)(*ebp);
            }

            /* save space for 0 terminator */
            bufferSize--;

            size_t i = 0;

            while (i < bufferSize && *ebp)
            {
                /* store the current frame pointer */
                buffer[i++] = *((uintptr_t*) ebp + 1);
                /* get the next frame pointer */
                ebp = (void**)(*ebp);
            }

            buffer[i] = 0;

            return true;
        }
    #endif

    #ifdef MMGC_ARM
        bool AVMPI_captureStackTrace(uintptr_t* buffer, size_t bufferSize, uint32_t framesToSkip)
        {
            // Android: Filter in gcc and armcc(RVCT)
    #if defined(ANDROID) && (defined(__GNUC__) || defined(__ARMCC_VERSION))
            /*
              The following code will work only when the code is compiled with the full apcs frame,
              For gcc this option is -mapcs-frame
              For APCS - ARM Procedue call standard as done by GCC register naming convention
              ------------------------------------
              Prolog
                ; store old sp -> ip
                mov ip, sp
                ; push regs, fp, ip, lr, pc onto stack -> regs are pushed last, they are at a lowed memory location, pc was pushed first, it is at a higher location
                push    {r4, fp, ip, lr, pc}
                ; The previous instruction modified sp, so take the old sp (which is in ip) and make it point to stored pc (stored pc = old sp - 4 bytes)
                sub fp, ip, #4  ; 0x4
                ; Now mess with sp, grow the stack the size of your local variables
                sub sp, sp, #28 ; 0x1c

              Epilog
                ; sp = topmost stored regs (fp is already = stored pc, subtracrt 4(3 + nStoredRegs))
                sub sp, fp, #16 ; 0x10
                ; restore the stored values, cool stuff!! Restore r4, then fp, then sp (this replaces the current sp with the stored ip, which is the old sp), move lr to pc (return) 
                ldm sp, {r4, fp, sp, pc}

              The fp register points to the place the pc is stored inside the stack.
              The __builtin_frame_address(0) call returns the current contents of the fp

                *(fp - 0) = (pc when the push was executed)
                *(fp - 1) = (lr when the push was executed)
                *(fp - 2) = (ip when the push was executed)
                *(fp - 3) = (fp when the push was executed)
              */
            
            // Current fp.
            void ** pCurrentFrame = (void **)__builtin_frame_address(0);
        
            // Things like dl_iterate_phdr and dlinfo are not present on some platforms.
            // Platforms which have ways to know the load address should use that information
            // if they do not want to traverse inside the stack of other modules.
            void * pCurrentReturnAddress = __builtin_return_address(0);
            uintptr_t iProbableLoadAddress = (uint32_t)pCurrentReturnAddress & 0xF0000000;
                 
            for(int32_t iDepth = -framesToSkip; iDepth < (signed)bufferSize && pCurrentFrame; iDepth++)
            {
                // *(fp - 1) = (lr when the push was executed) - which is the return location,
                // subtract 4 bytes to reach the instruction which actually called the current function.
                uintptr_t lr =  (((uintptr_t)*(pCurrentFrame - 1) - 4));
            
                // Do not store for the first 'framesToSkip' frames.
                if(iDepth >= 0)
                    buffer[iDepth] = lr;
            
                // Old fp, *(fp - 3) = (fp when the push was executed).
                void * pNextFrame = *(pCurrentFrame - 3);
            
                // Make sure you are moving deeper in the stack.
                // Make sure not to do this for other modules, they might not be built with apcs enforced.
                //
                // Stack grows up in memory, next frame should be above the current frame.

                if(pNextFrame > pCurrentFrame && ((lr & 0xF0000000) == iProbableLoadAddress))
                    pCurrentFrame = (void **)pNextFrame;
                else
                    pCurrentFrame = 0;
            }
            return true;
    #else
            (void) buffer;
            (void) bufferSize;
            (void) framesToSkip;
            return false;
    #endif
        }
    #endif

    bool AVMPI_getFunctionNameFromPC(uintptr_t pc, char *buffer, size_t bufferSize)
    {
#ifdef HAVE_BACKTRACE
        char **strings = backtrace_symbols((void**)&pc, 1);
        snprintf(buffer, bufferSize,"%s", strings[0]);
        free(strings);
        return true;
#else
        (void)pc;
        (void)buffer;
        (void)bufferSize;
        return false;
#endif
    }

    bool AVMPI_getFileAndLineInfoFromPC(uintptr_t pc, char *buffer, size_t bufferSize, uint32_t* /*lineNumber*/)
    {
#ifdef HAVE_BACKTRACE
        char **strings = backtrace_symbols((void**)&pc, 1);
        snprintf(buffer, bufferSize,"%s", strings[0]);
        free(strings);
        return true;
#else
        (void)pc;
        (void)buffer;
        (void)bufferSize;
        return false;
#endif
    }


    void AVMPI_setupPCResolution() { }

    void AVMPI_desetupPCResolution() { }

    bool AVMPI_isMemoryProfilingEnabled()
    {
        //read the mmgc profiling option switch
        const char *env = getenv("MMGC_PROFILE");
        return (env && (VMPI_strncmp(env, "1", 1) == 0));
    }

#endif //MEMORY_PROFILER


void AVMPI_cleanStack(size_t amt)
{
    void *space = alloca(amt);
    if(space)
    {
        VMPI_memset(space, 0, amt);
    }
}
