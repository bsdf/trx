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

#ifndef __avmplus_AvmCore__
#define __avmplus_AvmCore__

namespace avmplus
{

#define OBJECT_TYPE     (core->traits.object_itraits)
#define CLASS_TYPE      (core->traits.class_itraits)
#define FUNCTION_TYPE   (core->traits.function_itraits)
#define ARRAY_TYPE      (core->traits.array_itraits)
#define STRING_TYPE     (core->traits.string_itraits)
#define NUMBER_TYPE     (core->traits.number_itraits)
#define INT_TYPE        (core->traits.int_itraits)
#define UINT_TYPE       (core->traits.uint_itraits)
#define BOOLEAN_TYPE    (core->traits.boolean_itraits)
#define VOID_TYPE       (core->traits.void_itraits)
#define NULL_TYPE       (core->traits.null_itraits)
#define NAMESPACE_TYPE  (core->traits.namespace_itraits)
#define VECTORINT_TYPE  (core->traits.vectorint_itraits)
#define VECTORUINT_TYPE (core->traits.vectoruint_itraits)
#define VECTORDOUBLE_TYPE       (core->traits.vectordouble_itraits)
#define VECTOROBJ_TYPE  (core->traits.vectorobj_itraits)

const int kBufferPadding = 16;

    enum VB_Bits {
        // Output control bits for verbose mode
         VB_builtins     = 1<<31 // display output for builtins (default is to ignore any builtins)
        ,VB_parse        = 1<<30 // abc parsing information
        ,VB_verify       = 1<<29 // verification information
        ,VB_interp       = 1<<28 // interpreter information
        ,VB_jit          = 1<<27 // jit information
        ,VB_traits       = 1<<26 // traits creation information
        ,VB_execpolicy   = 1<<25  // execution policy information
        ,VB_raw          = 1<<24  // native code (via nanojit) is displayed in raw (i.e unbuffered bottom-up) fashion.
        ,VB_lircfg       = 1<<23  // produce a control-flow graph of any resultant lir (gml format) in extended basic blocks
        ,VB_lircfg_bb    = 1<<22  // same as above, except each vertex is a basic block
        ,VB_lircfg_ins   = 1<<21  // same as above, except each vertex is an instruction
        // @warning make sure these don't collide with LC_xxx bits
    };

    typedef UnmanagedPointerList<MethodRecognizer*> MethodRecognizerSet;

#ifdef VMCFG_NANOJIT
    // ABC->LIR JIT options.  This doesn't belong here, but
    // it cannot go in CodegenLIR.h to avoid a dependency cycle.
    struct JitConfig
    {
        // Generate inline fastpaths, introducing new control flow.
        // Distinct from non-speculative inlining and specialization.
        bool opt_inline;

        // Initialize with default options.
        JitConfig() : opt_inline(true) {}
    };
#endif

    struct Config
    {
    #ifdef VMCFG_NANOJIT
        // options for nanojit
        nanojit::Config njconfig;
        // options for CodegenLIR
        JitConfig jitconfig;

    #endif
        /**
         * The verbose flag may be set to display each bytecode
         * instruction as it is executed, along with a snapshot of
         * the state of the stack and scope chain.
         * @see VB_Bits for individual settings of these flags
         */
        uint32_t verbose_vb;

        /**
         * A list of MethodRecognizers that is used to determine
         * whether verbose output is to occur on a given method
         * or not.  If this list is empty then verbosity is enabled
         * for all methods (assuming its on).
         */
        const char* verboseOnlyString;

        // if true, record original names of methods at runtime.
        // if false, don't (Function.toString will return things like "Function-21")
        bool methodNames;

        // give "Vector.<*>" instead of "Vector$object", etc
        bool oldVectorMethodNames;

        enum Runmode runmode;
        uint32_t osr_threshold;
        const char* compilePolicyRules; // JIT compilation override

        /**
         * If this switch is set, executing code will check the
         * "interrupted" flag to see whether an interrupt needs
         * to be handled.
         */
        bool interrupts;

        bool verifyall;
        bool verifyonly;
        bool verifyquiet;

        bool jitordie;      // Always JIT, and if the JIT fails then abort
    };

    class MethodFrame;

    // Use this for picking apart and putting together double values from constituent
    // words.  "words.msw" is the most significant word, containing sign bit, exponent,
    // and most significant bits of mantissa.  "words.lsw" is the least significant
    // word, containing the least significant bits of mantissa.  "bits" are two words
    // you can use if you don't care about the word order or if you're constructing
    // a double from a machine representation to which you only have a byte pointer.
    // All words are native-endianness.
    //
    // (Note the structure tag 'words' is required, the Solaris C++ compiler does not
    // support unnamed structures as of May, 2010.

    union double_overlay
    {
        double_overlay() {}
        double_overlay(double d) { value=d; }
        double_overlay(uint64_t v) { bits64=v; }

        double value;
#if defined VMCFG_BIG_ENDIAN || defined VMCFG_DOUBLE_MSW_FIRST
        struct { uint32_t msw, lsw; } words;
#else
        struct { uint32_t lsw, msw; } words;
#endif
        uint32_t bits32[2];
        uint64_t bits64;
    };

    // Regular expression compilation cache.
    //
    // The cache is used by AvmCore and is a cache for compiled regular expressions.
    // The cache significantly reduces the cost of using literal regular expressions
    // in loops (rather than manually hoisting them out of the loop), and it does so
    // at low complexity.
    //
    // See Bugzilla 573504 for more information.

    // A single entry in the regex cache.  Sits inside a GCRoot.

    class RegexCacheEntry
    {
        friend class RegexCache;

    private:
        RegexCacheEntry()
            : timestamp(0)
            , hits(0)
            , optionFlags(0)
            , global(false)
            , hasNamedGroups(false)
        {
        }

        void clear()
        {
            pattern = NULL;
            options = NULL;
            regex = NULL;
            timestamp = 0;
            hits = 0;
        }

    public:
        bool match(String* pattern, String* options)
        {
            return pattern == this->pattern && options == this->options;
        }

        MMgc::GCRoot::GCMember<String>         pattern;
        MMgc::GCRoot::GCMember<String>         options;
        MMgc::GCRoot::GCMember<CompiledRegExp> regex;

        uint64_t                 timestamp;
        uint32_t                 hits;
        int                      optionFlags;
        bool                     global;
        bool                     hasNamedGroups;
    };

    // Cache replacement is LRU based on a simple timestamping mechanism.  There's also
    // a mechanism to disable the cache if the number of entries that are used just once
    // significantly outweigh the uses of the entries that are used more than once,
    // that mechanism cuts the overhead of cache maintenance for programs that don't
    // benefit from the cache.

    class RegexCache
    {
        friend class AvmCore;

    public:
        /**
         * Search for a match in the regex cache, set found to 'true' if there was
         * a match, otherwise evict a member, set found to 'false'.  In all cases
         * return a structure that can be updated.
         */
        RegexCacheEntry& findCachedRegex(bool& found, String* source, String* options);

        /**
         * Search for a match in the regex cache, return 'true' if it's there or
         * 'false' if not.
         */
        bool testCachedRegex(String* source, String* options);

        /**
         * Return true iff the cache has been disabled.  Note the cache is re-enabled
         * following every GC.
         */
        bool disabled() { return m_disabled; }

    private:
        RegexCache();

        /**
         * Clear the cache, reenable it if it was disabled, reset utilization data.
         */
        void clear();

    private:
        // These parameters are not subject to a lot of experimental validation, in
        // particular, the parameters for cache enabling/disabling are really just
        // gut feelings.  More experimental validation would be good but we don't
        // have a lot of content that uses regular expressions.

        static const size_t cacheSize = 4;                  // Gut feeling: 4 is about right
        static const uint64_t wastedWorkMultiplier = 10;    // Gut feeling: utilization should be above 10%
        static const uint64_t warmupTicks = 1000;           // Number of ticks before we start worrying about hit rate

        RegexCacheEntry m_entries[cacheSize];   // Cache storage
        uint64_t        m_timestamp;            // Timestamp value for LRU
        uint64_t        m_wasted;               // Utility of wasted cache entries (sum of usage counts == 1)
        uint64_t        m_useful;               // Utility of useful cache entries (sum of usage counts > 1)
        bool            m_disabled;             // true iff the cache has been (temporarily) disabled
    };

