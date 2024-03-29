/* -*- Mode: Java; c-basic-offset: 4; indent-tabs-mode: nil; tab-width: 4 -*- */
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
 * Portions created by the Initial Developer are Copyright (C) 2009
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
package abcasm;

import java.util.HashMap;

class Trait 
{
    int kind_byte;
    
    HashMap<String,Object> attrs = new HashMap<String,Object>();
    
    Trait(int kind)
    {
        this.kind_byte = kind;
    }

    void addAttr(String key, Object value)
    {
        if ( attrs.containsKey(key) )
        {
            throw new IllegalArgumentException("Trait attribute " + key + " cannot be specified twice." );
        }

        attrs.put(key, value);
    }

    void validate()
    {
        verifyContains("name", null);
    }

    public int getKind()
    {
        return kind_byte & 0x0F;
    }
    
    public void setAttr(String attr_name, Object attr_value)
    {
        attrs.put(attr_name, attr_value);
    }

    public boolean hasAttr(String attr_name)
    {
        return attrs.containsKey(attr_name);
    }

    public Object getAttr(String attr_name)
    {
        verifyContains(attr_name, null);
        return attrs.get(attr_name);
    }

    public int getIntAttr(String attr_name)
    {
        verifyContains(attr_name, Integer.class);
        return (Integer)attrs.get(attr_name);
    }
    
    public Name getNameAttr(String attr_name)
    {
        verifyContains(attr_name, Name.class);
        return (Name)attrs.get(attr_name);
    }
    
    void verifyContains(String attr_name, Class clazz)
    {
        if ( !attrs.containsKey(attr_name) )
            throw new IllegalArgumentException("Required attribute " + attr_name + " not found.");
        if ( ! ( null == clazz ||  attrs.get(attr_name).getClass().equals(clazz) ) )
        {
            throw new IllegalArgumentException("Attribute " + attr_name + " must be type " + clazz.getSimpleName() );
        }
    }
}
