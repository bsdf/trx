// Generated from ST_vmpi_threads.st
// -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*-
// vi: set ts=4 sw=4 expandtab: (add to ~/.vimrc: set modeline modelines=5) */
//
// ***** BEGIN LICENSE BLOCK *****
// Version: MPL 1.1/GPL 2.0/LGPL 2.1
//
// The contents of this file are subject to the Mozilla Public License Version
// 1.1 (the "License"); you may not use this file except in compliance with
// the License. You may obtain a copy of the License at
// http://www.mozilla.org/MPL/
//
// Software distributed under the License is distributed on an "AS IS" basis,
// WITHOUT WARRANTY OF ANY KIND, either express or implied. See the License
// for the specific language governing rights and limitations under the
// License.
//
// The Original Code is [Open Source Virtual Machine.].
//
// The Initial Developer of the Original Code is
// Adobe System Incorporated.
// Portions created by the Initial Developer are Copyright (C) 2004-2006
// the Initial Developer. All Rights Reserved.
//
// Contributor(s):
//   Adobe AS3 Team
//
// Alternatively, the contents of this file may be used under the terms of
// either the GNU General Public License Version 2 or later (the "GPL"), or
// the GNU Lesser General Public License Version 2.1 or later (the "LGPL"),
// in which case the provisions of the GPL or the LGPL are applicable instead
// of those above. If you wish to allow use of your version of this file only
// under the terms of either the GPL or the LGPL, and not to allow others to
// use your version of this file under the terms of the MPL, indicate your
// decision by deleting the provisions above and replace them with the notice
// and other provisions required by the GPL or the LGPL. If you do not delete
// the provisions above, a recipient may use your version of this file under
// the terms of any one of the MPL, the GPL or the LGPL.
//
// ***** END LICENSE BLOCK ***** */

// Bugzilla 543560 - here we risk deleting an object that is still on the mark stack because
// of how we perform large-object splitting.  The setup is that user code that deletes the object
// gets to run after the first part of the large object has been popped off the mark stack
// but before the rest has been handled.

#include <gtest/gtest.h>
#include "vmbase.h"

using namespace vmbase;

namespace threadtests {

    class ThreadTestBase {
        public:
            struct ThreadRecord {
                vmpi_thread_t thread;
                int id;
                bool startupOk;
            };

            ThreadTestBase(int threadQty, int iterations) : m_threadQty(threadQty), m_iterations(iterations), sharedCounter(0) {
                VMPI_recursiveMutexInit(&m_mutex);
                VMPI_condVarInit(&m_condition);
            }
            virtual ~ThreadTestBase() {
                VMPI_recursiveMutexDestroy(&m_mutex);
                VMPI_condVarDestroy(&m_condition);
            }

            void runTest() {

                ThreadRecord* threads = new ThreadRecord[m_threadQty];

                // Start up the threads
                for (int i = 0; i < m_threadQty; i++) {
                    threads[i].id = i;
                    threads[i].startupOk = VMPI_threadCreate(&threads[i].thread, NULL, _workerThreadEntry, this);
                }

                // ...then block until they all terminate
                for (int i = 0; i < m_threadQty; i++) {
                    if (threads[i].startupOk) {
                        VMPI_threadJoin(threads[i].thread);
                    }
                }

                delete[] threads;
            }
            
            static vmpi_thread_rtn_t VMPI_THREAD_START_CC detachbleThreadEntry(vmpi_thread_arg_t) {
                VMPI_threadDetach(VMPI_currentThread());
                return NULL;
            }
            
            static vmpi_thread_rtn_t VMPI_THREAD_START_CC joinableThreadEntry(vmpi_thread_arg_t) {
                return NULL;
            }

        protected:
            virtual void workerThreadEntry() {}

        private:
            static vmpi_thread_rtn_t VMPI_THREAD_START_CC _workerThreadEntry(vmpi_thread_arg_t arg) {
                ((ThreadTestBase*)arg)->workerThreadEntry();
                return NULL;
            }

        protected:
            const int m_threadQty;
            vmpi_mutex_t m_mutex;
            vmpi_condvar_t m_condition;
            const int m_iterations;
            
        public:
            int sharedCounter;
    };
    
    class MutexTest : public ThreadTestBase {
        public:
            MutexTest(int threadQty, int iterations) : ThreadTestBase(threadQty, iterations) {}
            virtual ~MutexTest() {}