    /**
     * Container for bug compatibility flags.
     *
     * NOTE the allocation code in AvmCore::createBugCompatibility assumes this structure
     * is pointer-free and allocates it as pointer-free memory (bugzilla 596529).
     * Subclassers should consider doing the same.
     */
    class BugCompatibility : public MMgc::LeafObject
    {
    public:
        enum Version
        {
            kSWF9,              // SWF9  (Flash Player 9.x)
            kSWF10,             // SWF10 (Flash Player 10.0 & 10.1)
            kSWF11,             // SWF11 (Flash Player 10.2 Spicy)
            kSWF12,             // SWF12 (Flash Player TBD Wasabi)
            kSWF13,             // SWF13 (Flash Player TBD Serrano)
            kSWF14,             // SWF14 (Flash Player TBD Anza)
            kSWF15,             // SWF15 (Flash Player TBD Brannan)

            VersionCount,

            kLatest = VersionCount - 1 // alias for "most recent"

        };

        /** These are the external names for swf versions - all integers.
         *
         * The names must correspond to the Version enum above.  The value will
         * be '9' for kSWF9, and so on.
         */
        static uint32_t const kNames[BugCompatibility::VersionCount];

        /** Compatibility bug fixes in the AVM - an overview.
         *
         * The AVM provides bug-compatible behavior to specific released versions
         * in both AS3 and C++ code.  The compatibility problem is orthogonal to
         * the API versioning problem (where names may be introduced globally or
         * in classes and interfaces but are visible only to certain versions).
         * However, groups of bug fixes appear in specific named verions of the AVM,
         * and since the API versioning mechansim already deals with named versions
         * the API mechanism can be used to control bug-compatibility at a high
         * level.  When the host code requests that a certain API version be enabled,
         * the bug-compatibility flags for that version are turned on, enabling
         * bug fixes appropriate for that version.
         *
         * In the C++ level bug-compatibility is handled by having one flag for each
         * bug for which we provide a compatibility switch.  C++ code tests
         * these flags, it never checks for a specific version name.
         *
         * The flags are made available to AS3 code through an global function,
         * 'bugzilla(n:int)', which is in an internal namespace (part of the
         * 'VM_INTERNAL' API).  It returns true if the flag for the numbered bug is
         * set, otherwise false.
         *
         * The flags always name a bug in Bugzilla, which means that bugs named here
         * can /only/ be about the one thing that is fixed.
         *
         * A short description is usually good to have by each of these, eg the subject
         * line from the bug report.  If the subject line is not very good then please
         * fix it first.
         *
         * Keep them in numeric order, please.
         *
         * DO NOT SET THESE FLAGS EXPLICITLY!  ALWAYS CALL THE CONSTRUCTOR!
         */

        // Entities are not escaped when appending String content to XML
        //
        // This and the "root" variant are the one and only special case we support,
        // due to existing usage we need to maintain. The rules are:
        //
        // -- if bugzilla444630 is set, we always escape the entities
        // -- if bugzilla444630 is clr,
        //      then we use AvmCore::bugzilla444630 to determine whether to escape
        //
        // The reason for this hackery is that existing Flash9/10 content assumes that
        // the root SWF defines the behavior, and all subordinate SWFs will follow it,
        // regardless of their version.
        //
        // This is some ugly hackery, but a necessary evil for legacy content. Please don't
        // ever use this antipattern for new bugs.
        unsigned bugzilla444630:1;

        unsigned bugzilla456852a:1;     // Different runtime error for invalid constructor call when -Ojit set
        unsigned bugzilla456852b:1;     // Different runtime error for negative vector index when -Ojit set
        unsigned bugzilla504525:1;      // Vector.concat processes arguments in reverse order
        unsigned bugzilla513018:1;      // parseFloat accepts illegal number syntax
        unsigned bugzilla513039:1;      // Number.toFixed(0) returns incorrect numbers, rounding issues
        unsigned bugzilla524122:1;      // Incorrect optimization for integers in numeric sort
        unsigned bugzilla526662:1;      // XMLParser stops at NUL char
        unsigned bugzilla532454:1;      // Sort incorrectly converts result of user-supplied comparison function to integer
        unsigned bugzilla539094:1;      // OSR (On-Stack replacement of interpreted methods by JIT-ted methods)
        unsigned bugzilla551587:1;      // MathClass:_min() does not correctly handle -0
        unsigned bugzilla558863:1;      // in operator on bytearray throws exception for non-natural number
        unsigned bugzilla574600:1;      // Vector.<>.AS3::map returns no value
        unsigned bugzilla585791:1;      // String.localeCompare with a null String object returns 0
        unsigned bugzilla598683:1;      // Bad XML with unterminated node with namespace not throwing correctly
        unsigned bugzilla609416:1;      // encodeURIComponent and decodeURIComponent give wrong output when input contains surrogate pairs
        unsigned bugzilla654807:1;      // sealed subclasses of Array act sort-of-dynamic
        unsigned bugzilla661330:1;      // Array.length behavior doesn't follow ECMA262 near 2^32-1
        unsigned bugzilla678952:1;      // Operations on Vector.<C> do not make use of or provide information about C

    protected:
        friend class AvmCore;

        // You should never construct a BugCompatibility directly;
        // instead, call AvmCore::createBugCompatibility, so that embedders
        // can create concrete subclasses if they so desire.
        explicit BugCompatibility(Version v);
    };

    // Internal to AvmCore, do not use for other purposes
    class GC_CPP_EXACT(LivePoolNode, MMgc::GCRoot)
    {
        GC_DATA_BEGIN(LivePoolNode)
    public:
        LivePoolNode* next;
        MMgc::GCWeakRef* GC_POINTER(pool);
        GC_DATA_END(LivePoolNode)

        LivePoolNode(MMgc::GC* gc) : GCRoot(gc, MMgc::kExact) {}
    };

    /**
     * The main class of the AVM+ virtual machine.  This is the
     * main entry point to the VM for running ActionScript code.
     *
     * ----- NOTE!! NOTE!! NOTE!! NOTE!! --------------------------
     *
     * AvmCore internal types and data fields are grouped at the beginning of the class
     * definition in order to give exact tracing a credible shot at staying correct.
     * PLEASE add data fields only in the correct group.
     *
     * ----- NOTE!! NOTE!! NOTE!! NOTE!! --------------------------
     */
    class AvmCore : public MMgc::GCRoot
    {
        friend class CodegenLIR;
        friend class EnterCodeContext;
        friend class EnterMethodEnv;
        friend class ExceptionFrame;
        friend class MethodFrame;
        friend class Traits;

        ////////////////////////////////////////////////////////////////////
        // BEGIN private data definitions

    private:
        enum {
            MIN_API_MARK = 0xE294, // that's "0xE000 + 660", 660 being an odd legacy wart
            MAX_API_MARK = 0xF8FF
        };

        // END private data definitions
        ////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////
        // BEGIN public data definitions

    public:
        enum InterruptReason
        {
            // normal state.  must be 0 to allow efficient code for interrupt checks
            NotInterrupted = 0,

            // script is running too long
            ScriptTimeout = 1,

            // host-defined external interrupt, other than a script timeout.
            ExternalInterrupt = 2
        };

        // avoid multiple inheritance issues
        class GCInterface : MMgc::GCCallback
        {
        public:
            GCInterface(MMgc::GC * _gc) : MMgc::GCCallback(_gc), core(NULL) {}
            void SetCore(AvmCore* _core) { this->core = _core; }
            void presweep() { if(core) core->presweep(); }
            void postsweep() { if(core) core->postsweep(); }
            void log(const char *str) { if(core) core->console << str; }
            void oom(MMgc::MemoryStatus status) { if(core) core->oom(status); }
        private:
            AvmCore *core;
        };

        class ICodeContextCreator
        {
        public:
            virtual ~ICodeContextCreator() {}
            virtual CodeContext* create(DomainEnv* domainEnv, const BugCompatibility* bugCompatibility) = 0;
        };

        struct CacheSizes
        {
            enum { DEFAULT_BINDINGS = 32, DEFAULT_METADATA = 1, DEFAULT_METHODS = 32 };

            uint16_t bindings;
            uint16_t metadata;
            uint16_t methods;

            inline CacheSizes() : bindings(DEFAULT_BINDINGS), metadata(DEFAULT_METADATA), methods(DEFAULT_METHODS) {}
        };

        // END public data definitions
        ////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////
        // BEGIN untraced private fields

