/*
 From Brian Beckman's siodffi.zip distribution.
*/

#ifdef UNICODE
#error "This module must not be compiled for UNICODE"
#endif

#ifdef _UNICODE
#error "This module must not be compiled for UNICODE"
#endif


#include <windows.h>
#include <crtdbg.h>
#include "imagehlp.h"
#include "pea.h"
#include "stdio.h"
#include "siod.h"

/* #include "sioddata.h" */
extern "C" 
{
    _declspec (dllimport) extern LISP sym_t;
    _declspec (dllimport) extern LISP sym_errobj;
}


/* This is not the way to do this. Should be using the lisp symbol table
   for this kind of stuff -- GJC
#include "dcomImports.h"
*/

static HANDLE g_heap ;

BOOL   WINAPI   DllMain
(
    HANDLE hInst,
    ULONG  ul_reason_for_call,
    LPVOID lpReserved
)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH)
    {
        g_heap = HeapCreate (HEAP_GENERATE_EXCEPTIONS, 0x1000, 0x100000) ;
    }

    if (ul_reason_for_call == DLL_PROCESS_DETACH)
    {
        HeapDestroy(g_heap);
        g_heap = 0;
    }

    return TRUE;
}


int LoadLibraryXXX (PPEA ppea, char * szModuleFileName)
{
    ppea->Init (g_heap);
    __try
    {
        ppea->OpenModule (szModuleFileName);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
        return 0 ;
    }
    return ppea->FScatterFile () ;
}


#define STR(x) ((x)->storage_as.string.data)
#define SYM(x) ((x)->storage_as.symbol.pname)
#define DIM(x) ((x)->storage_as.string.dim)
#define LNG(x) ((long)(FLONM(x)))
#define DW(x)  ((DWORD)(FLONM(x)))

#define CB(x)   LNG(x)
#define PB(x)   ((BYTE*)DW(x))
#define PV(x)   ((PVOID)DW(x))
#define RGPV(x) ((PVOID*)DW(x))
#define SZ(x)   ((char*)DW(x))
#define FP(x)   ((FARPROC)DW(x))

inline FARPROC FuncPtrFromLisp (LISP x)
{
	switch (TYPE(x))
	{
	case tc_cptr:
		return (FARPROC)(DWORD)CPTR(x) ;

	case tc_flonum:
		return FP(x) ;

	default:
		err ("wta FuncPtr", x) ;
		return 0 ;
	}
}

#define STRINGDATA(x) (TYPE(x) == tc_string ? STR(x) : SYM(x))


LISP llast_win32_errmsg(DWORD status)
{
    DWORD len,msgcode;
    char buffer[256];
    msgcode = (status == 0) ? GetLastError() : status;

    len = FormatMessage
    (
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS | FORMAT_MESSAGE_MAX_WIDTH_MASK,
        0,
        msgcode,
        0, /* what language? */
        buffer,
        sizeof(buffer),
        NULL
    );

    if (len)
        return (strcons(len,buffer));
    else
        return (flocons(msgcode));
}

LISP
reverse(LISP l)
{
    LISP n, p;

    n = NIL;
    for (p = l; NNULLP(p); p = cdr(p))
        n = cons(car(p), n);
    return (n);
}

//================================================================================
// A callback written in scheme (some of this is stupid, like the first
// two instructions in the epilogue -- fix later).
//================================================================================

static const BYTE prologue [] = 
{	0x55,						  // push        ebp
	0x8B, 0xEC,					  // mov         ebp,esp
	0x83, 0xEC, 0x5C,    		  // sub         esp,5Ch   ; make space for locals
    0x53,                         // push        ebx
    0x56,                         // push        esi
    0x57,                         // push        edi
} ;
static const BYTE uptofirstcall [] = 
{	0x6A, 0x00,					  // push        0
	0x6A, 0x00,					  // push        0
	0x8B, 0x45, 0x08,             // mov         eax,dword ptr [input]
	0x89, 0x45, 0xF4,             // mov         dword ptr [ebp-0Ch],eax
	0xC7, 0x45, 0xF8, 0, 0, 0, 0, // mov         dword ptr [ebp-8],0
	0xDF, 0x6D, 0xF4,             // fild        qword ptr [ebp-0Ch]
	0x83, 0xEC, 0x08,             // sub         esp,8
	0xDD, 0x1C, 0x24,             // fstp        qword ptr [esp]
} ;
static const BYTE callpop [] =
{	0x83, 0xC4, 0x08              // add         esp,8
} ;
static const BYTE pusheax [] =
{	0x50						  // push        eax
} ;
static const BYTE outputTransform [] =
{	0x89, 0x45, 0xFC,             // mov         dword ptr [d],eax
	0x8B, 0x4D, 0xFC,             // mov         ecx,dword ptr [ebp-4]
	0xDD, 0x41, 0x08,             // fld         qword ptr [ecx+8]
    0x83, 0xEC, 0x08,             // sub         esp,8
    0xDD, 0x1C, 0x24,             // fstp        qword ptr [esp]
} ;
static const BYTE epilogue [] =
{
    0x5F,                   	  // pop         edi
    0x5E,                   	  // pop         esi
    0x5B,                   	  // pop         ebx
    0x8B, 0xE5,					  // mov         esp,ebp
	0x5D,						  // pop         ebp
	0xC2, 0x04, 0x00              // ret         4
} ;
static inline void PaintRGB (BYTE * & pb, BYTE * pbSrc, DWORD cb)
{   CopyMemory (pb, pbSrc, cb) ;
    pb += cb ;
}

static inline void PaintPDW (BYTE * & pb, DWORD * pdw)
{   BYTE * pb2 = ((BYTE*)(pdw)) ;
    *pb++ = *pb2++ ;
    *pb++ = *pb2++ ;
    *pb++ = *pb2++ ;
    *pb++ = *pb2++ ;
}

static inline void PaintLiteral (BYTE * & pb, DWORD literal)
{   PaintPDW (pb, &literal) ;
}

enum EInstructions
{
    xload = 0x8B, 
    store = 0x89, 
    add   = 0x83,
    jmp   = 0xEB,
    call  = 0xE8,
    cmp   = 0x3B,
    jge   = 0x7D,
    fild1 = 0xDF,
    fild2 = 0x6D,
    xpush = 0x6A,
    jne   = 0x75,
} ;

// The following do not apply to all instructions
enum ERegister
{
    eax   = 0x45, 
    ebx   = 0x49,
    ecx   = 0x4D, 
    edx   = 0x55,
} ;

static inline void EmitLocalVariableIndicatorByte (BYTE * & pb, DWORD negativeOffsetFromEBP)
{   /*
    ebp-00h  100 
    ebp-04h   FC
    ebp-08h   F8
    ebp-0Ch   F4
    ebp-10h   F0
    ebp-14h   EC
    ebp-18h   E8
    ebp-1Ch   E4
    ebp-20h   E0
    ebp-24h   DC
    */
    *pb++ = (0xFF & ((BYTE)(0x100-negativeOffsetFromEBP))) ;
}

// The following are all negative offsets from EBP
enum ELocals 
{
    ELarity   = 0x04, 
    ELargList = 0x08, 
    ELi       = 0x0C, 
    ELargAddr = 0x10,
    ELd       = 0x14,
    ELflTmp1  = 0x18,
    ELflTmp2  = 0x1C,
} ;

static inline void EmitStoreLocal (BYTE * & pb, ERegister eReg, DWORD negativeOffsetFromEBP)
{   *pb ++ = store ;
    *pb ++ = eReg ;
    EmitLocalVariableIndicatorByte (pb, negativeOffsetFromEBP) ;
}

static inline void EmitLoadLocal (BYTE * & pb, ERegister eReg, DWORD negativeOffsetFromEBP)
{   *pb ++ = xload ;
    *pb ++ = eReg ;
    EmitLocalVariableIndicatorByte (pb, negativeOffsetFromEBP) ;
}

