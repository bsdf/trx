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

#include <stdlib.h>
#include <sys/time.h>

#include "avmplus.h"

namespace avmplus
{

#ifdef DEBUGGER
    struct IntWriteTimerData
    {
        uint32_t interval; // in microseconds
        pthread_t thread;
        volatile int *addr;
    };

    void *timerThread(void *arg)
    {
        IntWriteTimerData *data = (IntWriteTimerData*)arg;
        volatile int *addr = data->addr;
        uint32_t interval = data->interval;
        while(data->addr)
        {
            usleep(interval);
            *addr = 1;
        }
        pthread_exit(NULL);
        return NULL;
    }

    uintptr_t OSDep::startIntWriteTimer(uint32_t millis, volatile int *addr)
    {
        pthread_t p;
        IntWriteTimerData *data = mmfx_new( IntWriteTimerData() );
        data->interval = millis*1000;
        data->addr = addr;
        pthread_create(&p, NULL, timerThread, data);
        data->thread = p;
        return (uintptr_t)data;
    }

    void OSDep::stopTimer(uintptr_t handle)
    {
        IntWriteTimerData *data = (IntWriteTimerData*) handle;
        data->addr = NULL;
        pthread_join(data->thread, NULL);
        mmfx_delete( data );
    }
#endif // DEBUGGER
}