    private:
#ifdef DEBUGGER
        Sampler*  _sampler;   // Sampler is a GCRoot
#endif

        /**
         * Stack limit set by host and checked by executing AS3 code.
         * If the stack pointer goes below this value, handleStackOverflow is invoked,
         * which in turn calls the host-provided virtual stackOverflow() handler.
         */
        uintptr_t minstack;

        /**
         * the host-provided stack limit.  we never change this, but
         * it's used to save/restore minstack when minstack is moved
         * to trigger interrupt handling.
         */
        uintptr_t stack_limit;

        /**
         * If this field is not NotInterrupted, the host has requested that the currently
         * executing AS3 code stop and invoke handleInterrupt.  The field
         * is checked directly by executing code.
         *
         * Set to ScriptTimeout for a timeout interrupt,
         * ExternalInterrupt for an external (i.e., signal handler) interrupt.
         */
        InterruptReason interrupted;

        /**
         * points to the topmost AS3 frame that's executing and provides
         * the full AS3 callstack.  Every AS3 method prolog/epilog updates
         * this pointer.  Exception catch handlers update this as well.
         */
        MethodFrame*        currentMethodFrame;

        // note, allocated using mmfx_new, *not* gc memory
        LivePoolNode* livePools;

        static const int k_atomDoesNotNeedCoerce_Masks[8];

        /** size of interned String table */
        int stringCount;

        /** number of deleted entries in our String table */
        int deletedCount;

        /** size of interned Namespace table */
        int nsCount;

        int numStrings;
        int numNamespaces;

        // a running count of MethodInfos used to generate unique id's for the PoolObjects
        uint32_t currentMethodInfoCount;

        // API versioning state
        ApiVersionSeries const  m_activeApiVersionSeries;
        uint32_t const          m_activeApiVersionSeriesMask;

#ifdef VMCFG_LOOKUP_CACHE
        // Saturating counter.
        uint32_t lookup_cache_timestamp;
#endif
#ifdef VMCFG_NANOJIT
        // when set, we flush all binding caches at the end of the next gc sweep.
        bool m_flushBindingCachesNextSweep;
#endif

        // END untraced private fields
        ////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////
        // BEGIN untraced public fields
    public:
        /**
         * Default values for the config parameters.  These need to be visible, because
         * the shell's command line parsing needs to know what they are.  Presumably
         * other host environments might find them useful too.
         *
         * These are not conditionally included because the resulting code is a mess
         * at no benefit.
         */
        static const uint32_t verbose_default;
        static const bool methodNames_default;
        static const bool oldVectorMethodNames_default;
        static const bool verifyall_default;
        static const bool verifyonly_default;
        static const bool verifyquiet_default;
        static const Runmode runmode_default;
        static const uint32_t osr_threshold_default;
        static const bool interrupts_default;
        static const bool jitordie_default;

#ifdef AVMPLUS_VERBOSE
        // default set of flags to enable for "verbose" with no specific qualifiers
        static const uint32_t DEFAULT_VERBOSE_ON;
#endif

        /**
         * The console object.  Text to be displayed to the developer
         * or end-user can be directed to console, much like the cout
         * object in C++ iostreams.
         *
         * The console object is a wrapper around the console output
         * stream specified by the setConsoleStream method.
         * Programs embedding AVM+ will typically implement
         * the OutputStream interface and pass it in via
         * setConsoleStream.
         */
        PrintWriter console;

        /**
         * The GC used by this AVM instance
         */
        MMgc::GC * const gc;

#ifdef _DEBUG
        // Only the thread used to create the AvmCore is allowed to modify currentMethodFrame (and thus, use EnterCodeContext).
        // We don't enforce this in Release builds, but check for it and assert in Debug builds.
        vmpi_thread_t       codeContextThread;
#endif

        // Please see BugCompatibility for an explanation of this field.
        bool bugzilla444630;

#ifdef DEBUGGER
        int                 langID;
        bool                passAllExceptionsToDebugger;
#endif

        Config config;

        // execution manager, responsible for all invocation
        ExecMgr* exec;

#ifdef AVMPLUS_WITH_JNI
        Java* java;     /* java vm control */
#endif

        /** The call stack of currently executing code. */
        CallStackNode *callStack;

        /** env of the highest catch handler on the call stack, or NULL */
        ExceptionFrame *exceptionFrame;

        GCInterface gcInterface;

        // END untraced public fields
        ////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////
        // BEGIN traced private fields

    private:
#ifdef DEBUGGER
        Debugger*       _debugger;
        Profiler*       _profiler;
#endif
        QCache*         m_tbCache;
        QCache*         m_tmCache;
        QCache*         m_msCache;
        DomainMgr*      m_domainMgr;

        // hash set containing intern'ed strings
        GCMember<String> * strings;

        // hash set containing namespaces
        GCMember<Namespace> * namespaces;

        // API versioning state
        HeapHashtable*          m_versionedURIs;
#ifdef _DEBUG
        HeapHashtable*          m_unversionedURIs;
#endif

        GCMember<UnscannedTraitsArray> _emptySupertypeList; // empty supertype list shared by many Traits

        // END traced private fields
        ////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////
        // BEGIN traced public fields

    public:
        RegexCache      m_regexCache;   // Substructure with traceable data

        /**
         * This method will be invoked when the first exception
         * frame is set up.  This will be a good point to set
         * minstack by calling setStackLimit().
         */
        virtual void setStackBase();

        /** Internal table of strings for boolean type ("true", "false") */
        GCMember<String> booleanStrings[2];

        /** Container object for traits of built-in classes */
        BuiltinTraits traits;

        /** PoolObject for built-in classes */
        PoolObject* builtinPool;

        /** Domain for built-in classes */
        Domain* builtinDomain;

        /** BugCompatibility used to initialize the built-in classes */
        const BugCompatibility* builtinBugCompatibility;

        /**
         * The default namespace, "public"
         */
        GCMember<Namespace> publicNamespace;

        /**
         * The unnamed public namespaces
         */
        NamespaceSet* publicNamespaces;

        /**
         * @name interned constants
         * Constants used frequently in the VM; these are typically
         * identifiers that are part of the core language semantics
         * like "prototype" and "constructor".  These are interned
         * up front and held in AvmCore for easy reference.
         */
        /*@{*/

        GCMember<String> kconstructor;
        GCMember<String> kEmptyString;
        GCMember<String> ktrue;
        GCMember<String> kfalse;
        GCMember<String> kundefined;
        GCMember<String> knull;
        GCMember<String> ktoString;
        GCMember<String> ktoLocaleString;
        GCMember<String> kvalueOf;
        GCMember<String> kvalue;
        GCMember<String> klength;
        GCMember<String> kobject;
        GCMember<String> kxml;
        GCMember<String> kfunction;
        GCMember<String> kboolean;
        GCMember<String> kvoid;
        GCMember<String> knumber;
        GCMember<String> kstring;
        GCMember<String> kuri;
        GCMember<String> kprefix;
        GCMember<String> kglobal;
        GCMember<String> kcallee;
        GCMember<String> kNeedsDxns;
        GCMember<String> kVersion;
        GCMember<String> kanonymousFunc;
        // From here at least the spelling mirrors exactly the spelling in the string constant
        GCMember<String> klittleEndian;
        GCMember<String> kbigEndian;
        GCMember<String> kparent;
        GCMember<String> kattribute;
        GCMember<String> kcomment;
        GCMember<String> kprocessing_instruction;
        GCMember<String> kelement;
        GCMember<String> ktext;
        GCMember<String> kattributeAdded;
        GCMember<String> kattributeRemoved;
        GCMember<String> kattributeChanged;
        GCMember<String> knodeAdded;
        GCMember<String> knodeRemoved;
        GCMember<String> knodeChanged;
        GCMember<String> knamespaceAdded;
        GCMember<String> knamespaceRemoved;
        GCMember<String> knamespaceSet;
        GCMember<String> knameSet;
        GCMember<String> ktextSet;
        GCMember<String> klocalName;
        GCMember<String> kindex;
        GCMember<String> kinput;
        GCMember<String> kemptyCtor;