static inline void EmitPushAir (BYTE * & pb, DWORD howMuch)
{   *pb++ = 0x83 ;
    *pb++ = 0xEC ;
    *pb++ = (BYTE)howMuch ;
}

static inline void EmitPopAir (BYTE * & pb, DWORD literal)
{   *pb++ = 0x83 ;
    *pb++ = 0xC4 ;
    *pb++ = (BYTE)literal ;
}

static inline void EmitFild (BYTE * & pb, DWORD negativeOffsetFromEBP)
{   *pb ++ = fild1 ;
    *pb ++ = fild2 ;
    EmitLocalVariableIndicatorByte (pb, negativeOffsetFromEBP) ;
}

static inline void EmitSpecialFld (BYTE * & pb)
{   *pb ++ = 0xDD ;
    *pb ++ = 0x40 ;
    *pb ++ = 0x08 ;
}
static inline void EmitSpecialFstp (BYTE * & pb)
{   EmitPushAir (pb, 8) ;
    *pb ++ = 0xDD ;
    *pb ++ = 0x1C ;
    *pb ++ = 0x24 ;
}

static inline void EmitSpecialFld (BYTE * & pb, DWORD byte)
{   *pb ++ = 0xDD ;
    *pb ++ = 0x40 ;
    *pb ++ = (BYTE) byte ;
}

static inline void EmitLoadRegister (BYTE * & pb, DWORD literal)
{   *pb++ = xload;
    *pb++ = (BYTE)literal ;
}

static inline void EmitPushLiteral (BYTE * & pb, DWORD literal)
{   *pb++ = xpush;
    *pb++ = (BYTE)literal ;
}

static inline void EmitStoreLiteralLocal (BYTE * & pb, DWORD negativeOffsetFromEBP, DWORD literal)
{   *pb++ = 0xC7 ;
    *pb++ = 0x45 ;
    EmitLocalVariableIndicatorByte (pb, negativeOffsetFromEBP) ;
    PaintLiteral (pb, literal) ;
}

static inline void EmitCmpLocalLiteral (BYTE * & pb, DWORD negativeOffsetFromEBP, DWORD literal)
{   *pb++ = 0x83 ;
    *pb++ = 0x7D ;
    EmitLocalVariableIndicatorByte (pb, negativeOffsetFromEBP) ;
    *pb++ = (BYTE) literal ;
}

static inline void EmitDebugBreak (BYTE * & pb)
{   *pb ++ = 0xCC ;
}

static inline void EmitClearEAX (BYTE * & pb)
{   *pb ++ = 0x33 ;
    *pb ++ = 0xC0 ;
}

static inline void EmitPrologue (BYTE * & pb, long arity) 
{   
 // EmitDebugBreak        (pb) ;
   
    PaintRGB (pb, (BYTE*)prologue, sizeof (prologue)) ;
    EmitStoreLiteralLocal (pb, ELarity, arity) ;
    EmitStoreLiteralLocal (pb, ELargList, 0) ;
}

static inline void EmitJmp (BYTE * & pb, DWORD cb)
{   *pb++ = jmp ;
    *pb++ = (BYTE)(cb) ;
}

static inline void EmitJge (BYTE * & pb, DWORD cb)
{   *pb++ = jge ;
    *pb++ = (BYTE)(cb) ;
}

static inline void EmitJne (BYTE * & pb, DWORD cb)
{   *pb++ = jne ;
    *pb++ = (BYTE)(cb) ;
}

static inline void EmitAddECXLiteral (BYTE * & pb, DWORD literal)
{   *pb++ = add ;
    *pb++ = 0xC1 ;
    *pb++ = (BYTE)literal ;
}

static inline void EmitSubEAXLiteral (BYTE * & pb, DWORD literal)
{   *pb++ = 0x83 ;
    *pb++ = 0xE8 ;
    *pb++ = (BYTE)literal ;
}

static inline void EmitShlEAXLiteral (BYTE * & pb, DWORD literal)
{   *pb++ = 0xC1 ;
    *pb++ = 0xE0 ;
    *pb++ = (BYTE)literal ;
}

static inline void EmitLEASpecial (BYTE * & pb, DWORD offset)
{   *pb++ = 0x8D ;
    *pb++ = 0x4C ;
    *pb++ = 0x85 ;
    *pb++ = (BYTE)offset ;
}

static inline void EmitSubEAXLocal (BYTE * & pb, DWORD negativeOffsetFromEBP)
{   *pb ++ = 0x2B ;
    *pb ++ = 0x45 ;
    EmitLocalVariableIndicatorByte (pb, negativeOffsetFromEBP) ;
}

static inline void EmitCmpLocal (BYTE * & pb, ERegister eReg, DWORD negativeOffsetFromEBP)
{   *pb ++ = cmp ;
    *pb ++ = eReg ;
    EmitLocalVariableIndicatorByte (pb, negativeOffsetFromEBP) ;
}

static inline void EmitPushEAX (BYTE * & pb) 
{   PaintRGB (pb, (BYTE*)pusheax, sizeof (pusheax)) ;
}

static inline void EmitPushEDX (BYTE * & pb)
{   *pb++ = 0x52 ;
}

static inline void EmitCallPop (BYTE * & pb) 
{   PaintRGB (pb, (BYTE*)callpop, sizeof(callpop)) ;
}
static inline void EmitCallRelativeStd   (BYTE * & pb, BYTE * functocall)
{	*pb ++  =  call ;
	DWORD relativeaddress = functocall - (pb + 4) ;
    PaintPDW (pb, &relativeaddress) ;
}

static inline void EmitCallRelativeCDecl (BYTE * & pb, BYTE * functocall)
{   EmitCallRelativeStd (pb, functocall) ;
    EmitCallPop (pb) ;
}

static inline void EmitLoopPrologue (BYTE * & pb)
{   /*
    447:      for (long i = 0 ;  i < arity ;  i++)
    007E64E8 C7 45 F4 00 00 00 00 mov         dword ptr [i],0
    007E64EF EB 09                jmp         engineer+2Ah (007e64fa)
    007E64F1 8B 4D F4             mov         ecx,dword ptr [i]
    007E64F4 83 C1 01             add         ecx,1
    007E64F7 89 4D F4             mov         dword ptr [i],ecx
    007E64FA 8B 55 F4             mov         edx,dword ptr [i]
    007E64FD 3B 55 FC             cmp         edx,dword ptr [arity]
    007E6500 7D 45                jge         engineer+77h (007e6547)
    */
    EmitStoreLiteralLocal (pb, ELi, 0) ;
    EmitJmp               (pb, 9) ;
    EmitLoadLocal         (pb, ecx, ELi) ;
    EmitAddECXLiteral     (pb, 1) ;
    EmitStoreLocal        (pb, ecx, ELi) ;
    EmitLoadLocal         (pb, edx, ELi) ;
    EmitCmpLocal          (pb, edx, ELarity) ;
    EmitJge               (pb, 0x42) ;  // MUST USE 42 BECAUSE OF SHL BUGBUG
}

