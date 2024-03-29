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

#include "vmbase.h"

namespace vmbase {

    AtomicCounter32 VMThread::m_nextNameSuffix(0);
    VMThreadLocal<VMThread*> VMThread::m_currentThread;

    Runnable::~Runnable()
    {
    }

    WaitNotifyMonitor::WaitNotifyMonitor()
    {
    }

    WaitNotifyMonitor::~WaitNotifyMonitor()
    {
    }

    void WaitNotifyMonitor::wait()
    {
        m_condVar.wait(*this);
    }

    bool WaitNotifyMonitor::wait(int32_t timeoutMillis)
    {
        return m_condVar.wait(*this, timeoutMillis);
    }

    void VMThread::setNameFrom(const char* name)
    {
        size_t nameLength = VMPI_strlen(name);
        char* buf = new char[nameLength + 1];
        VMPI_strncpy(buf, name, nameLength + 1);
        m_name = buf;
    }

    VMThread::VMThread()
        : m_state(NOT_STARTED)
        , m_joinerQty(0)
    {
        m_runnable = this;
        int suffix = m_nextNameSuffix.incAndGet();
        char buf[64];
        VMPI_snprintf(buf, sizeof(buf), "Thread-%d", suffix);
        setNameFrom(buf);
    }

    VMThread::VMThread(const char* name)
        : m_state(NOT_STARTED)
        , m_joinerQty(0)
    {
        m_runnable = this;
        setNameFrom(name);
    }

    VMThread::VMThread(Runnable* runnable)
        : m_runnable(runnable)
        , m_state(NOT_STARTED)
        , m_joinerQty(0)
    {
        int suffix = m_nextNameSuffix.incAndGet();
        char buf[64];
        VMPI_snprintf(buf, sizeof(buf), "Thread-%d", suffix);
        setNameFrom(buf);
    }

    VMThread::VMThread(const char* name, Runnable* runnable)
        : m_runnable(runnable)
        , m_state(NOT_STARTED)
        , m_joinerQty(0)
    {
        setNameFrom(name);
    }

    VMThread::~VMThread()
    {
        // Before a VMThread is freed the freeing thread must ensure
        // that the VMThread has notified all of its joiners, and that
        // all of the joiners have released the VMThread's
        // m_joinMonitor after being notified. To accomplish this, just
        // having the freeing thread join the VMThread isn't enough,
        // the VMThread dtor must explicitly wait for the joiners to
        // release the mutex. Note that the VMThread's m_status cannot
        // be used as a wait-guard as the freeing thread may see the
        // update to TERMINATED before one of the joiners. So we have
        // to count joining threads in and out, and have the VMThread
        // dtor wait until there are zero joiners.
        SCOPE_LOCK_SP_NAMED(locker, m_joinMonitor) {
            if (m_state != NOT_STARTED) {
                while (m_state == RUNNABLE || m_joinerQty != 0) {
                    locker.wait();
                }
            }
        }
        delete [] m_name;
    }

    bool VMThread::start()
    {
        return start(NULL);
    }

    bool VMThread::start(ThreadPriority priority)
    {
        vmpi_thread_attr_t attr;
        VMPI_threadAttrInit(&attr);
        if (priority == HIGH_PRIORITY) {
            VMPI_threadAttrSetPriorityHigh(&attr);
        } else if (priority == LOW_PRIORITY) {
            VMPI_threadAttrSetPriorityLow(&attr);
        }
        return start(&attr);
    }

    bool VMThread::start(ThreadPriority priority, size_t stackSize, size_t guardSize)
    {
        vmpi_thread_attr_t attr;
        VMPI_threadAttrInit(&attr);
        if (priority == HIGH_PRIORITY) {
            VMPI_threadAttrSetPriorityHigh(&attr);
        } else if (priority == LOW_PRIORITY) {
            VMPI_threadAttrSetPriorityLow(&attr);
        }
        if (!VMPI_threadAttrSetStackSize(&attr, stackSize)) {
            return false;
        }
        if (!VMPI_threadAttrSetGuardSize(&attr, guardSize)) {
            return false;
        }
        return start(&attr);
    }

    bool VMThread::start(vmpi_thread_attr_t* attr)
    {
        SCOPE_LOCK_SP(m_joinMonitor) {
            assert(m_state == NOT_STARTED);
            if (m_state == NOT_STARTED) {
                if (VMPI_threadCreate(&m_threadID, attr, VMThread::startInternal, (vmpi_thread_arg_t) (this))) {
                    m_state = RUNNABLE;
                    return true;
                }
            }
        }
        return false;
    }

    void VMThread::join()
    {
        SCOPE_LOCK_SP_NAMED(locker, m_joinMonitor) {
            assert(m_state != NOT_STARTED);
            if (m_state != TERMINATED) {
                m_joinerQty++;
                do {
                    locker.wait();
                } while (m_state != TERMINATED);
                m_joinerQty--;
            }
        }
    }

    /* static */ vmpi_thread_rtn_t VMPI_THREAD_START_CC VMThread::startInternal(vmpi_thread_arg_t args)
    {
        VMThread* thread = (VMThread*) (args);
        VMThread::m_currentThread.set(thread);

        // Block until we know our parent thread has finished its side of the setup
        SCOPE_LOCK_NO_SP(thread->m_joinMonitor) {
            assert(thread->m_state == RUNNABLE);
        }

        thread->m_runnable->run();

        // VMThread has logically terminated. Cleanup and notify all joiners.
        SCOPE_LOCK_NO_SP_NAMED(locker, thread->m_joinMonitor) {
            VMPI_threadDetach(thread->m_threadID);
            thread->m_state = TERMINATED;
            locker.notifyAll();
        }
        return 0;
    }

    const char* VMThread::getName() const
    {
        return m_name;
    }

    void VMThread::run()
    {
        // Override me.
    }

#ifdef VMCFG_SAFEPOINTS
    /**
     * Wraps calls to VMPI_threadSleep with a register flush and safepoint gate
     */
    class SafepointHelper_VMThreadSleep {
    private:
        static void sleepInSafepointGate(void* stackPointer, void* timeout)
        {
            SafepointGate gate(stackPointer);
            VMPI_threadSleep((int32_t)(intptr_t)timeout);
        }
    public:
        static void sleep(int32_t timeout)
        {
            VMPI_callWithRegistersSaved(sleepInSafepointGate, (void*)timeout);
        }
    };

    /* static */ void VMThread::sleep(int32_t timeout)
    {
        if (SafepointRecord::hasCurrent()) {
            SafepointHelper_VMThreadSleep::sleep(timeout);
        } else {
            // Thread has no current safepoint context, so just sleep.
            VMPI_threadSleep(timeout);
        }
    }
#else
    /* static */ void VMThread::sleep(int32_t timeout)
    {
        VMPI_threadSleep(timeout);
    }
#endif
}