            virtual void workerThreadEntry() {
                for (int i = 0; i < m_iterations; i++) {
                    VMPI_recursiveMutexLock(&m_mutex);
                    VMPI_recursiveMutexLock(&m_mutex);
                    sharedCounter++;
                    VMPI_recursiveMutexUnlock(&m_mutex);
                    VMPI_recursiveMutexUnlock(&m_mutex);
                }
            }
    };
    
    
    class ConditionTest : public ThreadTestBase {
        public:
            ConditionTest(int threadQty, int iterations) : ThreadTestBase(threadQty, iterations), m_finishedQty(0) {}
            virtual ~ConditionTest() {}

            virtual void workerThreadEntry() {
                if (m_threadQty < 2) {
                    return;
                }

                for (int i = 0; i < m_iterations; i++) {
                    VMPI_recursiveMutexLock(&m_mutex);
                    sharedCounter++;
                    
                    // If there's another thread still active then wait.
                    if (m_finishedQty < m_threadQty - 1) {
                        VMPI_condVarSignal(&m_condition);
                        VMPI_condVarWait(&m_condition, &m_mutex);
                    }
                    
                    // This thread has finished, so let's wake everyone else up
                    if (i == m_iterations - 1) {
                        m_finishedQty++;
                        VMPI_condVarBroadcast(&m_condition);
                    }
                    VMPI_recursiveMutexUnlock(&m_mutex);

                }

            }

        private:
            int m_finishedQty;
    };

    class AtomicIncrementTest : public ThreadTestBase {
        public:
            AtomicIncrementTest(int threadQty, int iterations, bool withBarrier) : ThreadTestBase(threadQty, iterations), m_withBarrier(withBarrier) {}
            virtual ~AtomicIncrementTest() {}

            virtual void workerThreadEntry() {
                if (m_withBarrier) {
                    for (int i = 0; i < m_iterations; i++) {
                        VMPI_atomicIncAndGet32WithBarrier(&sharedCounter);
                    }
                } else {
                    for (int i = 0; i < m_iterations; i++) {
                        VMPI_atomicIncAndGet32(&sharedCounter);
                    }
                }
            }
        private:
            bool m_withBarrier;
    };

    class AtomicDecrementTest : public ThreadTestBase {
        public:
            AtomicDecrementTest(int threadQty, int iterations, bool withBarrier) : ThreadTestBase(threadQty, iterations), m_withBarrier(withBarrier) {}
            virtual ~AtomicDecrementTest() {}

            virtual void workerThreadEntry() {
                if (m_withBarrier) {
                    for (int i = 0; i < m_iterations; i++) {
                        VMPI_atomicDecAndGet32WithBarrier(&sharedCounter);
                    }
                } else {
                    for (int i = 0; i < m_iterations; i++) {
                        VMPI_atomicDecAndGet32(&sharedCounter);
                    }
                }
            }
        private:
            bool m_withBarrier;
    };

    class CASTest : public ThreadTestBase {
        public:
            CASTest(int threadQty, int iterations, bool withBarrier) : ThreadTestBase(threadQty, iterations), m_withBarrier(withBarrier) {}
            virtual ~CASTest() {}

            virtual void workerThreadEntry() {
                if (m_withBarrier) {
                    for (int i = 0; i < m_iterations; i++) {
                        int32_t current, next;
                        do {
                            current = sharedCounter;
                            next = current + 1;
                        } while (!VMPI_compareAndSwap32WithBarrier(current, next, &sharedCounter));
                    }
                } else {
                    for (int i = 0; i < m_iterations; i++) {
                        int32_t current, next;
                        do {
                            current = sharedCounter;
                            next = current + 1;
                        } while (!VMPI_compareAndSwap32(current, next, &sharedCounter));
                    }
                }
            }
        private:
            bool m_withBarrier;
    };

    /**
     * We protect a shared counter with a Dekker-style lock that has been made
     * sequentially consistent with memory barriers.
     *
     * The idea is that if the barriers are correct, then two threads can compete
     * to update the counter n times each, so that the final counter value is 2n. If
     * the final value is not 2n, then the barriers have failed to ensure sequential
     * consistency.
     *
     * FIXME: bug 609943
     * This seems way too complicated. We have to be confident in the algorithm
     * before considering the barrier implementations, and I'm not convinced as yet.
     * Is there something simpler?
     *
     * This is test is not actually run. The verifyPass below just returns true.
     */
    class MemoryBarrierTest : public ThreadTestBase {
        public:
            MemoryBarrierTest(int iterations) : ThreadTestBase(2, iterations), m_thread0(0), m_thread1(0), m_turn(NULL) {}
            virtual ~MemoryBarrierTest() {}