static inline void EmitLoopBody (BYTE * & pb)
{   /*
    448:      {
    449:          DWORD * argAddr = (&ebp08 + 4*(arity-1-i)) ;
    007E6502 8B 45 FC             mov         eax,dword ptr [arity]
    007E6505 83 E8 01             sub         eax,1
    007E6508 2B 45 F4             sub         eax,dword ptr [i]
    BUGBUG X X X X 007E650B C1 E0 02             shl         eax,2  ;;; BUGBUG
    007E650E 8D 4C 85 08          lea         ecx,[ebp+8+eax*4]
    007E6512 89 4D F0             mov         dword ptr [argAddr],ecx
    450:          argList = cons ( flocons (*argAddr), argList ) ;
    007E6515 8B 55 F8             mov         edx,dword ptr [argList]
    007E6518 52                   push        edx
    007E6519 8B 45 F0             mov         eax,dword ptr [argAddr]
    007E651C 8B 08                mov         ecx,dword ptr [eax]
    007E651E 89 4D E4             mov         dword ptr [flTmp2],ecx
    007E6521 C7 45 E8 00 00 00 00 mov         dword ptr [flTmp1],0
    007E6528 DF 6D E4             fild        qword ptr [flTmp2]
    007E652B 83 EC 08             sub         esp,8
    007E652E DD 1C 24             fstp        qword ptr [esp]
    007E6531 E8 D6 1D 00 00       call        flocons (007e830c)
    007E6536 83 C4 08             add         esp,8
    007E6539 50                   push        eax
    007E653A E8 D9 1D 00 00       call        cons (007e8318)
    007E653F 83 C4 08             add         esp,8
    007E6542 89 45 F8             mov         dword ptr [argList],eax
    451:      }
    007E6BC5 EB AA                jmp         engineer+21h (007e6b71)
*/
    EmitLoadLocal         (pb, eax, ELarity) ;
    EmitSubEAXLiteral     (pb, 1) ;
    EmitSubEAXLocal       (pb, ELi) ;
    ///EmitShlEAXLiteral     (pb, 2) ;
    EmitLEASpecial        (pb, 8) ;
    EmitStoreLocal        (pb, ecx, ELargAddr) ;
    EmitLoadLocal         (pb, edx, ELargList) ;
    EmitPushEDX           (pb) ;
    EmitLoadLocal         (pb, eax, ELargAddr) ;
    EmitLoadRegister      (pb, 8) ;
    EmitStoreLocal        (pb, ecx, ELflTmp2) ;
    EmitStoreLiteralLocal (pb, ELflTmp1, 0) ;
    EmitFild              (pb, ELflTmp2) ;
    EmitSpecialFstp       (pb) ;
    EmitCallRelativeCDecl (pb, (BYTE*)flocons) ;
    EmitPushEAX           (pb) ;
    EmitCallRelativeCDecl (pb, (BYTE*)cons) ;
    EmitStoreLocal        (pb, eax, ELargList) ;
    EmitJmp               (pb, 0xAD) ; // MUST USE AD BECAUSE OF SHL BUGBUG
}

static inline void EmitPushClosure (BYTE * & pb, BYTE * closure)
{	*pb ++  =  0x68 ;  // push
    PaintPDW (pb, ((DWORD*)(& closure))) ;
}

static inline void EmitLoopEpilogue (BYTE * & pb, BYTE* closure)
{
    /*
    648:      DWORD d = (DWORD) leval
    649:      (
    650:          cons
    651:          (
    652:              (LISP)0xFEEDBEEF
    653:              ,argList
    660:          )
    661:          ,0
    662:      )
    663:      ;
    007E6BC7 6A 00                push        0
    007E6BC9 8B 55 F8             mov         edx,dword ptr [ELargList]
    007E6BCC 52                   push        edx
    007E6BCD 68 EF BE ED FE       push        0FEEDBEEFh
    007E6BD2 E8 71 17 00 00       call        cons (007e8348)
    007E6BD7 83 C4 08             add         esp,8
    007E6BDA 50                   push        eax
    007E6BDB E8 80 17 00 00       call        leval (007e8360)
    007E6BE0 83 C4 08             add         esp,8
    007E6BE3 89 45 EC             mov         dword ptr [ELd],eax
    */
    EmitPushLiteral       (pb, 0) ;
    EmitLoadLocal         (pb, edx, ELargList) ;
    EmitPushEDX           (pb) ;
   	EmitPushClosure       (pb, closure) ; // here's the biggy
	EmitCallRelativeCDecl (pb, (BYTE*)cons   ) ;
	EmitPushEAX           (pb) ;
	EmitCallRelativeCDecl (pb, (BYTE*)leval  ) ;
    EmitStoreLocal        (pb, eax, ELd) ;
}

static DWORD DwFromDouble (double d)
{   return (DWORD) d ;
}

static inline void EmitNewReturn (BYTE * & pb)
{
    /*
    665:      return DwFromDouble(FLONM((LISP)d));
    007E6BE6 8B 45 EC             mov         eax,dword ptr [ELd]
    007E6BE9 DD 40 08             fld         qword ptr [eax+8]
    007E6BEC 83 EC 08             sub         esp,8
    007E6BEF DD 1C 24             fstp        qword ptr [esp]
    007E6BF2 E8 09 CE FF FF       call        DwFromDouble (007e3a00)
    007E6BF7 83 C4 08             add         esp,8
    666:  }
    /* // I don't know why this bullxxit is here: it's BUGBUG
    EmitLoadLocal         (pb, eax, ELd) ;
    EmitSpecialFld        (pb, 8) ;
    EmitSpecialFstp       (pb) ;
    EmitCallRelativeCDecl (pb, (BYTE*)DwFromDouble) ;
    */

    /*
    007E6F43 89 45 EC             mov         dword ptr [ebp-14h],eax
    818:
    819:      if (d==0)
    007E6F46 83 7D EC 00          cmp         dword ptr [ebp-14h],0
    007E6F4A 75 04                jne         engineer+0A0h (007e6f50)
    820:          return 0 ;
    007E6F4C 33 C0                xor         eax,eax
    007E6F4E EB 14                jmp         engineer+0B4h (007e6f64)
    821:      else
    822:          return DwFromDouble(FLONM((LISP)d));
    007E6F50 8B 45 EC             mov         eax,dword ptr [ebp-14h]
    007E6F53 DD 40 08             fld         qword ptr [eax+8]
    007E6F56 83 EC 08             sub         esp,8
    007E6F59 DD 1C 24             fstp        qword ptr [esp]
    007E6F5C E8 0F 02 00 00       call        DwFromDouble (007e7170)
    007E6F61 83 C4 08             add         esp,8
    823:  }

    */
    EmitCmpLocalLiteral (pb, ELd, 0) ;
    EmitJne (pb, 4) ;
    EmitClearEAX (pb) ;
    EmitJmp (pb, 0x14) ;
    EmitLoadLocal (pb, eax, ELd) ;
    EmitSpecialFld (pb) ;
    EmitSpecialFstp (pb) ;
    EmitCallRelativeCDecl (pb, (BYTE*)DwFromDouble) ;
}

static inline void EmitUpToFirstCall (BYTE * & pb) 
{   PaintRGB (pb, (BYTE*)uptofirstcall, sizeof(uptofirstcall)) ;
}

static inline void EmitOutputTransform (BYTE * & pb) 
{   PaintRGB(pb, (BYTE*)outputTransform, sizeof (outputTransform)) ;
    EmitCallRelativeCDecl (pb, (BYTE*)DwFromDouble) ;
}

static inline void EmitEpilogue (BYTE * & pb, long arity) 
{   
    /*
    0x5F,                   	  // pop         edi
    0x5E,                   	  // pop         esi
    0x5B,                   	  // pop         ebx
    0x8B, 0xE5,					  // mov         esp,ebp
	0x5D,						  // pop         ebp
	0xC2, 0x04, 0x00              // ret         4
    */
    
    PaintRGB (pb, (BYTE*)epilogue, sizeof (epilogue)-2) ;

    arity *= 4 ;

    WORD w = (WORD)arity ;

    CopyMemory (pb, &w, 2) ;

    pb += 2 ;
}

static void EmitSpecializedCall (BYTE * & pb, BYTE * closure, long arity)
{
    EmitPrologue          (pb, arity) ;
    /**/
    EmitLoopPrologue      (pb) ;
    EmitLoopBody          (pb) ;
    EmitLoopEpilogue      (pb, closure) ;
    EmitNewReturn         (pb) ;
    /** /
    EmitUpToFirstCall     (pb) ;
	EmitCallRelativeCDecl (pb, (BYTE*)flocons) ;
	EmitPushEAX           (pb) ;
	EmitCallRelativeCDecl (pb, (BYTE*)cons   ) ;
	EmitPushEAX           (pb) ;
	EmitPushClosure       (pb, closure       ) ; // here's the biggy
	EmitCallRelativeCDecl (pb, (BYTE*)cons   ) ;
	EmitPushEAX           (pb) ;
	EmitCallRelativeCDecl (pb, (BYTE*)leval  ) ;
    EmitOutputTransform   (pb) ;
    /**/
    EmitEpilogue          (pb, arity) ;	
}