        GCMember<String> kAsterisk;     // '*'
        GCMember<String> kColon;        // ':'
        GCMember<String> kUnderscore;   // '_'
        GCMember<String> kXML1998NS;    // http://www.w3.org/XML/1998/namespace
        GCMember<String> kzero;         // '0'  - 'kZero' conflicts with MMgc
        GCMember<String> kClassS;       // 'Class$'
        GCMember<String> kVectorNumber; // 'Vector.<Number>'
        GCMember<String> kVectorint;    // 'Vector.<int>'
        GCMember<String> kVectoruint;   // 'Vector.<uint>'
        GCMember<String> kVectorAny;    // 'Vector.<*>'

        Atom kNaN;

        GCMember<String> cachedChars[128];
        /*@}*/

        Exception *exceptionAddr;

        /** The XML entities table, used by E4X */
        HeapHashtable* xmlEntities;

        // END traced public fields
        ////////////////////////////////////////////////////////////////////

        ////////////////////////////////////////////////////////////////////
        // BEGIN methods (private/public intermixed)

    public:
        // return the BugCompatibility that is associated with the current
        // CodeContext. If there is no current CodeContext, return the
        // BugCompatibility associated with the VM's builtins.
        // This call will never return NULL.
        const BugCompatibility* currentBugCompatibility() const;

        // To be called post-PoolObjects' population to assign it a unique id amongst pools
        void assignPoolId(PoolObject* pool);

#ifdef DEBUGGER
    public:
        Debugger* debugger() const;
        Profiler* profiler() const;
        Sampler* get_sampler() const;
        void sampleCheck();

    protected:
        virtual Debugger* createDebugger(int tracelevel);
        virtual Profiler* createProfiler();
        virtual Sampler* createSampler();
#endif

    public:
        void addLivePool(PoolObject* pool);

    public:
        /**
         * inlineable interrupt check; used by interpreter in situations
         * where we must do more work before calling handleInterrupt, like
         * in the body of OP_absjump.
         */
        bool interruptCheck(bool interruptable);

    public:
        QCache* tbCache();
        QCache* tmCache();
        QCache* msCache();

        // safe to call at any time, but calling tosses existing caches, thus has a perf hit --
        // don't call cavalierly
        void setCacheSizes(const CacheSizes& cs);

    public:
        /**
         * Redirects the standard output of the VM to the specified
         * output stream.  Output from print() statements and
         * error messages will be sent here.
         * @param stream output stream to use for console output
         */
        void setConsoleStream(GCOutputStream *stream);
        void setConsoleStream(NonGCOutputStream *stream);

        /**
         * GCCallback functions
         */
        virtual void presweep();
        virtual void postsweep();
        virtual void oom(MMgc::MemoryStatus status);

    public:
        DomainMgr* domainMgr() const;

#ifdef VMCFG_NANOJIT // accessors
#if defined AVMPLUS_IA32 || defined AVMPLUS_AMD64
        bool use_sse2() const;
#endif
#endif

#ifdef AVMPLUS_VERBOSE
        bool isVerbose(uint32_t b, MethodInfo* info=NULL);
        static uint32_t parseVerboseFlags(const char* arg, char*& badFlag);

        /**
         * A list of MethodRecognizers that is used to determine
         * whether verbose output is to occur on a given method
         * or not.  If this list is empty then verbosity is enabled
         * for all methods (assuming its on).
         */
        MethodRecognizerSet _verboseRestrictedTo;

        virtual const char* identifyDomain(Domain* domain);
#endif

        /**
         * Check for stack overflow and call handler if it happens.
         * This is the canonical stack overflow check called from Interpreter.
         * An inlined version of this code is also generated by CodegenLIR::prologue().
         */
        void stackCheck(MethodEnv* env);

        /**
         * Like the previous method but for a Toplevel* argument.
         */
        void stackCheck(Toplevel* env);

        /** set the stack limit that will be checked by executing AS3 code. */
        void setStackLimit(uintptr_t p);

        /** called by executing code when stack overflow is detected (sp < minstack) */
        static void FASTCALL handleStackOverflowMethodEnv(MethodEnv* env);

        /** called by executing code when stack overflow is detected (sp < minstack) */
        static void FASTCALL handleStackOverflowToplevel(Toplevel* env);

        /**
         * Called by certain functions in PCRE to check for overflow.  The state is
         * kept in thread-local storage and is set up by call-ins to PCRE.
         */
        static void FASTCALL checkPCREStackOverflow();

        /**
         * Set the state for call-ins to PCRE.
         */
        static void setPCREContext(Toplevel* env);

        /**
         * Calls destroy on PCREContext to release the thread local variable key
         * back to the system. This is only used on platforms (e.g. Android) that do not
         * call the destructor of the static variable when the library is unloaded
         * (on Android this is an OS bug in Android 2.2, and probably earlier).
         */
        static void releasePCREContext();

    private:
        ScriptEnv* initOneScript(Toplevel* toplevel, AbcEnv* abcEnv, Traits* scriptTraits);
        ScriptEnv* initAllScripts(Toplevel* toplevel, AbcEnv* abcEnv);
        Atom callScriptEnvEntryPoint(ScriptEnv* main);

    public:
        /**
         * Execute an ABC file that has been parsed into a
         * PoolObject.
         * @param pool PoolObject containing the ABC file to
         *             execute
         * @param toplevel the Toplevel object to execute against.
         * @param codeContext CodeContext to use for execution (implicitly specifies DomainEnv)
         * @throws Exception If an error occurs, an Exception object will
         *         be thrown using the AVM+ exceptions mechanism.
         *         Calls to handleActionBlock should be bracketed
         *         in TRY/CATCH.
         */
        Atom handleActionPool(PoolObject* pool,
                                   Toplevel* toplevel,
                                   CodeContext *codeContext);

        template<class CLASSMANIFEST>
        CLASSMANIFEST* prepareBuiltinActionPool(PoolObject* pool,
                                     Toplevel* toplevel,
                                     CodeContext* codeContext);

        /**
         * Parse the ABC block starting at offset start in code.
         * @param code buffer holding the ABC block to parse
         * @param start zero-indexed offset, in bytes, into the
         *              buffer where the code begins
         * @param toplevel the Toplevel object to execute against,
         *                 or NULL if a Toplevel should be
         *                 created.
         * @param domain FIXME
         * @param ninit FIXME
         * @param api The api version of the code being parsed. It must
         *            coorespond to one of the versions in api-versions.h
         * @throws Exception If an error occurs, an Exception object will
         *         be thrown using the AVM+ exceptions mechanism.
         *         Calls to handleActionBlock should be bracketed
         *         in TRY/CATCH.
         */
        PoolObject* parseActionBlock(ScriptBuffer code,
                                     int start,
                                     Toplevel* toplevel,
                                     Domain* domain,
                                     const NativeInitializer* ninit,
                                     ApiVersion apiVersion);

        // This is equivalent to a call to parseActionBlock,
        // followed by a call to the now-defunct prepareActionPool.
        // Basically, it does everything handleActionBlock does
        // *except* that it doesn't actually call the main entry
        // point, but rather, returns it to the caller. (Needed
        // by Flash in QueueAbcBuffer.)
        ScriptEnv* prepareActionBlock(ScriptBuffer code,
                                     Toplevel* toplevel,
                                     Domain* domain,
                                     CodeContext* codeContext,
                                     ApiVersion apiVersion);