            virtual void workerThreadEntry() {

                volatile int* me;
                volatile int* other;
                volatile int* const counterp = &sharedCounter;

                VMPI_recursiveMutexLock(&m_mutex);
                if (m_turn == NULL) {
                    me = &m_thread0;
                    other = &m_thread1;
                    m_turn = me;
                } else {
                    me = &m_thread1;
                    other = &m_thread0;
                }
                VMPI_recursiveMutexUnlock(&m_mutex);

                for (int i = 0; i < m_iterations; i++) {
                    // Dekker lock
                    *me = 1;
                    VMPI_memoryBarrier();
                    while (*other == 1) {
                        VMPI_memoryBarrier();
                        if (m_turn == other) {
                            VMPI_memoryBarrier();
                            *me = 0;
                            VMPI_memoryBarrier();
                            while (m_turn == other) {
                                VMPI_memoryBarrier();
                            }
                            VMPI_memoryBarrier();
                            *me = 1;
                            VMPI_memoryBarrier();
                        }
                    }
                    VMPI_memoryBarrier();
                    (*counterp)++;
                    VMPI_memoryBarrier();
                    m_turn = other;
                    VMPI_memoryBarrier();
                    *me = 0;
                    VMPI_memoryBarrier();
                }
        }
        private:
            volatile int m_thread0;
            volatile int m_thread1;
            volatile int* volatile m_turn;
    };
    
    class TryLockTest : public ThreadTestBase {
        public:
            TryLockTest(int threadQty, int iterations) : ThreadTestBase(threadQty, iterations) {}
            virtual ~TryLockTest() {}

            virtual void workerThreadEntry() {
                for (int i = 0; i < m_iterations; i++) {
                    int counter = 0;
                    while (!VMPI_recursiveMutexTryLock(&m_mutex)) {
                        ++counter;
                        if((counter & 63) == 0) {
                            VMPI_threadSleep(1);
                        } 
                    }
                    sharedCounter++;
                    VMPI_recursiveMutexUnlock(&m_mutex);
                }
            }
    };
    
    class ConditionWithWaitTest : public ThreadTestBase {
        public:
            ConditionWithWaitTest(int threadQty, int iterations) : ThreadTestBase(threadQty, iterations) {}
            virtual ~ConditionWithWaitTest() {}

            virtual void workerThreadEntry() {
                for (int i = 0; i < m_iterations; i++) {
                    VMPI_recursiveMutexLock(&m_mutex);
                    sharedCounter++;
                    VMPI_condVarTimedWait(&m_condition, &m_mutex, 1);
                    VMPI_recursiveMutexUnlock(&m_mutex);
                }
            }
    };
    
    class SleepTest : public ThreadTestBase {
        public:
            SleepTest(int threadQty, int iterations) : ThreadTestBase(threadQty, iterations) {}
            virtual ~SleepTest() {}

            virtual void workerThreadEntry() {
                for (int i = 0; i < m_iterations; i++) {
                    VMPI_recursiveMutexLock(&m_mutex);
                    sharedCounter++;
                    VMPI_recursiveMutexUnlock(&m_mutex);
                    VMPI_threadSleep(1);
                }
            }
    };
}
    
using namespace threadtests;

// This needs to be at least 2 for ConditionTest
#define THREAD_QTY 4
#define ITERATIONS 100000

TEST(VMPIThreads, Mutex) {
    MutexTest mutexTest(THREAD_QTY, ITERATIONS);
    mutexTest.runTest();
    ASSERT_EQ(THREAD_QTY * ITERATIONS, mutexTest.sharedCounter);
}

TEST(VMPIThreads, Condition) {
    ConditionTest conditionTest(THREAD_QTY, ITERATIONS);
    conditionTest.runTest();
    ASSERT_EQ(THREAD_QTY * ITERATIONS, conditionTest.sharedCounter);
}

TEST(VMPIThreads, AtomicIncrement) {
    AtomicIncrementTest atomicIncrementTest(THREAD_QTY, ITERATIONS, false);
    atomicIncrementTest.runTest();
    ASSERT_EQ(THREAD_QTY * ITERATIONS, atomicIncrementTest.sharedCounter);
}