static int CBCallback (BYTE * closure, long * pArity)
{   static int retval = 0 ;

    LISP z = (LISP)closure ;

    // First, find out the arity of the function (i.e., the length of the argument list).
    
    z = (CDR(z)) ; // this contains the argument list in its car and the code in its cdr
    z = (CAR(z)) ; // this is the argument list
    *pArity = nlength (z) ;

    if (0 == retval)
    {
        BYTE * temp = (BYTE*)LocalAlloc (0x40, 4096) ;
        BYTE * pb2 = temp ;
        EmitSpecializedCall (pb2, (BYTE*)closure, *pArity) ;
        retval = pb2 - temp ;
        LocalFree (temp) ;
    }
    return retval ;
}

typedef DWORD ( __stdcall * CALLBACK_TYPE ) ( DWORD ) ;

static BYTE * CreateCallback (BYTE * closure)
{
    long arity ;
    DWORD cb = CBCallback (closure, &arity) ;

    BYTE * temp = (BYTE*)LocalAlloc (0x40, cb) ;
    BYTE * pb2 = temp ;
    DWORD oldProtect ;
    BOOL f = VirtualProtect (temp, cb, PAGE_EXECUTE_READWRITE, &oldProtect) ;
    if (! f)
        err ("Can't allocate Callback", flocons(GetLastError())) ;
    EmitSpecializedCall (pb2, closure, arity) ;
    return temp ;
}

DWORD __stdcall engineer (DWORD ebp08, DWORD ebp0C, DWORD ebp10, DWORD ebp14);

LISP SiodCallback (LISP lclosure)
{
//    engineer (0, 1, 2, 3) ;

    if (TYPE(lclosure) != tc_closure)
        err ("wta callback", lclosure) ;

    BYTE * cb = CreateCallback ((BYTE*)lclosure) ;
    
    CALLBACK_TYPE f = (CALLBACK_TYPE)cb ;

    //    DWORD dw2 = f(42) ;
    
    return flocons ((DWORD)f) ;
}

//
// The C - facade will be a pointer to function that returns a DWORD and takes
// a single DWORD argument, as a stdcall. 
  /*

  Ok, here's what we need to do.  Suppose we have a C function that wants a 
  callback address, but we want the code for the callback to be written in 
  Siod.  Great.  When we call the C function, and we get an argument of Siod
  type tc_closure, we'll assume that we're passing such a callback to the C
  function.  So, we'll gen up a simple C wrapper that calls the given closure.
  
	It should be mostly like the following (this is a starting point -- we'll
	abstract the argument lists later and add advanced error checking later):

	268:  DWORD __stdcall engineer (DWORD input)
	269:  {
	00DD5CF1 55                   push        ebp
	00DD5CF2 8B EC                mov         ebp,esp
	00DD5CF4 83 EC 0C             sub         esp,0Ch
	270:      // The closure is some constant we'll be given.  This must construct a
	271:      // call to the closure with the argument given.
	272:      //
	273:      DWORD d = (DWORD) leval
	274:      (   cons
	276:          (   (LISP)0xFEEDBEEF // this thing had better be a closure
	278:              ,cons
	279:              (   flocons (input)
	281:                  ,0
	283:          )   )
	284:          ,0
	285:      )
	286:      ;
	00DD5CF7 6A 00                push        0
	00DD5CF9 6A 00                push        0
	00DD5CFB 8B 45 08             mov         eax,dword ptr [input]
	00DD5CFE 89 45 F4             mov         dword ptr [ebp-0Ch],eax
	00DD5D01 C7 45 F8 00 00 00 00 mov         dword ptr [ebp-8],0
	00DD5D08 DF 6D F4             fild        qword ptr [ebp-0Ch]
	00DD5D0B 83 EC 08             sub         esp,8
	00DD5D0E DD 1C 24             fstp        qword ptr [esp]
	00DD5D11 E8 12 10 00 00       call        _flocons(0x00dd6d28)
	00DD5D16 83 C4 08             add         esp,8
	00DD5D19 50                   push        eax
	00DD5D1A E8 15 10 00 00       call        _cons(0x00dd6d34)
	00DD5D1F 83 C4 08             add         esp,8
	00DD5D22 50                   push        eax
	00DD5D23 68 EF BE ED FE       push        0FEEDBEEFh
	00DD5D28 E8 07 10 00 00       call        _cons(0x00dd6d34)
	00DD5D2D 83 C4 08             add         esp,8
	00DD5D30 50                   push        eax
	00DD5D31 E8 28 BD FF FF       call        _leval(0x00dd1a5e)
	00DD5D36 83 C4 08             add         esp,8
	00DD5D39 89 45 FC             mov         dword ptr [d],eax
	287:
	288:      return d ;
	00DD5D3C 8B 45 FC             mov         eax,dword ptr [d]
	289:  }
	00DD5D3F 8B E5                mov         esp,ebp
	00DD5D41 5D                   pop         ebp
	00DD5D42 C2 04 00             ret         4
  */