        /**
         * Execute the ABC block starting at offset start in code.
         * @param code buffer holding the ABC block to execute
         * @param start zero-indexed offset, in bytes, into the
         *              buffer where the code begins
         * @param toplevel the Toplevel object to execute against.
         * @param ninit FIXME
         * @param codeContext CodeContext to use for execution (implicitly specifies DomainEnv)
         * @param api The api version of the code being parsed. It must
         *            coorespond to one of the versions in api-versions.h
         * @throws Exception If an error occurs, an Exception object will
         *         be thrown using the AVM+ exceptions mechanism.
         *         Calls to handleActionBlock should be bracketed
         *         in TRY/CATCH.
         */
        Atom handleActionBlock(ScriptBuffer code,
                                    int start,
                                    Toplevel* toplevel,
                                    const NativeInitializer* ninit,
                                    CodeContext *codeContext,
                                    ApiVersion apiVersion);

#ifdef VMCFG_AOT
        /**
         * Parse and execute the ABC blocks linked into the runtime by the AOT
         * compiler.
         * @param toplevel the Toplevel object to execute against.
         * @param codeContext CodeContext to use for execution (implicitly specifies DomainEnv)
         * @throws Exception If an error occurs, an Exception object will
         *         be thrown using the AVM+ exceptions mechanism.
         *         Calls to handleAOT should be bracketed
         *         in TRY/CATCH.
         */
        void handleAOT(Toplevel* toplevel, CodeContext* codeContext);
#endif

#ifdef VMCFG_EVAL
        /**
         * Compile the source code held in 'code' and then execute it
         * as for handleActionBlock() above.
         *
         * @param code The code to be compiled and executed.  The string must be
         *             NUL-terminated and the NUL is not considered part of the
         *             input.  If 'code' is not in UTF-16 format it will be converted
         *             to UTF-16 format, so it is highly advisable that the caller
         *             has created an UTF-16 string.
         * @param filename The name of the file originating the code, or
         *                 NULL if the source code does not originate from a file.
         *                 If not NULL then ActionScript's 'include' directive will
         *                 be allowed in the program and files will be loaded
         *                 relative to 'filename'.
         * @param toplevel the Toplevel object to execute against.
         * @param ninit FIXME
         * @param codeContext CodeContext to use for execution (implicitly specifies DomainEnv)
         * @param api The api version of the code being parsed. It must
         *            coorespond to one of the versions in api-versions.h
         * @throws Exception If an error occurs, an Exception object will
         *         be thrown using the AVM+ exceptions mechanism.
         *         Calls to handleActionBlock should be bracketed
         *         in TRY/CATCH.
         */
        Atom handleActionSource(String* code,
                                String* filename,
                                Toplevel* toplevel,
                                const NativeInitializer* ninit,
                                CodeContext *codeContext,
                                ApiVersion apiVersion);

        /**
         * Obtain input from a file to handle ActionScript's 'include' directive.
         *
         * This method invoked by the run-time compiler if the script uses 'include'
         * and the use of 'include' is allowed because the script originated from
         * a file; see 'handleActionSource()' above.
         *
         * 'referencingFilename' should be taken into
         * account by this method if 'filename' is not an absolute file name.
         *
         * @param referencingFilename The name of the file from which the script
         *                            containing the 'include' directive was loaded
         * @param filename  The filename in the 'include' directive.
         * @return  A string representing the contents of the file named by 'filename'.
         *          The string must NUL-terminated and the NUL is not considered part
         *          of the input.  If the returned string is not in UTF-16 format then
         *          it will be converted to UTF-16 format, so it is highly advisable
         *          that the method has created an UTF-16 string.  If the file cannot
         *          be opened or read then the return value should be NULL, an
         *          exception should not be thrown.
         */
        virtual String* readFileForEval(String* referencingFilename, String* filename) = 0;
#endif // VMCFG_EVAL

        /**
         * When ABC is unloaded, we might not be able to free all associated
         * data structures immediately without some help: the VM caches some
         * information (eg TraitsBindings) that might prevent a timely collection.
         * Calling this is a hint from the client that cached information should
         * be flushed more aggressively to encourage faster collection of the discarded
         * ABC. Note that it is safe to call this "too often", but doing so will hurt
         * performance; you need call it only once per ABC unloading event.
         */
        void handleAbcUnloaded();

        /** Implementation of OP_equals */
        Atom equals(Atom lhs, Atom rhs);

        /**
         * this is the abstract relational comparison algorithm according to ECMA 262 11.8.5
         * @param lhs
         * @param rhs
         * @return trueAtom, falseAtom, or undefinedAtom
         */
        static Atom FASTCALL compare(Atom lhs, Atom rhs);

        /** Implementation of OP_strictequals */
        static Atom FASTCALL stricteq(Atom lhs, Atom rhs);

        /**
         * Helper method; returns true if the atom is a tagged ScriptObject
         * pointer.  The actual type of the object is indicated by
         * ScriptObject->vtable->traits.
         */
        static bool isObject(Atom atom);

        static bool isPointer(Atom atom);

        static bool isNamespace(Atom atom);

        static BindingKind bindingKind(Binding b);

        static bool isMethodBinding(Binding b);

        static bool isAccessorBinding(Binding b);

        static bool hasSetterBinding(Binding b);

        static bool hasGetterBinding(Binding b);

        static int bindingToGetterId(Binding b);

        static int bindingToSetterId(Binding b);

        static int bindingToMethodId(Binding b);

        static int bindingToSlotId(Binding b);

        /** true if b is a var or a const */
        static int isSlotBinding(Binding b);

        static Binding makeSlotBinding(uintptr_t id, BindingKind kind);

        static Binding makeMGSBinding(uintptr_t id, BindingKind kind);

        static Binding makeGetSetBinding(Binding b);

        /** true only if b is a var */
        static int isVarBinding(Binding b);

        /** true only if b is a const */
        static int isConstBinding(Binding b);

        /** Helper method; returns true if atom is an Function */
        bool isFunction(Atom atom);

        /** Helper method; returns true if atom's type is double */
        static bool isDouble(Atom atom);

        // removed, because it was being (erroneously) used to ask
        // "will you fit in int32_t?", which was never right for 64-bit.
        // instead, use atomIsIntptr(), which asks "will you fit in intptr?"
        // static bool isInteger(Atom atom);

        /** Helper method; returns true if atom's type is Number */
        static bool isNumber(Atom atom);

        /** Helper method; returns true if atom's type is boolean */
        static bool isBoolean(Atom atom);

        /** Helper method; returns true if atom's type is null */
        static bool isNull(Atom atom);

        /** Helper method; returns true if atom's type is undefined */
        static bool isUndefined(Atom atom);

        static bool isNullOrUndefined(Atom atom);

#ifdef AVMPLUS_VERBOSE
        /** Disassembles an opcode and places the text in str. */
        void formatOpcode(PrintWriter& out, const uint8_t *pc, const uint8_t *code_end, AbcOpcode opcode, ptrdiff_t off, PoolObject* pool);
#ifdef VMCFG_WORDCODE
        void formatOpcode(PrintWriter& out, const uintptr_t *pc, const uintptr_t *code_end, WordOpcode opcode, ptrdiff_t off, PoolObject* pool);
        void formatBits(PrintWriter& buffer, uint32_t bits);
#endif
        static void formatMultiname(PrintWriter& out, uint32_t index, PoolObject* pool);
#endif
        /** Constructor */
        AvmCore(MMgc::GC *gc, ApiVersionSeries apiVersionSeries);

        /** Destructor */
        ~AvmCore();

       /**
        * Convenience method to access the AvmCore associated
        * the current thread. Performance may vary across platforms
        * since the implementation depends on thread local storage.
        */
       static AvmCore* getActiveCore();

       /**
         * Parses builtin.abc into a PoolObject, to be executed
         * later for each new Toplevel
         */
#ifdef DEBUGGER
        void initBuiltinPool(int tracelevel);
#else
        void initBuiltinPool();
#endif

        /**
         * Initializes the specified Toplevel object by running
         * builtin.abc. codeContextCreator will be used to create the
         * CodeContext (so clients that subclass CodeContext can create one
         * as appropriate).
         */
        Toplevel* initToplevel(ICodeContextCreator& codeContextCreator);

        /**
         * If a client needs to subclass Toplevel, it should override this
         * method to create the proper subclass. (AvmCore will never create a
         * a Toplevel directly, it will use this as a factory method.)
         */
        virtual Toplevel* createToplevel(AbcEnv* abcEnv);

        /**
         * If a client need to subclass BugCompatibility, it should override this
         * method to create the proper subclass. (AvmCore will never create a
         * a BugCompatibility directly, it will use this as a factory method.)
         */
        virtual const BugCompatibility* createBugCompatibility(BugCompatibility::Version v);

        /**
         * Support for API versioning
         */

        /**
         * Add to the list of URIs that must be versioned for namespaces.
         *
         * @param uris       NULL-terminated list of URIs that will need versioning
         */
        void addVersionedURIs(char const* const* uris);

        /**
         * Get the AVM wide default API version.
         */
        virtual ApiVersion getDefaultAPI() = 0;

        /**
         * Find the ApiVersion from the first non-builtin caller on the AS3 call stack.
         * If the AS3 call stack is empty, return getDefaultAPI().
         */
        ApiVersion getApiVersionFromCallStack();

        /**
         * If the given Namespace has a version that allows it to be used by
         * the ApiVersion active on the call stack, return true; otherwise return false.
         * This is equivalent to ns->getApiVersion() <= getApiVersionFromCallStack(),
         * but it's easy for client code to get the comparison reversed, so this is
         * useful syntactic sugar.
         */
        bool isNamespaceVisibleToApiVersionFromCallStack(Namespace* ns);

