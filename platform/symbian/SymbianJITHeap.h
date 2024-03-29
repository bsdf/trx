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
 * Portions created by the Initial Developer are Copyright (C) 1993-2006
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

#ifndef __SymbianJITHeap__
#define __SymbianJITHeap__

#include <e32std.h>

/*
* A simple "first pick" allocator.
* Uses a map where each bit points to a 4k chunk.
* It returns always the first free chunk in the map:
* the chunk that was previously freed or the next free chunk from where previous chunk was allocated from.
*/

class SymbianJITHeap
{
public:

    SymbianJITHeap();
    ~SymbianJITHeap();

    bool IsInitialized();
    TInt GetNumAllocs();

    void* Alloc();
    void Free(void* ptr);

private:

    enum
    {
        kMaxBitLookup = 256,
        kBitsInTUint = 32,
        kPageSize = 4096,
        kMaxHeapSize = kPageSize*16384, // 64Mb, should be enough for Symbian for now
        kHeapAdjustSize  = kPageSize*8, // it is likely to be cheaper to not adjust after each allocation
        kMaxMap = kMaxHeapSize / kPageSize / kBitsInTUint // kMaxMap is 512 when kMaxHeapSize is 64Mb
    };

    void SetupBitLookup();
    int GetEmptyBit(int arrayPos);
    void ClearBit(int mapPos);
    void SetBit(int mapPos);
    int GetEmptyChunkIndex();
    void SetEmptyChunkIndex(int index);

    TInt m_previousFree;
    TInt m_currentMaxMem;
    TUint m_map[kMaxMap];
    TUint8 m_bitLookup[kMaxBitLookup];
    RChunk m_chunk;
    TUint m_startAddr;
    TInt m_numAllocs;
};


#endif __SymbianJITHeap__