/*

  Here's the new "engineer" with variable arity:

632:  DWORD __stdcall engineer (DWORD ebp08, DWORD ebp0C, DWORD ebp10, DWORD ebp14)
633:  {
007E6B50 55                   push        ebp
007E6B51 8B EC                mov         ebp,esp
007E6B53 83 EC 5C             sub         esp,5Ch
007E6B56 53                   push        ebx
007E6B57 56                   push        esi
007E6B58 57                   push        edi
634:      // The closure is some constant we'll be given.  This must construct a
635:      // call to the closure with the argument given.  The closure must compute
636:      // a flonum, and we'll return the address of the new LISP object, which
637:      // superglue will convert to a flonum.
638:      //
639:      long arity = _testArity ;
007E6B59 A1 80 9C 81 00       mov         eax,[_testArity (00819c80)]
007E6B5E 89 45 FC             mov         dword ptr [ebp-4],eax
640:      LISP argList = 0 ;
007E6B61 C7 45 F8 00 00 00 00 mov         dword ptr [ebp-8],0
641:
642:      for (long i = 0 ;  i < arity ;  i++)
007E6B68 C7 45 F4 00 00 00 00 mov         dword ptr [ebp-0Ch],0
007E6B6F EB 09                jmp         engineer+2Ah (007e6b7a)
007E6B71 8B 4D F4             mov         ecx,dword ptr [ebp-0Ch]
007E6B74 83 C1 01             add         ecx,1
007E6B77 89 4D F4             mov         dword ptr [ebp-0Ch],ecx
007E6B7A 8B 55 F4             mov         edx,dword ptr [ebp-0Ch]
007E6B7D 3B 55 FC             cmp         edx,dword ptr [ebp-4]
007E6B80 7D 45                jge         engineer+77h (007e6bc7)
643:      {
644:          DWORD * argAddr = (&ebp08 + 4*(arity-1-i)) ;
007E6B82 8B 45 FC             mov         eax,dword ptr [ebp-4]
007E6B85 83 E8 01             sub         eax,1
007E6B88 2B 45 F4             sub         eax,dword ptr [ebp-0Ch]
007E6B8B C1 E0 02             shl         eax,2
007E6B8E 8D 4C 85 08          lea         ecx,[ebp+eax*4+8]
007E6B92 89 4D F0             mov         dword ptr [argAddr],ecx
645:          argList = cons ( flocons (*argAddr), argList ) ;
007E6B95 8B 55 F8             mov         edx,dword ptr [ebp-8]
007E6B98 52                   push        edx
007E6B99 8B 45 F0             mov         eax,dword ptr [argAddr]
007E6B9C 8B 08                mov         ecx,dword ptr [eax]
007E6B9E 89 4D E4             mov         dword ptr [ebp-1Ch],ecx
007E6BA1 C7 45 E8 00 00 00 00 mov         dword ptr [ebp-18h],0
007E6BA8 DF 6D E4             fild        qword ptr [ebp-1Ch]
007E6BAB 83 EC 08             sub         esp,8
007E6BAE DD 1C 24             fstp        qword ptr [esp]
007E6BB1 E8 86 17 00 00       call        flocons (007e833c)
007E6BB6 83 C4 08             add         esp,8
007E6BB9 50                   push        eax
007E6BBA E8 89 17 00 00       call        cons (007e8348)
007E6BBF 83 C4 08             add         esp,8
007E6BC2 89 45 F8             mov         dword ptr [ebp-8],eax
646:      }
007E6BC5 EB AA                jmp         engineer+21h (007e6b71)
647:
648:      DWORD d = (DWORD) leval
649:      (
650:          cons
651:          (
652:              (LISP)0xFEEDBEEF
653:              ,argList
654:
655:              //,cons
656:              //(
657:              //  flocons (input)
658:              //  ,0
659:              //)
660:          )
661:          ,0
662:      )
663:      ;
007E6BC7 6A 00                push        0
007E6BC9 8B 55 F8             mov         edx,dword ptr [ebp-8]
007E6BCC 52                   push        edx
007E6BCD 68 EF BE ED FE       push        0FEEDBEEFh
007E6BD2 E8 71 17 00 00       call        cons (007e8348)
007E6BD7 83 C4 08             add         esp,8
007E6BDA 50                   push        eax
007E6BDB E8 80 17 00 00       call        leval (007e8360)
007E6BE0 83 C4 08             add         esp,8
007E6BE3 89 45 EC             mov         dword ptr [ebp-14h],eax
664:
665:      return DwFromDouble(FLONM((LISP)d));
007E6BE6 8B 45 EC             mov         eax,dword ptr [ebp-14h]
// BEGIN BULLXXIT
007E6BE9 DD 40 08             fld         qword ptr [eax+8]
007E6BEC 83 EC 08             sub         esp,8
007E6BEF DD 1C 24             fstp        qword ptr [esp]
007E6BF2 E8 09 CE FF FF       call        DwFromDouble (007e3a00)
007E6BF7 83 C4 08             add         esp,8
// END BULLXXIT
666:  }
007E6BFA 5F                   pop         edi
007E6BFB 5E                   pop         esi
007E6BFC 5B                   pop         ebx
007E6BFD 8B E5                mov         esp,ebp
007E6BFF 5D                   pop         ebp
007E6C00 C2 10 00             ret         10h // ACTUALLY, 4 * ARITY
// NON-BULLXXIT
007E6F43 89 45 EC             mov         dword ptr [ebp-14h],eax
818:
819:      if (d==0)
007E6F46 83 7D EC 00          cmp         dword ptr [ebp-14h],0
007E6F4A 75 04                jne         engineer+0A0h (007e6f50)
820:          return 0 ;
007E6F4C 33 C0                xor         eax,eax
007E6F4E EB 14                jmp         engineer+0B4h (007e6f64)
821:      else
822:          return DwFromDouble(FLONM((LISP)d));
007E6F50 8B 45 EC             mov         eax,dword ptr [ebp-14h]
007E6F53 DD 40 08             fld         qword ptr [eax+8]
007E6F56 83 EC 08             sub         esp,8
007E6F59 DD 1C 24             fstp        qword ptr [esp]
007E6F5C E8 0F 02 00 00       call        DwFromDouble (007e7170)
007E6F61 83 C4 08             add         esp,8
823:  }
 */

//================================================================================
DWORD _testArity = 4 ;

DWORD __stdcall engineer (DWORD ebp08, DWORD ebp0C, DWORD ebp10, DWORD ebp14)
{
	// The closure is some constant we'll be given.  This must construct a 
	// call to the closure with the argument given.  The closure must compute
    // a flonum, and we'll return the address of the new LISP object, which 
    // superglue will convert to a flonum. 
	//
    long arity = _testArity ;
    LISP argList = 0 ;

    for (long i = 0 ;  i < arity ;  i++)
    {
        DWORD * argAddr = (&ebp08 + 4*(arity-1-i)) ;
        argList = cons ( flocons (*argAddr), argList ) ;
    }

	DWORD d = (DWORD) leval 
	(
		cons 
		(
			(LISP)0xFEEDBEEF
            ,argList

			//,cons 
			//(
			//	flocons (input)
			//	,0
			//)
		)
		,0
	) 
	;

    if (d==0)
        return 0 ;
    else
	    return DwFromDouble(FLONM((LISP)d));
}


//================================================================================
// Use this to convert arguments for calling DLL entry points
//================================================================================

DWORD  CTypeFromLispType (LISP z)
{
//    engineer (0) ;
    switch (TYPE(z))
    {
    case tc_nil:          return 0 ;
    case tc_cons:         return (DWORD)(&(z->storage_as.cons)) ;
    case tc_flonum:       return (DWORD)(FLONM(z)) ;
    case tc_symbol:       return (DWORD)(PNAME(z)) ;
    case tc_subr_0:       return (DWORD)(SUBR0(z)) ;
    case tc_subr_1:       return (DWORD)(SUBR1(z)) ;
    case tc_subr_2:       return (DWORD)(SUBR2(z)) ;
    case tc_subr_3:       return (DWORD)(SUBR3(z)) ;
    case tc_subr_4:       return (DWORD)(SUBR4(z)) ;
    case tc_subr_5:       return (DWORD)(SUBR5(z)) ;
    case tc_subr_2n:      return (DWORD)(SUBR2(z)) ;
    case tc_lsubr:        return (DWORD)(SUBRF(z)) ;
    case tc_fsubr:        return (DWORD)(SUBRF(z)) ;
    case tc_msubr:        return (DWORD)(SUBRM(z)) ;
    case tc_free_cell:    return (DWORD)( &(z->storage_as)              ) ;
    case tc_string:       return (DWORD)(z->storage_as.string.data      ) ;
    case tc_double_array: return (DWORD)(z->storage_as.double_array.data) ;
    case tc_long_array:   return (DWORD)(z->storage_as.long_array.data  ) ;
    case tc_lisp_array:   return (DWORD)(z->storage_as.long_array.data  ) ;
    case tc_c_file:       return (DWORD)(z->storage_as.c_file.f         ) ;
    case tc_byte_array:   return (DWORD)(z->storage_as.string.data      ) ;
    case tc_closure:      return (DWORD)(FLONM(SiodCallback(z))) ;

    //
    // Use CPTRs when the entry point needs an address to load.  Lisp
    // will treat a CPTR as a void*, but it can be converted to a number
    // and back.
    //
    
    case tc_cptr:         return (DWORD)( & CPTR(z) ) ;

    default:
      /*
        if (TYPE(z) == tc_guid)       {return 0 ;}
        if (TYPE(z) == tc_hresult)    {return 0 ;}
        if (TYPE(z) == tc_interface)  {return 0 ;}
        if (TYPE(z) == tc_typeattr)   {return 0 ;}
        if (TYPE(z) == tc_funcdesc)   {return 0 ;}
        if (TYPE(z) == tc_vardesc)    {return 0 ;}
        if (TYPE(z) == tc_typedesc)   {return 0 ;}
        if (TYPE(z) == tc_bstr)       {return 0 ;}
      */

        return (int)err("unknown LISP type", z) ;
    }

}

#define CHECK_C_TYPE(E,F) if (TYPE(E) != tc_flonum) {return err("wta to " F, E) ;}
#define CHECK_TYPE(E,F,T) if (TYPE(E) != T        ) {return err("wta to " F, E) ;}


//================================================================================
// Get a Lisp String from a CString and vice versa.
//================================================================================

LISP StringFromCString (LISP flo)
{
    CHECK_C_TYPE(flo, "StringFromCString") ;

    LISP z = strcons (-1, SZ(flo)) ;
    return z ;
}

