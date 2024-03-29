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

#ifndef __GCLargeAlloc_inlines__
#define __GCLargeAlloc_inlines__

namespace MMgc
{
    /*static*/
    REALLY_INLINE GCLargeAlloc::LargeBlock* GCLargeAlloc::GetLargeBlock(const void *addr)
    {
        return (LargeBlock*)GetBlockHeader(addr);
    }

    /*static*/
    REALLY_INLINE GCLargeAlloc::LargeBlock* GCLargeAlloc::Next(LargeBlock* b)
    {
        return (LargeBlock*)b->next;
    }

#ifndef MMGC_FASTBITS
    REALLY_INLINE gcbits_t& GCLargeAlloc::GetGCBits(const void *realptr)
    {
        return GetLargeBlock(realptr)->flags[0];
    }
#endif

    /*static*/
    REALLY_INLINE bool GCLargeAlloc::IsLargeBlock(const void *item)
    {
        // The pointer should be 4K aligned plus 16 bytes
        // Mac inserts 16 bytes for new[] so make it more general
        return (((uintptr_t)item & GCHeap::kOffsetMask) == sizeof(LargeBlock));
    }

    /*static*/
    REALLY_INLINE bool GCLargeAlloc::IsProtectedAgainstFree(const void *item)
    {
        LargeBlock *block = GetLargeBlock(item);
        return (block->flags[0] & kQueued) != 0 || (block->flags[1] & kProtected) != 0;
    }

    /*static*/
    REALLY_INLINE void GCLargeAlloc::ProtectAgainstFree(const void *item)
    {
        LargeBlock *block = GetLargeBlock(item);
        block->flags[1] |= kProtected;
    }

    /*static*/
    REALLY_INLINE void GCLargeAlloc::UnprotectAgainstFree(const void *item)
    {
        LargeBlock *block = GetLargeBlock(item);
        block->flags[1] &= ~kProtected;
    }

    /*static*/
    REALLY_INLINE void* GCLargeAlloc::FindBeginning(const void *item)
    {
        LargeBlock *block = GetLargeBlock(item);
        GCAssertMsg(item >= block->GetObject(), "Can't call FindBeginning on something pointing to GC header");
        return block->GetObject();
    }

    REALLY_INLINE int GCLargeAlloc::LargeBlock::GetNumBlocks() const
    {
        return (size + sizeof(LargeBlock)) / GCHeap::kBlockSize;
    }

    REALLY_INLINE void* GCLargeAlloc::LargeBlock::GetObject() const
    {
        return (void*) (this+1);
    }

    REALLY_INLINE GCLargeAllocIterator::GCLargeAllocIterator(MMgc::GCLargeAlloc* alloc)
        : alloc(alloc)
        , block(alloc->m_blocks)
    {
    }

    REALLY_INLINE bool GCLargeAllocIterator::GetNextMarkedObject(void*& out_ptr)
    {
        while (block != NULL) {
            GCLargeAlloc::LargeBlock* b = block;
            block = GCLargeAlloc::Next(block);
            if ((b->flags[0] & kMark) != 0 && (b->containsPointers != 0)) {
                out_ptr = GetUserPointer(b->GetObject());
                return true;
            }
        }
        return false;
    }
}

#endif /* __GCLargeAlloc_inlines__ */
