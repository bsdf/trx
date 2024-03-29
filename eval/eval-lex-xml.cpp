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
 * Portions created by the Initial Developer are Copyright (C) 2008
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

#ifdef VMCFG_EVAL

#include "eval.h"

namespace avmplus
{
    namespace RTC
    {
        Token Lexer::xmlAtomImpl()
        {
            mark=idx;
            switch (idx[0]) {
                case 0:
                    compiler->syntaxError(lineno, SYNTAXERR_XML_EOI_IN_MARKUP);
                case '<':
                    switch (idx[1]) {
                        case '!':
                            if (idx[2] == '[' &&
                                idx[3] == 'C' &&
                                idx[4] == 'D' &&
                                idx[5] == 'A' &&
                                idx[6] == 'T' &&
                                idx[7] == 'A' &&
                                idx[8] == '[') {
                                idx += 9;
                                return xmlMarkup(T_XmlCDATA);
                            }
                            if (idx[2] == '-' && idx[3] == '-') {
                                idx += 4;
                                return xmlMarkup(T_XmlComment);
                            }
                            compiler->syntaxError(lineno, SYNTAXERR_XML_INVALID_LEFTBANG);
                            
                        case '?':
                            idx += 2;
                            return xmlMarkup(T_XmlProcessingInstruction);
                            
                        case '/':
                            idx += 2;
                            return T_XmlLeftAngleSlash;

                        default:
                            idx += 1;
                            return T_XmlLeftAngle;
                    }
                    
                case '/':
                    if (idx[1] == '>') {
                        idx += 2;
                        return T_XmlSlashRightAngle;
                    }
                    compiler->syntaxError(lineno, SYNTAXERR_XML_INVALID_SLASH);
                    
                case '>':
                    idx += 1;
                    return T_XmlRightAngle;
                    
                case '{':
                    idx += 1;
                    return T_XmlLeftBrace;
                    
                case '}':
                    idx += 1;
                    return T_XmlRightBrace;

                case '=':
                    idx += 1;
                    return T_XmlEquals;

                case ' ':
                case '\t':
                case '\r':
                case '\n':
                    return xmlWhitespace();
                    
                case '"':
                case '\'':
                    return xmlString();
                    
                default:
                    if (isXmlNameStart(idx[0]))
                        return xmlName();
                    else
                        return xmlText();
            }
        }

        // Capture everything from the starting through the ending punctuation.

        Token Lexer::xmlMarkup(Token token)
        {
            uint32_t l = lineno;
            switch (token) {
                case T_XmlComment:
                    mark = idx-4; // "<!--"
                    break;
                case T_XmlCDATA:
                    mark = idx-9; // "<![CDATA["
                    break;
                case T_XmlProcessingInstruction:
                    mark = idx-2; // "<?"
                    break;
                default:
                    AvmAssert(!"Inconsistent internal state");
            }

            while (idx < limit) {
                if (idx[0] == '-' || idx[0] == '?' || idx[0] == ']') {
                    switch (token) {
                        case T_XmlComment:
                            if (idx[0] == '-' && idx[1] == '-') {
                                // Done; we require > to follow but it's not part of the stop condition.
                                if (idx[2] != '>')
                                    compiler->syntaxError(lineno, SYNTAXERR_XML_ILLEGAL_CHARS);
                                idx += 3;
                                goto endloop;
                            }
                            break;
                        case T_XmlCDATA:
                            if (idx[0] == ']' && idx[1] == ']' && idx[2] == '>') {
                                // Done.
                                idx += 3;
                                goto endloop;
                            }
                            break;
                        case T_XmlProcessingInstruction:
                            if (idx[0] == '?' && idx[1] == '>') {
                                // Done.
                                idx += 2;
                                goto endloop;
                            }
                            break;
                    }
                }

                switch (idx[0]) {
                    case '\n':
                        lineno++;
                        break;
                    case '\r':
                        lineno++;
                        if (idx[1] == '\n')
                            idx++;
                        break;
                    default:
                        idx++;
                        break;
                }
            }
            
        endloop:
            if (idx == limit)
                compiler->syntaxError(l, SYNTAXERR_XML_UNTERMINATED);
            val.s = compiler->intern(mark, uint32_t(idx-mark));
            return token;
        }

        Token Lexer::xmlWhitespace()
        {
            mark = idx;
            while (idx < limit) {
                switch (*idx) {
                    case ' ':
                    case '\t':
                        break;
                    case '\r':
                        lineno++;
                        if (idx[1] == '\n')
                            idx++;
                        break;
                    case '\n':
                        lineno++;
                        break;
                    default:
                        goto end_loop;
                }
                idx++;
            }
        end_loop:
            val.s = compiler->intern(mark, uint32_t(idx-mark));
            return T_XmlWhitespace;
        }

        Token Lexer::xmlName()
        {
            AvmAssert( isXmlNameStart(*idx) );
            mark = idx;
            while (isXmlNameSubsequent(*idx))
                idx++;
            val.s = compiler->intern(mark, uint32_t(idx-mark));
            return T_XmlName;
        }
        
        // mark has been set at the beginning of the starting punctuation,
        // we wish to capture the ending punctuation as well.
        
        Token Lexer::xmlString()
        {
            wchar terminator = *idx;
            uint32_t l = lineno;

            idx++;
            while (idx < limit && *idx != terminator) {
                if (*idx == '\r') {
                    idx++;
                    if (*idx == '\n')
                        idx++;
                    lineno++;
                }
                else if (*idx == '\n') {
                    idx++;
                    lineno++;
                }
                else
                    idx++;
            }

            if (idx == limit)
                compiler->syntaxError(l, SYNTAXERR_XML_UNTERMINATED);

            idx++;
            val.s = compiler->intern(mark, uint32_t(idx-mark));
            return T_XmlString;
        }

        // FIXME: E4X says to stop only at "{" and "<".

        Token Lexer::xmlText()
        {
            mark = idx;
            while (idx < limit) {
                switch (*idx) {
                    case ' ':
                    case '\t':
                    case '\r':
                    case '\n':
                    case '{':
                    case '}':
                    case '<':
                    case '>':
                    case '/':
                    case '=':
                        goto end_loop;
                    default:
                        if (isXmlNameStart(*idx))
                            goto end_loop;
                }
                idx++;
            }
        end_loop:
            val.s = compiler->intern(mark, uint32_t(idx-mark));
            return T_XmlText;
        }

        bool Lexer::isXmlNameStart(wchar c)
        {
            return isUnicodeLetter(c) || c == ':' || c == '_';
        }
        
        bool Lexer::isXmlNameSubsequent(wchar c)
        {
            return isUnicodeLetter(c) || isUnicodeDigit(c) || c == '_' || c == ':' || c == '.' || c == '-';
        }
        
    }
}

#endif // VMCFG_EVAL