LISP CStringFromString (LISP str)
{
    CHECK_TYPE(str, "CStringFromString", tc_string) ;

    DWORD cb = DIM(str) + 1 ;
    char * sz = must_malloc (cb) ;
    memcpy (sz, STR(str), cb) ;

    return flocons ((double)(DWORD)(sz)) ;
}

//================================================================================
// Get a Lisp Byte Array from a C Byte Array and vice versa.
//================================================================================

LISP ByteArrayFromCByteArray (LISP floPb, LISP floCb)
{
    if (floCb == NULL)
        return err("need CB", floCb) ;

    if (floPb == NULL)
        return err("need PB", floPb) ;

    long cb = CB(floCb) ;

    LISP z = arcons (tc_byte_array, cb, 0) ;

    memcpy (STR(z), PB(floPb), cb) ;

    return z ;
}

LISP SiodArrayFromCByteArray (LISP dim, LISP kind, LISP floPb)
{
    LISP    a ;
    long flag ;
    long    n ;
    
    if (NFLONUMP(dim) || (FLONM(dim) < 0))
        return (err("bad dimension to carray->array",dim));
    else
        n = (long) FLONM(dim);

    if (floPb == NULL)
        return err("need c array", floPb) ;

    flag = no_interrupt(1);
    a = cons(NIL,NIL);
    if (EQ (cintern("double"),kind))
    {
        a->type = tc_double_array;
        a->storage_as.double_array.dim = n;
        a->storage_as.double_array.data = (double *) must_malloc(n *sizeof(double));
        memcpy (a->storage_as.double_array.data, PB(floPb), sizeof(double)*n) ;
    }
    else if (EQ (cintern("long"),kind))
    {
        a->type = tc_long_array;
        a->storage_as.long_array.dim = n;
        a->storage_as.long_array.data = (long *) must_malloc(n * sizeof(long));
        memcpy (a->storage_as.long_array.data, PB(floPb), sizeof(long)*n) ;
    }
    else if (EQ (cintern("string"),kind))
    {
        a->type = tc_string;
        a->storage_as.string.dim = n;
        a->storage_as.string.data = (char *) must_malloc(n+1);
        a->storage_as.string.data[n] = 0;
        memcpy (a->storage_as.string.data, PB(floPb), n) ;
    }
    else if (EQ (cintern("byte"),kind))
    {
        a->type = tc_byte_array;
        a->storage_as.string.dim = n;
        a->storage_as.string.data = (char *) must_malloc(n);
        memcpy (a->storage_as.string.data, PB(floPb), n) ;
    }
/**/else if (EQ(cintern("cptr"),kind))
    {
        a->type = tc_cptr_array;
        a->storage_as.cptr_array.dim = n;
        a->storage_as.cptr_array.data = (void**) must_malloc(n * sizeof(void*)) ;
        memcpy (a->storage_as.cptr_array.data, PB(floPb), n * sizeof(void*)) ;
    }
/**/else if (EQ(cintern("lisp"),kind) || NULLP(kind))
    {
        a->type = tc_lisp_array;
        a->storage_as.lisp_array.dim = n;
        a->storage_as.lisp_array.data = (LISP *) must_malloc(n * sizeof(LISP));
        memcpy (a->storage_as.lisp_array.data, PB(floPb), n * sizeof(LISP)) ;
    }
    else
        err("bad type of array",kind);
    no_interrupt(flag);
    return (a);
}

LISP CByteArrayFromArray (LISP a)
{
    DWORD cb    = 0 ;
    PBYTE pb    = 0 ;
    PBYTE pbSrc = 0 ;

    switch (TYPE (a))
    {
    case tc_string:
    case tc_byte_array:
        cb = DIM(a) + 1 ;
        pbSrc = (PBYTE)STR(a) ;
        break ;

    case tc_double_array:
        cb = a->storage_as.double_array.dim * sizeof(double) ;
        pbSrc = (PBYTE)a->storage_as.double_array.data ;
        break ;

    case tc_long_array:
        cb = a->storage_as.long_array.dim * sizeof(long) ;
        pbSrc = (PBYTE)a->storage_as.long_array.data ;
        break ;

    case tc_lisp_array:
        cb = a->storage_as.lisp_array.dim * sizeof(LISP) ;
        pbSrc = (PBYTE)a->storage_as.lisp_array.data ;
        break ;

    default:
        return (err("invalid CByteArrayFromArray",a));
    }
        
    pb = (PBYTE)must_malloc (cb) ;
    memcpy (pb, pbSrc, cb) ;
    return flocons ((double)(DWORD)(pb)) ;
}

//
// Interpret every Lisp Element as a flocons containing a secret
// C pointer. 
//
LISP RgPvFromLispArray (LISP a)
{
    DWORD   cb    = 0 ;
    DWORD   cpv   = 0 ;
    PVOID * rgpv  = 0 ;
    PVOID   pv    = 0 ;
    DWORD   i     = 0 ;
    LISP    l     = 0 ;
    double  f     = 0 ;
    
    switch (TYPE (a))
    {
    case tc_lisp_array:
        {
            cpv = a->storage_as.lisp_array.dim ;
            cb =  cpv * sizeof(PVOID) ;
            rgpv = (PVOID*)must_malloc (cb) ;
            for (i = 0 ;  i < cpv ;  i ++)
            {
                l = a->storage_as.lisp_array.data[i] ;
                if (NFLONUMP(l))
                    return err ("wta array->rgpv", l) ;
                    
                pv = PV (l) ;
                rgpv[i] = pv ;
            }
        }
        break ;

    default:
        return (err("wta array->rgpv", a));
    }
        
    return flocons ((double)(DWORD)(rgpv)) ;
}

LISP LispArrayFromRgPv (LISP floRgPv, LISP floCpv)
{
    if (floCpv == NULL)
        return err("need dim of RgPv", floCpv) ;

    if (floRgPv == NULL)
        return err("need RgPv", floRgPv) ;

    long    cpv  = LNG(floCpv) ;
    PVOID * rgpv = RGPV(floCpv) ;
    long    i    = 0 ;

    LISP z = arcons (tc_lisp_array, cpv, 0) ;

    for (i = 0 ;  i < cpv ;  i ++)
    {
        z->storage_as.lisp_array.data[i] = flocons ((double)(DWORD)(rgpv[i])) ;
    }

    return z ;
}

//================================================================================
// Call a Stdcall C function after converting and pushing arguments
//================================================================================

LISP MainDispatcher (LISP floAddr, LISP listOfArgs)
{
    FARPROC func = FuncPtrFromLisp(floAddr) ;

    LISP l = reverse (listOfArgs) ;

    for (LISP p = l ; NNULLP(p); p = cdr(p))
    {
        DWORD x = CTypeFromLispType (car (p)) ;
        __asm {push x}
    }

	DWORD  d = func () ;
	double x = (double) d ;
	LISP   f = flocons (x) ;
	
	return f ;
}

//================================================================================
// Call a CDECL function after converting and pushing arguments
//================================================================================

LISP MainDispatcherCDecls (LISP floAddr, LISP listOfArgs)
{
    FARPROC func = FuncPtrFromLisp(floAddr) ;
    int pops ;

    LISP l = reverse (listOfArgs) ;
    LISP r ;

    pops = 0 ;
    for (LISP p = l ; NNULLP(p); p = cdr(p))
    {
        DWORD x = CTypeFromLispType (car (p)) ;
        pops ++ ;
        __asm {push x}
    }

    pops *= 4 ;
    
    r = flocons ((double)func ()) ;

    __asm {add esp,pops}

    return r ;
}

//================================================================================
// Find out whether a DLL's export is a CDECL
//================================================================================

