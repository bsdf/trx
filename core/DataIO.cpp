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
    //
    // DataIOBase
    //

    void DataIOBase::ThrowEOFError()
    {
        toplevel()->throwEOFError(kEOFError);
    }

    void DataIOBase::ThrowMemoryError()
    {
        toplevel()->throwMemoryError(kOutOfMemoryError);
    }

    void DataIOBase::ThrowRangeError()
    {
        toplevel()->throwRangeError(kParamRangeError);
    }

    //
    // DataInput
    //

    bool DataInput::ReadBoolean()
    {
        uint8_t value;
        Read(&value, 1);
        return value != 0;
    }

    uint8_t DataInput::ReadU8()
    {
        uint8_t value;
        Read(&value, 1);
        return value;
    }

    uint16_t DataInput::ReadU16()
    {
        union {
            uint8_t b[2];
            uint16_t u;
        };
        Read(b, 2);
        ConvertU16(u);
        return u;
    }

    uint32_t DataInput::ReadU32()
    {
        union {
            uint8_t b[4];
            uint32_t u;
        };
        Read(b, 4);
        ConvertU32(u);
        return u;
    }

    float DataInput::ReadFloat()
    {
        union {
            uint8_t b[4];
            uint32_t u;
            float f;
        };
        Read(b, 4);
        ConvertU32(u);
        return f;
    }

    double DataInput::ReadDouble()
    {
        union {
            uint8_t b[8];
            uint64_t u;
            double d;
        };
        Read(b, 8);
        ConvertD64(u);
        return d;
    }

    String* DataInput::ReadMultiByte(uint32_t length, String* charset)
    {
        CheckEOF(length);

        Toplevel* toplevel = this->toplevel();
        uint32_t codepage = toplevel->charsetToCodepage(charset);
        String* result = toplevel->readMultiByte(codepage, length, this);
        return result;
    }

    String* DataInput::ReadUTFBytes(uint32_t length)
    {
        CheckEOF(length);

        char *buffer = mmfx_new_array_opt( char, length+1, MMgc::kCanFail );
        if (!buffer) {
            ThrowMemoryError();
        }

        Read(buffer, length);
        buffer[length] = 0;

        // Since this is supposed to read UTF8 into a string, it really should ignore the UTF8 BOM that
        // might reasonably occur at the head of the data.
        char* utf8chars = buffer;
        if (length >= 3 && (unsigned char)buffer[0] == 0xEF && (unsigned char)buffer[1] == 0xBB && (unsigned char)buffer[2] == 0xBF)
        {
            utf8chars += 3;
        }

        String* out = toplevel()->core()->newStringUTF8(utf8chars);
        mmfx_delete_array( buffer );

        return out;
    }

    String* DataInput::ReadUTF()
    {
        return ReadUTFBytes(ReadU16());
    }

    void DataInput::ReadByteArray(ByteArray& buffer,
                                  uint32_t offset,
                                  uint32_t count)
    {
        uint32_t available = Available();

        if (count == 0) {
            count = available;
        }

        if (count > available) {
            ThrowEOFError();
        }

        // check for 32 bit overflow
        uint64_t total = (uint64_t)offset + (uint64_t)count;
        if (total != (uint64_t)(offset + count))
            ThrowRangeError();

        // Grow the buffer if necessary
        if (offset + count >= buffer.GetLength()) {
            buffer.SetLength(offset + count);
        }

        Read(buffer.GetWritableBuffer() + offset, count);
    }

    void DataInput::CheckEOF(uint32_t count)
    {
        if (Available() < count) {
            ThrowEOFError();
        }
    }

    Atom DataInput::ReadObject()
    {
        Toplevel* toplevel = this->toplevel();
        return toplevel->readObject(GetObjectEncoding(), this);
    }

    //
    // DataOutput
    //

    void DataOutput::WriteBoolean(bool value)
    {
        WriteU8(value ? 1 : 0);
    }

    void DataOutput::WriteU8(uint8_t value)
    {
        Write(&value, 1);
    }

    void DataOutput::WriteU16(uint16_t value)
    {
        union {
            uint8_t b[2];
            uint16_t u;
        };
        u = value;
        ConvertU16(u);
        Write(b, 2);
    }

    void DataOutput::WriteU32(uint32_t value)
    {
        union {
            uint8_t b[4];
            uint32_t u;
        };
        u = value;
        ConvertU32(u);
        Write(b, 4);
    }

    void DataOutput::WriteFloat(float value)
    {
        union {
            uint8_t b[4];
            uint32_t u;
            float f;
        };
        f = value;
        ConvertU32(u);
        Write(b, 4);
    }

    void DataOutput::WriteDouble(double value)
    {
        union {
            uint8_t b[8];
            uint64_t u;
            double d;
        };
        d = value;
        ConvertD64(u);
        Write(b, 8);
    }

    void DataOutput::WriteMultiByte(String* str, String* charset)
    {
        Toplevel* toplevel = this->toplevel();
        uint32_t codepage = toplevel->charsetToCodepage(charset);
        toplevel->writeMultiByte(codepage, str, this);
    }

    void DataOutput::WriteUTF(String *str)
    {
        StUTF8String utf8(str);
        uint32_t length = utf8.length();
        if (length > 65535) {
            ThrowRangeError();
        }
        WriteU16((uint16_t)length);
        Write(utf8.c_str(), length*sizeof(char));
    }

    void DataOutput::WriteUTFBytes(String *str)
    {
        StUTF8String utf8(str);
        int len = utf8.length();
        Write(utf8.c_str(), len*sizeof(char));
    }

    void DataOutput::WriteByteArray(ByteArray& buffer,
                                    uint32_t offset,
                                    uint32_t count)
    {
        if (offset > buffer.GetLength())
            offset = buffer.GetLength();

        if (count == 0) {
            count = buffer.GetLength()-offset;
        }

        if (count > buffer.GetLength()-offset) {
            ThrowRangeError();
        }

        if (count > 0) {
            Write(buffer.GetReadableBuffer()+offset, count);
        }
    }

    void DataOutput::WriteObject(Atom atom)
    {
        Toplevel* toplevel = this->toplevel();
        toplevel->writeObject(GetObjectEncoding(), this, atom);
    }
}