TEST(VMPIThreads, AtomicDecrement) {
    AtomicDecrementTest atomicDecrementTest(THREAD_QTY, ITERATIONS, false);
    atomicDecrementTest.runTest();
    ASSERT_EQ(-(THREAD_QTY * ITERATIONS), atomicDecrementTest.sharedCounter);
}

TEST(VMPIThreads, CAS) {
    CASTest casTest(THREAD_QTY, ITERATIONS, false);
    casTest.runTest();
    ASSERT_EQ(THREAD_QTY * ITERATIONS, casTest.sharedCounter);
}

TEST(VMPIThreads, AtomicIncrement2) {
    AtomicIncrementTest atomicIncrementTest(THREAD_QTY, ITERATIONS, true);
    atomicIncrementTest.runTest();
    ASSERT_EQ(THREAD_QTY * ITERATIONS, atomicIncrementTest.sharedCounter);
}

TEST(VMPIThreads, AtomicDecrement2) {
    AtomicDecrementTest atomicDecrementTest(THREAD_QTY, ITERATIONS, true);
    atomicDecrementTest.runTest();
    ASSERT_EQ(-(THREAD_QTY * ITERATIONS), atomicDecrementTest.sharedCounter);
}

TEST(VMPIThreads, CAS2) {
    CASTest casTest(THREAD_QTY, ITERATIONS, true);
    casTest.runTest();
    ASSERT_EQ(THREAD_QTY * ITERATIONS, casTest.sharedCounter);
}

TEST(VMPIThreads, MemoryBarrier) {
#if 0
    /* This test is failing on Windows and Mac OSX 10.4.
     * For Windows, see bug 609820.
     * For Mac, are the 10.4 APIs not reliable?
     * It could also be the test, or the compiler!
     * FIXME: bug 609943 Selftests to stress memory barriers (fences)
     */
    // Note that the memory barrier test is based on a Dekker lock, so we
    // only ever use 2 threads.
    MemoryBarrierTest memoryBarrierTest(ITERATIONS);
    memoryBarrierTest.runTest();
    ASSERT_EQ(2 * ITERATIONS, memoryBarrierTest.sharedCounter);
#endif
}

TEST(VMPIThreads, TryLock) {
    TryLockTest tryLockTest(THREAD_QTY, ITERATIONS);
    tryLockTest.runTest();
    ASSERT_EQ(THREAD_QTY * ITERATIONS, tryLockTest.sharedCounter);
}

TEST(VMPIThreads, ConditionWithWait) {
    // Use 2000 iterations with a 1 ms wait
    ConditionWithWaitTest conditionWithWaitTest(THREAD_QTY, 2000);
    conditionWithWaitTest.runTest();
    ASSERT_EQ(THREAD_QTY * 2000, conditionWithWaitTest.sharedCounter);
}

TEST(VMPIThreads, Sleep) {
    // Use 2000 iterations with a 1 ms sleep
    SleepTest sleepTest(THREAD_QTY, 2000);
    sleepTest.runTest();
    ASSERT_EQ(THREAD_QTY * 2000, sleepTest.sharedCounter);
}

TEST(VMPIThreads, ThreadCreate) {
    // We can't really test for much here...
    vmpi_thread_t thread;
    bool started = VMPI_threadCreate(&thread, NULL, ThreadTestBase::detachbleThreadEntry, NULL);
    ASSERT_TRUE(started);
}

TEST(VMPIThreads, ThreadCreate2) {
    vmpi_thread_attr_t attr;
    ASSERT_TRUE(VMPI_threadAttrInit(&attr));
    
    VMPI_threadAttrSetGuardSize(&attr, VMPI_threadAttrDefaultGuardSize());
    VMPI_threadAttrSetStackSize(&attr, VMPI_threadAttrDefaultStackSize());
    VMPI_threadAttrSetPriorityLow(&attr);
    VMPI_threadAttrSetPriorityNormal(&attr);
    VMPI_threadAttrSetPriorityHigh(&attr);
    
    vmpi_thread_t thread;
    bool started = VMPI_threadCreate(&thread, &attr, ThreadTestBase::joinableThreadEntry, NULL);
    ASSERT_TRUE(VMPI_threadAttrInit(&attr));
    if (started) {
        VMPI_threadJoin(thread);
    }
}