BOOL IsThingACDecl (char * name)
{
    char   tank[512]    ;
    char * ptank = tank ;

    DWORD dw = UnDecorateSymbolName 
    (
        name, tank, sizeof(tank), 
        UNDNAME_COMPLETE                    | 
        //UNDNAME_NO_LEADING_UNDERSCORES    |
        UNDNAME_NO_FUNCTION_RETURNS         | 
        //UNDNAME_NO_ALLOCATION_LANGUAGE    |
        //UNDNAME_NO_MS_THISTYPE            | 
        //UNDNAME_NO_CV_THISTYPE            |
        //UNDNAME_NO_THISTYPE               | 
        //UNDNAME_NO_ACCESS_SPECIFIERS      |
        //UNDNAME_NO_THROW_SIGNATURES       | 
        //UNDNAME_NO_MEMBER_TYPE            |
        //UNDNAME_NO_RETURN_UDT_MODEL       | 
        //UNDNAME_32_BIT_DECODE             |
        //UNDNAME_NO_ARGUMENTS              | 
        //UNDNAME_NO_SPECIAL_SYMS           |
        0
    ) ;

    if (strlen(tank)==0)
        return FALSE ; // assume __stdcall for Windows & C DLLs

    char * score ;
    while (score = strchr (ptank, '_'))
    {
        if      (strncmp (score, "__stdcall",   9) == 0)
            return FALSE ;
        else if (strncmp (score, "__cdecl",     7) == 0)
            return TRUE ;
        else if (strncmp (score, "__fastcall", 10) == 0)
            return FALSE ;

        ptank ++ ;
    }

    return FALSE ;
}

//================================================================================
// Get a list of all the exported functions in a DLL
//================================================================================

LISP GetProcAlist (LISP filename)
{
    PEA  pea ;
    int  i   ;
    char tank[512] ;

    if ((TYPE(filename) != tc_string) && (TYPE(filename) != tc_symbol))
        return err("wta GetProcNames", filename);

    i = LoadLibraryXXX (&pea, filename->storage_as.string.data) ;
    if (i == 0) return NIL ;

    HINSTANCE h = LoadLibrary (filename->storage_as.string.data) ;
    if (h == NULL) return NIL ;

    char *  rgszExports ;
    char *  sz          ;
    int     cExports      = pea.NumberOfExportedFunctions ();
    int     cbExportNames = pea.GetExportFunctionNames (&rgszExports);
    LISP    lout = NIL ;
    LISP    f, g, k ;
    FARPROC zed ;

    for
    (
        i = 0, sz = rgszExports;
        i < cExports;
        i++, sz += strlen (sz) + 1
    )
    {
		if (strlen (sz) == 0)
			// Stop when we first hit an empty name (empirical fact)
			break ;
		
        if (strchr (sz, '.'))
        {
			// skip names with dots in them, they're not real exports
            i-- ;
            continue ;
        }

        DWORD dw = UnDecorateSymbolName (sz, tank, sizeof(tank), 
            UNDNAME_NAME_ONLY | UNDNAME_COMPLETE | UNDNAME_NO_LEADING_UNDERSCORES) ;
        BOOL  b  = IsThingACDecl (sz) ;
        k = flocons ( (double)b ) ;

        zed = GetProcAddress (h, sz) ;

        if (zed == 0)
            f = llast_win32_errmsg (0) ;
        else
            f = flocons ( (double) (DWORD) zed ) ;

        if (lstrlenA(tank))
            g = rintern (tank);
        else
            g = rintern (sz) ;

        lout = cons ( cons (g, cons (f, k)), lout) ;
    }

    pea.Free (rgszExports);

    return lout ;
}

//================================================================================
// Get a list of exports from a DLL known to contain mostly, if not exclusively
// CDECLs. 
//================================================================================

LISP GetProcAlistCDecls (LISP filename)
{
    PEA  pea ;
    int  i   ;
    char tank[512] ;

    if ((TYPE(filename) != tc_string) && (TYPE(filename) != tc_symbol))
        return err("wta GetProcNamesCDecls", filename);

    i = LoadLibraryXXX (&pea, filename->storage_as.string.data) ;
    if (i == 0) return NIL ;

    HINSTANCE h = LoadLibrary (filename->storage_as.string.data) ;
    if (h == NULL) return NIL ;

    char *  rgszExports ;
    char *  sz          ;
    int     cExports      = pea.NumberOfExportedFunctions ();
    int     cbExportNames = pea.GetExportFunctionNames (&rgszExports);
    LISP    lout = NIL ;
    LISP    f, g, k ;
    FARPROC zed ;

    for
    (
        i = 0, sz = rgszExports;
        i < cExports && *sz ;
        i++, sz += strlen (sz) + 1
    )
    {
        if (strchr (sz, '.'))
            continue ;

        DWORD dw = UnDecorateSymbolName (sz, tank, sizeof(tank), 
            UNDNAME_NAME_ONLY | UNDNAME_COMPLETE | UNDNAME_NO_LEADING_UNDERSCORES) ;
        k = flocons ( 1.0 ) ;

        zed = GetProcAddress (h, sz) ;

        if (zed == 0)
            f = llast_win32_errmsg (0) ;
        else
            f = flocons ( (double) (DWORD) zed ) ;

        if (lstrlenA(tank))
            g = rintern (tank);
        else
            g = rintern (sz) ;

        lout = cons ( cons (g, cons (f, k)), lout) ;
    }

    pea.Free (rgszExports);

    return lout ;
}

//================================================================================
// The following are experimental
//================================================================================

LISP SiodGetSystemDirectory ()
{
	char buf [MAX_PATH + 1] ;
	memset (buf, 0, MAX_PATH+1) ;
	UINT ui = GetSystemDirectory (buf,  MAX_PATH) ;
	if (0 == ui)
		FormatMessage (0x1000, 0, GetLastError (), 0, buf, MAX_PATH, 0) ;
	return strcons (-1, buf) ;
}

LISP newVariant (LISP vtype, LISP vval)
{
	LISP z = 0 ;

	if (NFLONUMP(vtype))
		return err ("wta newVariant", vtype) ;

	if (NFLONUMP(vval))
		return err ("wta newVariant", vval) ;

	VARIANT * pVar = (VARIANT*)LocalAlloc (0x40, sizeof(VARIANT)) ;
	
	if (0 == pVar)
		return err ("out of memory", z) ;

	z = flocons ((double)(DWORD)pVar) ;

	pVar -> vt = (VARTYPE)FLONM(vtype) ;
	switch (pVar -> vt)
	{
	case VT_I4:
		pVar -> lVal = (LONG)FLONM(vval) ;
		break ;

	case VT_NULL:
		pVar -> lVal = 0 ;
		break ;

	default:
		LocalFree (pVar) ;
		z = err ("unsupported VARIANT VTYPE", vtype) ;
		break ;
	}

	return z ;
}


//================================================================
// CPTR ops
//================================================================

#define CHKTYP(val,f) {if (NCPTRP(val)) err ("wta to " #f, val) ;}

LISP FlonumFromCPtr (LISP cp)
{
    CHKTYP(cp, "cptr->num") ;
    return flocons ((unsigned long)(CPTR(cp))) ;
}

LISP LispFromFlonum (LISP flo)
{
    LISP z ;
    if (NFLONUMP(flo)) 
		err ("wta to num->cptr", flo) ;
    z = (LISP)(unsigned long)(FLONM(flo)) ;
    return z ;
}

LISP FlonumFromLisp (LISP l)
{
	return flocons ((unsigned long)l) ;
}

LISP CPtrFromFlonum (LISP flo)
{
    LISP z ;
    if (NFLONUMP(flo)) err ("wta to num->cptr", flo) ;
    z = newcell (tc_cptr) ;
    CPTR(z) = (void*)(unsigned long)(FLONM(flo)) ;
    return z ;
}

LISP CPtrAddressOfCPtr (LISP cp)
{
    LISP z ;
    CHKTYP(cp, "&") ;
    z = newcell (tc_cptr) ;
    CPTR(z) = & CPTR(cp) ;
    return z ;
}

LISP CPtrIndirectCPtr (LISP cp)
{
    LISP z ;

    z = newcell (tc_cptr) ;
    
	if (CPTR(cp))
        CPTR(z) = (* ((void**)CPTR(cp))) ;
    else
        CPTR(z) = 0 ;
    return z ;
}