        /**
         * Find the current public by walking the call stack
         */
        Namespacep findPublicNamespace();

        /**
         * Get the public namespace associated with the given pool's version.
         *
         * @param pool The caller's pool object.
         */
        Namespacep getPublicNamespace(PoolObject* pool);

        /**
         * Get a public namespace that will match with any/all ApiVersions.
         */
        Namespacep getAnyPublicNamespace();

        /**
         * Get the public namespace associated with a specific ApiVersion.
         */
        Namespacep getPublicNamespace(ApiVersion apiVersion);

        /**
         * If the ApiVersion is valid for the current ApiVersionSeries, return true.
         * Otherwise, return false.
         */
        bool isValidApiVersion(ApiVersion apiVersion);

        /**
         * If the ApiVersion is not valid for the current ApiVersionSeries, return the
         * ApiVersion that is the smallest valid superset. (This might be VM_INTERNAL)
         */
        ApiVersion getValidApiVersion(ApiVersion apiVersion);

        /**
         * toUInt32 is the ToUInt32 algorithm from
         * ECMA-262 section 9.6, used in many of the
         * native core objects
         */
        static uint32_t toUInt32(Atom atom);

        /**
         * toInteger is the ToInteger algorithm from
         * ECMA-262 section 9.4, used in many of the
         * native core objects
         */
        static double toInteger(Atom atom);

        /**
         * Converts the passed atom to a 32-bit signed integer.
         * If the atom is already an integer, it is simply
         * decoded.  Otherwise, it is coerced to the int type
         * and returned.  This is ToInt32() from E3 section 9.5
         */
        static int32_t integer(Atom atom);

        // convert atom to integer when we know it is already a legal signed-32 bit int value
        static int32_t integer_i(Atom a);

        // convert atom to integer when we know it is already a legal unsigned-32 bit int value
        static uint32_t integer_u(Atom a);

        static int integer_d(double d);
        Atom doubleToAtom(double n);

#if defined (AVMPLUS_IA32) || defined(AVMPLUS_AMD64)
        static int integer_d_sse2(double d);
        Atom doubleToAtom_sse2(double n);
#endif

        static int doubleToInt32(double d);

    public:
        static double number_d(Atom a);

        /**
         * intAtom is similar to the integer method, but returns
         * an atom instead of a C++ int.
         */
        Atom intAtom(Atom atom);

        Atom uintAtom(Atom atom);

        /**
         * Converts the passed atom to a C++ bool.
         * If the atom is already an E4 boolean, it is simply
         * decoded.  Otherwise, it is coerced to the boolean type
         * and returned.
         * [ed] 12/28/04 use int because bool is sometimes byte-wide.
         */
        static int boolean(Atom atom);

        /**
         * Returns the passed atom's string representation.
         * If the passed atom is not a string, it is coerced
         * to type string using the ECMAScript coercion rules.
         */
        Stringp string(Atom atom);

        Stringp coerce_s(Atom atom);

        /**
         * Returns true if the passed atom is of string type.
         */
        static bool isString(Atom atom);

        static bool isName(Atom atom);

        /**
         * an interned atom is canonicalized in this way:
         * boolean -> "true" or "false"
         * number -> intern'ed string value
         * string -> intern'ed string value
         * object -> intern'ed result of toString()
         *
         * this way, interned atoms are suitable to be used as map keys and can
         * be compared using ==.
         * @param atom
         * @return
         */
        Stringp intern(Atom atom);

        Namespacep internNamespace(Namespacep ns);

        /** Helper function; reads a signed 24-bit integer from pc */
        static int readS24(const uint8_t *pc);

        /**
         * Returns the size of the instruction + all it's operands.  For OP_lookupswitch the size will not include
         * the size for the case targets.
         */
        static int calculateInstructionWidth(const uint8_t* p);

        /**
         * Read in some operands for the instruction located at *pc.
         * Returns the size of the instruction, but will not read in all the case targets for
         * an OP_lookupswitch, since there will be a variable number of them.
         */
        static void readOperands(const uint8_t* &pc, unsigned int& imm32, int& imm24, unsigned int& imm32b, int& imm8 );

        /**
         * Helper function; reads an unsigned 32-bit integer from pc
         * See AbcParser::readS32 for more explanation of the variable length
         * encoding scheme.
         */
        static uint32_t readU32(const uint8_t *&p)
        {
            // @todo -- needs to be moved into AvmCore-inlines.h,
            // but first we must determine whether it should be inline, REALLY_INLINE, etc...

            unsigned int result = p[0];
            if (!(result & 0x00000080))
            {
                p++;
                return result;
            }
            result = (result & 0x0000007f) | p[1]<<7;
            if (!(result & 0x00004000))
            {
                p += 2;
                return result;
            }
            result = (result & 0x00003fff) | p[2]<<14;
            if (!(result & 0x00200000))
            {
                p += 3;
                return result;
            }
            result = (result & 0x001fffff) | p[3]<<21;
            if (!(result & 0x10000000))
            {
                p += 4;
                return result;
            }
            result = (result & 0x0fffffff) | p[4]<<28;
            p += 5;
            return result;
        }

        // when you need to skip over a U32 and don't care about the result,
        // this is slightly faster.
        static void skipU32(const uint8_t*& p, int count = 1)
        {
            // @todo -- needs to be moved into AvmCore-inlines.h,
            // but first we must determine whether it should be inline, REALLY_INLINE, etc...

            while (count-- > 0)
            {
                if (!(p[0] & 0x80)) { p += 1; continue; }
                if (!(p[1] & 0x80)) { p += 2; continue; }
                if (!(p[2] & 0x80)) { p += 3; continue; }
                if (!(p[3] & 0x80)) { p += 4; continue; }
                //if (!(*p[4] & 0x80)) { p += 5; continue; }    // test should be unnecessary
                AvmAssert(!(p[4] & 0x80));
                p += 5;
            }
        }

        /** Helper function; reads an unsigned 16-bit integer from pc */
        static int32_t readU16(const uint8_t *pc);

    public:
        // note, return of true means we definitely DO NOT need a coerce,
        // but return of false still means we *might* need to (ie, negating the result of this function
        // isn't "needscoerce")
        static bool atomDoesNotNeedCoerce(Atom a, BuiltinType bt);

        /**
         * this is the implementation of the actionscript "is" operator.  similar to java's
         * instanceof.  returns true/false according to AS rules.  in particular, it will return
         * false if value==null.
         */
        static bool istype(Atom atom, Traits* itraits);

        /**
         * this is the implementation of the actionscript "is" operator.  similar to java's
         * instanceof.  returns true/false according to AS rules.  in particular, it will return
         * false if value==null.
         */
        static Atom istypeAtom(Atom atom, Traits* itraits);

        /**
         * implements ECMA as operator.  Returns the same value, or null.
         */
        static Atom astype(Atom atom, Traits* expected);

        /**
         * implementation of OP_increg, decreg, increment, decrement which correspond to
         * ++ and -- operators in AS.
         */
        void increment_d(Atom *atom, int delta);

        /**
         * implementation of OP_increg, decreg, increment, decrement which correspond to
         * ++ and -- operators in AS.
         */
        void increment_i(Atom *atom, int delta);

        /**
         * ES3's internal ToPrimitive() function
         */
        static Atom primitive(Atom atom);

        /** OP_toboolean; ES3 ToBoolean() */
        static Atom booleanAtom(Atom atom);

        /** OP_tonumber; ES3 ToNumber */
        Atom numberAtom(Atom atom);

        /**
         * ES3's internal ToNumber() function for internal use
         */
        static double number(Atom atom);

        /**
         * The interrupt method is called from executing code
         * when the interrupted flag is set.  interrupt()
         * MUST NOT RETURN; the caller expects a thrown exception.
         */
        virtual void interrupt(Toplevel *env, InterruptReason) = 0;

        /**
         * called by the host to raise the AS3 interrupt exception.
         * if AS3 code is executing, then soon after this call,
         * interrupt() will be invoked by the currently executing function.
         */
        void raiseInterrupt(InterruptReason reason);

        // return true if there is a pending interrupt of the specific InterruptReason.
        bool interruptCheckReason(InterruptReason r) const;

        /**
         * called by AS3 code when the interrupt is detected.  Must
         * not return!
         */
        static void handleInterruptMethodEnv(MethodEnv*);

