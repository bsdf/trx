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

#include "avmplus.h"

namespace avmplus
{
    using namespace MMgc;

    StringBuffer::StringOutputStream::StringOutputStream(MMgc::GC *gc)
    {
        m_gc = gc;
        m_buffer = NULL;
        m_length = 0;
    }

    StringBuffer::StringOutputStream::~StringOutputStream()
    {
        if (m_buffer != NULL) {
            m_gc->Free(m_buffer);
            m_buffer = 0;
        }
        m_gc = 0;
        m_length = 0;
    }

    void StringBuffer::StringOutputStream::write(const char* utf8)
    {
        writeN(utf8, String::Length(utf8));
    }

    void StringBuffer::StringOutputStream::ensureCapacity(size_t count)
    {
        if (m_buffer == NULL || m_length+count >= (size_t)GC::Size(m_buffer))
        {
            size_t newCapacity = (m_length+count+1)*2;
            char* newBuffer = (char*) m_gc->Alloc(newCapacity);
            if (m_buffer != NULL) {
                VMPI_memcpy(newBuffer, m_buffer, m_length);
                m_gc->Free(m_buffer);
            }
            if (m_gc->IsPointerToGCPage(this))
                WB(m_gc, m_gc->FindBeginningFast(this), &m_buffer, newBuffer);
            else
                m_buffer = newBuffer;
        }
    }

    void StringBuffer::StringOutputStream::writeN(const char* buffer, size_t count)
    {
        ensureCapacity(m_length + count);
        VMPI_memcpy(m_buffer+m_length, buffer, count);
        m_length += (int)count;
        m_buffer[m_length] = 0;
    }

    void StringBuffer::writeN(const char* buffer, size_t count)
    {
        m_stream.writeN(buffer, count);
    }
}