LISP NumIndirectNum (LISP num)
{
	LISP z ;
	unsigned long * pdw ;

	//
	// don't use flocons -- it sometimes returns internal
	// numbers (inum[xxx]) to save space
	//

	z = newcell (tc_flonum) ;  
	FLONM(z) = 0.0 ;

	pdw = (unsigned long *)(unsigned long)FLONM(num) ;
	if (pdw)
		FLONM(z) = (double)(*pdw) ;

	return z ;
}

LISP indirect (LISP thing)
{
	switch (TYPE(thing))
	{
	case tc_flonum:
		return NumIndirectNum (thing) ;

	case tc_cptr:
		return CPtrIndirectCPtr (thing) ;

	default:
		return err ("wta indirect", thing) ;
	}
}

LISP pokeDw (LISP destAddr, LISP ldw)
{
	unsigned long * pdw ;
	unsigned long   dw  ;

	if (TYPE (ldw) == tc_flonum)
		dw = (unsigned long) FLONM (ldw) ;
	else
		return err ("wta poke-dw", ldw) ;

	switch (TYPE(destAddr))
	{
	case tc_flonum:
		pdw = (unsigned long *)(unsigned long)FLONM(destAddr) ;
		break ;

	case tc_cptr:
		pdw = (unsigned long *) CPTR (destAddr) ;
		break ;

	default:
		return err ("wta poke-dw", destAddr) ;
	}

	*pdw = dw ;

	return destAddr ;
}

LISP peekByte (LISP addr)
{
	LISP z ;
	unsigned char * pb ;

	switch (TYPE(addr))
	{
	case tc_flonum:
		//
		// don't use flocons -- it sometimes returns internal
		// numbers (inum[xxx]) to save space
		//

		z = newcell (tc_flonum) ;  
		FLONM(z) = 0.0 ;

		pb = (unsigned char *)(unsigned long)FLONM(addr) ;
		if (pb)
			FLONM(z) = (double)(*pb) ;

		return z ;

	default:
		return err ("wta peek-byte", addr) ;
	}
}

LISP pokeByte (LISP destAddr, LISP lb)
{
	unsigned char * pb ;
	unsigned char   b  ;

	if (TYPE (lb) == tc_flonum)
		b = (unsigned char) FLONM (lb) ;
	else
		return err ("wta poke-byte", lb) ;

	switch (TYPE(destAddr))
	{
	case tc_flonum:
		pb = (unsigned char *)(unsigned long)FLONM(destAddr) ;
		break ;

	case tc_cptr:
		pb = (unsigned char *) CPTR (destAddr) ;
		break ;

	default:
		return err ("wta poke-byte", destAddr) ;
	}

	*pb = b ;

	return destAddr ;
}

typedef unsigned short int WORD ;

LISP peekWord (LISP addr)
{
	LISP z ;
	WORD * pw ;

	switch (TYPE(addr))
	{
	case tc_flonum:
		//
		// don't use flocons -- it sometimes returns internal
		// numbers (inum[xxx]) to save space
		//

		z = newcell (tc_flonum) ;  
		FLONM(z) = 0.0 ;

		pw = (WORD*)(unsigned long)FLONM(addr) ;
		if (pw)
			FLONM(z) = (double)(*pw) ;

		return z ;

	default:
		return err ("wta peek-word", addr) ;
	}
}

LISP pokeWord (LISP destAddr, LISP lb)
{
	WORD * pw ;
	WORD   w  ;

	if (TYPE (lb) == tc_flonum)
		w = (WORD) FLONM (lb) ;
	else
		return err ("wta poke-word", lb) ;

	switch (TYPE(destAddr))
	{
	case tc_flonum:
		pw = (WORD*)(unsigned long)FLONM(destAddr) ;
		break ;

	case tc_cptr:
		pw = (WORD*) CPTR (destAddr) ;
		break ;

	default:
		return err ("wta poke-word", destAddr) ;
	}

	*pw = w ;

	return destAddr ;
}

LISP conscptr (void* d)
{
    LISP z ;
    z = newcell (tc_cptr) ;
    CPTR(z) = d ;
    return z ;
}

LISP newcptr (void)
{
    return conscptr (0) ;
}

#undef CHKTYP

LISP bitand(LISP a,LISP b)  {return (flocons(get_c_long(a) & get_c_long(b)));}
LISP bitor (LISP a,LISP b)  {return (flocons(get_c_long(a) | get_c_long(b)));}
LISP bitxor(LISP a,LISP b)  {return (flocons(get_c_long(a) ^ get_c_long(b)));}
LISP bitnot(LISP a)         {return (flocons(~get_c_long(a)));               }
LISP bitshl(LISP a,LISP b)  {return (flocons(get_c_long(a) << get_c_long(b)));}
LISP bitshr(LISP a,LISP b)  {return (flocons(get_c_long(a) >> get_c_long(b)));}



//================================================================================
// The following is required by SIODs so-loader
//================================================================================

static void SetSubrs(void)
{
    init_subr_1 ("GetProcAlist",          GetProcAlist           ) ;
    init_subr_1 ("GetProcAlistCDecls",    GetProcAlistCDecls     ) ;
    init_subr_2 ("MainDispatch",          MainDispatcher         ) ;
    init_subr_2 ("MainDispatchCDecl",     MainDispatcherCDecls   ) ;
    init_subr_1 ("cstring->string",       StringFromCString      ) ;
    init_subr_1 ("string->cstring",       CStringFromString      ) ;

    init_subr_1 ("array->rgpv",           RgPvFromLispArray      ) ;
    init_subr_2 ("rgpv->array",           LispArrayFromRgPv      ) ;

    init_subr_1 ("array->cbytes",         CByteArrayFromArray    ) ;
    init_subr_1 ("array->carray",         CByteArrayFromArray    ) ;
    init_subr_2 ("cbytes->array",         ByteArrayFromCByteArray) ;
    init_subr_3 ("carray->array",         SiodArrayFromCByteArray) ;

    init_subr_0 ("ScmGetSystemDirectory", SiodGetSystemDirectory ) ;
    init_subr_0 ("sgsd",                  SiodGetSystemDirectory ) ;

    init_subr_2 ("newVariant",            newVariant             ) ;

    init_subr_1 ("callback",              SiodCallback           ) ;
    
    //init_subr_3 ("pokeByte",           PokeByte                ) ;
    //init_subr_2 ("peekByte",           PeekByte                ) ;


	init_subr_1 ("num->lisp",          LispFromFlonum    ) ;
	init_subr_1 ("lisp->num",          FlonumFromLisp    ) ;
    init_subr_1 ("cptr->num",          FlonumFromCPtr    ) ;
    init_subr_1 ("num->cptr",          CPtrFromFlonum    ) ;
    init_subr_1 ("address-of",         CPtrAddressOfCPtr ) ;
    init_subr_0 ("cptr",               newcptr           ) ;
	init_subr_1 ("val",                indirect          ) ;
    init_subr_1 ("indirect",           indirect          ) ;
	init_subr_1 ("peek-dw",            indirect          ) ;
	init_subr_2 ("poke-dw",            pokeDw            ) ;
	init_subr_1 ("peek-dword",         indirect          ) ;
	init_subr_2 ("poke-dword",         pokeDw            ) ;
	init_subr_1 ("peek-byte",          peekByte          ) ;
	init_subr_2 ("poke-byte",          pokeByte          ) ;
	init_subr_1 ("peek-word",          peekWord          ) ;
	init_subr_2 ("poke-word",          pokeWord          ) ;


	init_subr_2("&",                  bitand);
    init_subr_2("|",                  bitor);
    init_subr_2("^",                  bitxor);
    init_subr_1("~",                  bitnot);
	init_subr_2("<<",                 bitshl);
	init_subr_2(">>",                 bitshr);



}

extern "C"
{
    __declspec (dllexport) void init_RealLoad(void)
    {
        SetSubrs () ;
    }
} // extern "C"