        /**
         * called by AS3 code when the interrupt is detected.  Must
         * not return!
         */
        static void handleInterruptToplevel(Toplevel*);

        /**
         * This is called when the stack overflows
         * (when the machine stack pointer is about to go below
         *  minstack)
         */
        virtual void stackOverflow(Toplevel *env) = 0;

        /**
         * Throws an exception.  Constructs an Exception object
         * and calls throwException.
         */
        void throwAtom(Atom atom);

        /**
         * The AVM+ equivalent of the C++ "throw" statement.
         * Throws an exception, transferring control to the
         * nearest CATCH block.
         */
        void throwException(Exception *exception);

        /**
         * throwErrorV is a convenience function for throwing
         * an exception with a formatted error message,
         */
        void throwErrorV(ClassClosure *type, int errorID, Stringp arg1=0, Stringp arg2=0, Stringp arg3=0);

        /**
         * formatErrorMessageV is a convenience function for
         * assembling an error message with varags.
         */
        String* formatErrorMessageV( int errorID, Stringp arg1=0, Stringp arg2=0, Stringp arg3=0);

        /**
         * Convenience methods for converting various objects into value
         * strings used for error message output.
         */
        String* toErrorString(int d);
        String* toErrorString(MethodInfo* m);
        String* toErrorString(const Multiname& n);
        String* toErrorString(const Multiname* n);
        String* toErrorString(Namespacep ns);
        String* toErrorString(const Traits* t);
        String* toErrorString(const char* s);
        String* toErrorString(const wchar* s);
        String* atomToErrorString(Atom a);

        /**
         * getErrorMessage returns the format string for an
         * errorID.  Override to provide format strings for
         * additional implementation-dependent error strings.
         */
        virtual String* getErrorMessage(int errorID);

#ifdef DEBUGGER
        /**
         * willExceptionBeCaught walks all the way up the
         * ActionScript stack to see if there is any "catch"
         * statement which is going to catch the specified
         * exception.
         */
        bool willExceptionBeCaught(Exception* exception);

#ifndef VMCFG_DEBUGGER_STUB
        /**
         * findErrorMessage searches an error messages table.
         * Only available in debugger builds.
         */
        String* findErrorMessage(int errorID,
                                 int* mapTable,
                                 const char** errorTable,
                                 int numErrors);
#endif

        /**
         * Determines the language id of the given platform
         */
        virtual int determineLanguage();


        /**
         * Creates a StackTrace from the current executing call stack
         */
        StackTrace* newStackTrace();

#ifdef _DEBUG
        void dumpStackTrace();
#endif

#endif /* DEBUGGER */

        CodeContext* codeContext() const;
        Namespace* dxns() const;

        /**
         * implements OP_dxns.  internedUri is expected to be from a constant
         * pool and therefore already interned.  Creates a namespace and
         * saves it as the given method frame's default xml namespace.
         */
        void setDxns(MethodFrame*, String* internedUri);

        /**
         * implements OP_dxnslate.  uri is any value, which we must convert
         * to an interned-string, then create an namespace from it,
         * then save the value in the current method frame as the current
         * default xml namespace.
         */
        void setDxnsLate(MethodFrame*, Atom uri);

        /**
         * Searches the exception handler table of info for
         * a try/catch block that contains the instruction at pc
         * and matches the type of exception.
         * @param info      the method to search the exception handler
         *                  table of
         * @param pc        the program counter at the point where
         *                  the exception occurred; either a pointer into
         *                  bytecode or into native compiled code
         * @param exception the exception object that was thrown;
         *                  used to match the type.
         * @return ExceptionHandler object for the exception
         *         handler that matches, or re-throws the passed-in
         *         exception if no handler is found.
         */
        ExceptionHandler* findExceptionHandler(MethodInfo *info,
                                               intptr_t pc,
                                               Exception *exception);

        /**
         * Just like findExceptionHandler(), except that this function
         * returns NULL if it can't find an exception handler, whereas
         * findExceptionHandler() re-throws the passed-in exception if
         * it can't find a handler.  Also, sets *ordinal to the index of
         * the handler in the exception handler table, if a handler is
         * found.
         */
        ExceptionHandler* findExceptionHandlerNoRethrow(MethodInfo *info,
                                                        intptr_t pc,
                                                        Exception *exception,
                                                        int32_t  *ordinal);

        /**
         * Returns true if the passed atom is an XML object,
         * as defined in the E4X Specification.
         */
        static bool isXML(Atom atm);

        /**
         * Returns true if the passed atom is a XMLList object,
         * as defined in the E4X Specification.
         */
        static bool isXMLList(Atom atm);

        static bool isXMLorXMLList(Atom atm);

        /* Returns tru if the atom is a Date object */
        static bool isDate(Atom atm);

        // From http://www.w3.org/TR/2004/REC-xml-20040204/#NT-Name
        static bool isLetter(wchar c);
        static bool isDigit(wchar c);
        static bool isCombiningChar(wchar c);
        static bool isExtender(wchar c);

        Stringp ToXMLString (Atom a);
        Stringp EscapeElementValue (const Stringp s, bool removeLeadingTrailingWhitespace);
        Stringp EscapeAttributeValue (Atom v);

        /**
         * Converts an Atom to a E4XNode if its traits match.
         * Otherwise, null is returned. (An exception is NOT thrown)
         */
        static E4XNode* atomToXML(Atom atm);

        /**
         * Converts an Atom to a XMLObject if its traits match.
         * Otherwise, null is returned. (An exception is NOT thrown)
         */
        static XMLObject* atomToXMLObject(Atom atm);

        /**
         * Converts an Atom to a XMLListObject if its traits match.
         * Otherwise, null is returned. (An exception is NOT thrown)
         */
        static XMLListObject* atomToXMLList(Atom atm);

        /**
         * Returns true if the passed atom is a QName object,
         * as defined in the E4X Specification.
         */
        static bool isQName(Atom atm);

        /**
         * Returns true if the passed atom is a Dictionary object,
         * as defined in the E4X Specification.
         */
        static bool isDictionary(Atom atm);

        static bool isDictionaryLookup(Atom key, Atom obj);

        /**
         * Returns true if the passed atom is a valid XML name,
         * as defined in the E4X Specification.
         */
        bool isXMLName(Atom arg);

        /**
         * Converts an Atom to a QNameObject if its traits match.
         * Otherwise, null is returned. (An exception is NOT thrown)
         */
        static QNameObject* atomToQName(Atom atm);

        /** Implementation of OP_typeof */
        Stringp _typeof (Atom arg);

    private:
        static bool isBuiltinType(Atom atm, BuiltinType bt);
        static bool isBuiltinTypeMask(Atom atm, int btmask);


    public:

        static Namespacep atomToNamespace(Atom atom);

        static double atomToDouble(Atom atom);

        /**
         * Convert an Atom of kStringType to a Stringp
         * @param atom atom to convert.  Note that the Atom
         *             must be of kStringType
         */
        static Stringp atomToString(Atom atom);

        // Avoid adding validation checks here and returning NULL.  If this
        // is returning a bad value, the higher level function should be fixed
        // or AbcParser/Verifier should be enhanced to catch this case.
        static ScriptObject* atomToScriptObject(const Atom atom);

        // Helper function, allows generic objects to work with InlineHashtable,
        // uses double type which is the only non-RC pointer tag.
        // The key here is that these methods round-trip any pointer value to the
        // same pointer value, there is no casting that might adjust the pointer.
        static Atom genericObjectToAtom(const void* obj);
        static const void* atomToGenericObject(Atom a);
        static bool isGenericObject(Atom a);

    private:
        /** search the string intern table */
        int findStringLatin1(const char* s, int len);
        int findStringUTF16(const wchar* s, int len);
        int findString(Stringp s);
        int findStringWithIndex(uint32_t value);

        int numStringsCheckLoadBalance();

        /** search the namespace intern table */
        int findNamespace(Namespacep ns, bool canRehash = true);

        Namespacep gotNamespace(uintptr_t uriAndType, ApiVersion apiVersion);

    public:
        /**
         * If the uri is versioned, set apiVersion to the version and strip the uri string, and return true.
         * If the uri is unversioned, leave uri and apiVersion unchanged and return false.
         */
        bool getBaseURIAndApiVersion(Stringp& uri, ApiVersion& apiVersion);

        Namespacep getVersionedNamespace(Namespacep ns, ApiVersion apiVersion);

        bool isVersionedURI(Stringp uri);

        /**
         * Convert a version name to a version enum
         */
        static bool parseApiVersion(const char* p, ApiVersion& apiVersion, ApiVersionSeries& apiVersionSeries);

        // String creation. If len is omitted, zero-termination is assumed.
        Stringp newStringLatin1(const char* str, int len = -1);
        /**
         * Create a string out of UTF-8 data. WARNING: If strict is true,
         * the return value may be NULL. Callers must check for this condition
         * to avoid crashes due to malformed UTF-8 character sequences!
         * If strict is false, malformed UTF-8 sequences are copied character
         * by character.
         */
        Stringp newStringUTF8(const char* str, int len = -1, bool strict = false);
        /**
         * Create a string out of UTF-16 data. WARNING: If strict is true,
         * the return value may be NULL. Callers must check for this condition
         * to avoid crashes due to malformed UTF-16 surrogate pairs!
         * If strict is false, malformed UTF-16 surrogate pairs are copied as
         * two characters.
         */
        Stringp newStringUTF16(const wchar* str, int len = -1, bool strict = false);

        // decodes UTF16LE or UTF16BE. Same restriction as newStringUTF16().
        Stringp newStringEndianUTF16(bool littleEndian, const wchar* str, int len = -1, bool strict = false);

        // like newStringLatin1, but the string constant is assumed to remain valid
        // for the life of the AvmCore. Generally, should only be used for literal
        // strings, eg newConstantStringLatin1("foo")
        Stringp newConstantStringLatin1(const char* str);


        // variants on the newStringXXX() calls that also intern the string.
        Stringp internStringLatin1(const char* s, int len = -1);
        Stringp internStringUTF8(const char* s, int len = -1, bool constant = false, bool strict = true);
        Stringp internStringUTF16(const wchar* s, int len = -1);

        // like internStringLatin1, but the string constant is assumed to remain valid
        // for the life of the AvmCore. Generally, should only be used for literal
        // strings, eg internStringLatin1("foo")
        Stringp internConstantStringLatin1(const char* s);

        /**
         * intern the given string atom which has already been allocated
         * @param atom
         * @return
         */
        Stringp internString(Stringp s);
        Stringp internString(Atom atom);
        Stringp internInt(int n);
        Stringp internDouble(double d);
        Stringp internUint32(uint32_t ui);

        /* true return value implies *result set to interned string for ui;
         * false return value implies ui is not in intern table. */
        bool isInternedUint(uint32_t ui, Stringp *result);

        Stringp internSubstring(Stringp s, int32_t start, int32_t end);

        Stringp internIntSlowPath(int value);

#ifdef DEBUGGER
        /**
         * intern without allocating memory, returns NULL if its not already interned
         */
        Stringp findInternedString(const char *s, int len);
#endif

        static bool getIndexFromAtom(Atom a, uint32_t *result);

        ScriptBufferImpl* newScriptBuffer(size_t size);
        VTable* newVTable(Traits* traits, VTable* base, Toplevel* toplevel);

        RegExpObject* newRegExp(RegExpClass* regExpClass,
                                Stringp pattern,
                                Stringp options);

        ScriptObject* newObject(VTable* ivtable, ScriptObject *delegate);

        FrameState* newFrameState(int frameSize, int scopeBase, int stackBase);
        Namespacep newNamespace(Atom prefix, Atom uri, Namespace::NamespaceType type = Namespace::NS_Public);
        Namespacep newNamespace(Atom uri, Namespace::NamespaceType type = Namespace::NS_Public);
        Namespacep newNamespace(Stringp uri, Namespace::NamespaceType type, ApiVersion apiVersion);
        // Like newNamespace(), but versioned such that it will match all versions of the namespace
        // with the same uri/type. This is useful for glue code that (e.g.) needs to look up specific
        // Bindings in the builtins. (Analogous to "getAnyPublicNamespace" for arbitrary names.)
        Namespacep newAnyNamespace(Stringp uri, Namespace::NamespaceType type);
        Namespacep newPublicNamespace(Stringp uri);

        Stringp uintToString(uint32_t i);
        Stringp intToString(int i);
        Stringp doubleToString(double d);
        Stringp concatStrings(Stringp s1, Stringp s2);

        Atom uintToAtom(uint32_t n);
        Atom intToAtom(int32_t n);

        Atom allocDouble(double n);

        void rehashStrings(int newlen);
        void rehashNamespaces(int newlen, bool canFail=false);
        void rehashNamespacesIfPossible(int newlen);

        // static version for smart pointers
        static void atomWriteBarrier(MMgc::GC *gc, const void *container, Atom *address, Atom atomNew);
        static void atomWriteBarrier_ctor(MMgc::GC *gc, const void *container, Atom *address, Atom atomNew);
        static void atomWriteBarrier_dtor(Atom *address);

        // fills the area with zero
        static void decrementAtomRegion(Atom *ar, int length);
        // fills the area with nullObjectAtom
        static void decrementAtomRegion_null(Atom *ar, int length);

#ifdef VMCFG_LOOKUP_CACHE
    public:
        uint32_t lookupCacheTimestamp() const;
        bool lookupCacheIsValid(uint32_t t) const;
        void invalidateLookupCache();
#endif

#ifdef VMCFG_NANOJIT
    public:
        void flushBindingCachesNextSweep();
#endif

        // END methods (private/public intermixed)
        ////////////////////////////////////////////////////////////////////
    };

    /*
        MethodFrame is a way of maintaining CodeContext and DXNS in a uniform way
        in both Interpreter and JIT modes. CodeContext is a poorly-documented
        structure that is exercised very little in current acceptance tests, but is
        used extensively for Flash and AIR. The theory of operation:
        -- Normally, the "active" CodeContext is that of the most-recently-called
            non-builtin MethodEnv on the call stack.
        -- native C++ code can override the current CodeContext by using EnterCodeContext(),
            which just pushes another MethodFrame onto the stack...
            it overrides the current CodeContext, but subsequent nested calls to non-builtin
            methods will in turn override this.
        -- The implementation is a bit convoluted, in the name of saving stack space.
            A single field can contain either a MethodEnv* (for a normal MethodFrame)
            or a CodeContext* (for one pushed by EnterCodeContext). This means that the top-of-stack
            may not have the current MethodEnv* handy, so walking down the stack is necessary
            to find it.
        -- Note that MethodFrame doesn't contain a pointer to AvmCore*; this is by design
            (as a stack-saving measure), as CodegenLIR doesn't need to save it (it can emit the proper constant value),
            and all other callers have ready access to one.
    */
    class MethodFrame
    {
    public:
        // deliberately no ctor or dtor here.

        // NOTE, the code in enter/exit is replicated in CodegenLIR.cpp;
        // if you make changes here, you may need to make changes there as well.
        void enter(AvmCore* core, MethodEnv* e);
        void enter(AvmCore* core, CodeContext* cc);
        void exit(AvmCore* core);
        CodeContext* cc() const;
        MethodEnv* env() const;

        // Search for a frame that has a default namespace, starting on the given frame.
        static Namespace* findDxns(const MethodFrame* start);

        void setDxns(Namespace* ns);

    public:
        MethodFrame*    next;

    private:
        friend class CodegenLIR;
        friend class OSR;
        enum {
            IS_EXPLICIT_CODECONTEXT = 0x1,
            DXNS_NOT_NULL = 0x2,
            FLAGS_MASK = 0x3
        };
        uintptr_t       envOrCodeContext;
        Namespace*      dxns; // NOTE: this struct is always stack-allocated (or via avmStackAlloc, which is just as good), so no GCMember needed
    };

// NOTE: this is a temporary fix until OOM handling hooks in to Avm exceptions generally;
// see https://bugzilla.mozilla.org/show_bug.cgi?id=611078
#ifdef DEBUGGER
    class AvmCoreAutoEnter : public MMgc::AbortUnwindObject
    {
    public:
        AvmCoreAutoEnter(AvmCore* core);
        ~AvmCoreAutoEnter();
        virtual void Unwind();
    private:
        MMgc::EnterFrame* m_ef;
        AvmCore* m_savedCore;
        CallStackNode* m_savedCallStack;
    };
    #define AVMCORE_ENTER(c) avmplus::AvmCoreAutoEnter __avmcore_auto_enter(c);
#else
    #define AVMCORE_ENTER(c)
#endif

}

#endif /* __avmplus_AvmCore__ */
