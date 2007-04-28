/*
 From Brian Beckman's siodffi.zip distribution.
*/
    //================================================================
   //
  // BBLUtil.h -- generic stuff for bobble source
 //
//================================================================

#ifndef __BBLUTIL_H__
#define __BBLUTIL_H__

#define IRRELEVANT 2 // for ternary logic (neither TRUE nor FALSE)

#include "warning.h"

    //----------------------------------------------------------------
   //
  // Math macros
 //
//----------------------------------------------------------------
#define MIN(x,y) (((x) < (y)) ? (x) : (y))
#define MAX(x,y) (((x) > (y)) ? (x) : (y))

inline  DWORD DwRoundUp (DWORD dw, int granule) 
{
    return ((dw) + (granule - 1)) & (-granule);
}

inline  DWORD DwSpanCount (DWORD dw, int granule)
{
    return DwRoundUp(dw, granule) / granule ;
}

inline  VOID AttemptAccess (DWORD dw)
{
    PBYTE pb = (PBYTE)dw ;
    BYTE   b = *pb;
}

    //----------------------------------------------------------------
   //
  // Exception codes for these ADTs
 //
//----------------------------------------------------------------
//
// I borrowed some of these from Jeffrey Richter.  
//
#define EXCEPTION_CODE(Severity, Facility, Exception) \
   ((DWORD) (                                          \
   /* Severity code    */  (Severity       ) |          \
   /* MS(0) or Cust(1) */  (1         << 29) |           \
   /* Reserved(0)      */  (0         << 28) |            \
   /* Facility code    */  (Facility  << 16) |             \
   /* Exception code   */  (Exception <<  0)   ))

#define ECODE(num) EXCEPTION_CODE(ERROR_SEVERITY_ERROR, FACILITY_NULL, num)

#define SE_ZERO_EXPECTED              ECODE( 1)
#define SE_ADDRESS_WRAP               ECODE( 2)
#define SE_ARS_OVERLAP                ECODE( 3)
#define SE_INVALID_INPUT              ECODE( 4)
#define SE_NYI                        ECODE( 5)
#define SE_CANT_HAPPEN                ECODE( 6)
#define SE_MALFORMED_SET              ECODE( 7)
#define SE_RESERVE_PAGETABLES         ECODE( 8)
#define SE_COMMIT_PDIR                ECODE( 9)
#define SE_COMMIT_PTBL                ECODE(10)
#define SE_VIRTUAL_FREE               ECODE(11)
#define SE_PTE_NONNULL                ECODE(12)
#define SE_INTERNAL_PERMISSION        ECODE(13)
#define SE_PERMISSION_MUTEX           ECODE(14)
#define SE_EXCEPTION_ARCHITECTURE     ECODE(15)
#define SE_OPEN_MODULE                ECODE(16)
#define SE_CREATE_FILE_MAPPING        ECODE(17)
#define SE_MAP_VIEW_OF_MODULE         ECODE(18)
#define SE_NO_EXPORTS                 ECODE(19)
#define SE_HEAPFREE                   ECODE(20)
#define SE_TOO_MANY_MODULES           ECODE(21)
#define SE_MISSING_IMPORT_NAME        ECODE(22)
#define SE_OVERFLOW_STACKVAR          ECODE(23)
#define SE_UNSUPPORTED_FIXUP_TYPE     ECODE(24)
#define SE_TEXT_EXECUTE_PERMISSION    ECODE(25)
#define SE_DATA_SECTION_PERMISSION    ECODE(26)
#define SE_RDATA_SECTION_PERMISSION   ECODE(27)
#define SE_GETFILESIZE                ECODE(28)
#define SE_MALFORMED_SECTION_HEADERS  ECODE(29)
#define SE_OUTOFMEMORY                ECODE(30)
#define SE_NO_BASE_MODULE             ECODE(31)
#define SE_NO_PROC_ADDRESS_BY_NAME    ECODE(32)
#define SE_NO_PROC_ADDRESS_BY_ORDINAL ECODE(33)
#define SE_TOO_MANY_UNKNOWN_SECTIONS  ECODE(34)
#define SE_ADDRESS_RANGE_PERMISSION   ECODE(35)
#define SE_VIRTUAL_QUERY              ECODE(36)
#define SE_NO_MODULES_LOADED          ECODE(37)
#define SE_TOO_MANY_BOBBLES           ECODE(38)
#define SE_MISSING_COMPLEMENT         ECODE(39)
#define SE_LIBRARY_NOT_FOUND          ECODE(40)
#define SE_BBL_PAGE_DATA              ECODE(41)

inline void SimpleException(DWORD code) {RaiseException(code, EXCEPTION_NONCONTINUABLE, 0, NULL);}

//
// The following works only if defined exactly this way (and I don't know why).  
// Use it as in 
//
// #pragma UNDONE(blah blah blah)
//
#define STRINGIZE(x) #x
#define STRINGIZE2(x) STRINGIZE(x)
#define UNDONE(desc) message("UNDONE: " __FILE__ "(" STRINGIZE2(__LINE__) "):" STRINGIZE(desc))

    //----------------------------------------------------------------
   //
  // Hardware stuff
 //
//----------------------------------------------------------------
#if (PROCESSOR_ARCHITECTURE == x86)
const DWORD g_cbPageSize      = 4096;
const DWORD g_dwPageMask      = g_cbPageSize - 1;
const DWORD g_cPagesFullSpace = 1024 * 1024;
#else
#pragma UNDONE(Page data for non-x86 architectures)
#endif

    //----------------------------------------------------------------
   //             
  // MODULE STUFF
 //
//----------------------------------------------------------------
#pragma warning(disable:4229)
typedef BOOL (* WINAPI PFN_DLLMAIN)(HANDLE, ULONG, LPVOID) ; //DLL_PROCESS_ATTACH
#pragma warning(default:4229)

      //----------------------------------------------------------------
     //
    // TYPEDEF -- a macro for defining the long name and three-letter
   //             acronyms (tlas) for a given struct tag and its 
  //              common derivative pointer types.
 //
//----------------------------------------------------------------
#define TYPEDEF(longname, tla)                         \
    typedef struct s_##longname                         \
        longname, *P##longname, tla, *P##tla, *LP##tla;  \
    typedef struct s_##longname CONST *LPC##tla;          \
    typedef struct s_##longname CONST *PC##tla;            \
    typedef struct s_##longname CONST  C##tla;

//
// Some TLAs
//

typedef MEMORY_BASIC_INFORMATION MBI ;

    //----------------------------------------------------------------
   //             
  // MISCELLANEOUS
 //
//----------------------------------------------------------------

#define outline // opposite of inline? -- means look in the cpp file for definition.

inline void DbgErrorBox (DWORD error)
{
#ifdef _DEBUG
	char * pszMessage;                                            
    FormatMessage                                                  
    (   FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
        0, error, 0, (LPSTR)(&pszMessage), 0, 0);          
    MessageBox (NULL, pszMessage, "Error", MB_OK);                  
    LocalFree (pszMessage);   
#else
	error += 0 ;
#endif
}

inline void DbgLastErrorBox ()
{
	DbgErrorBox (GetLastError ()) ;
}

//
// Zero out a structure. If fInitSize is TRUE, initialize the first int to
// the size of the structure. Many structures like WNDCLASSEX and STARTUPINFO
// require that their first member be set to the size of the structure itself.
//
// DON'T USE THIS ON ANYTHING THAT HAS A VTABLE  !!! ! !  !  ! ! !!!
//
#define ZERO_STRUCT(S, F)               \
   (ZeroMemory(&(S), sizeof(S)), F ? (((DWORD*) &(S))[0] = sizeof(S)) : 0)


#define C_ARRAY_ELTS(A) (sizeof(A) / sizeof(A[0]))


#endif // __BBLUTIL_H__
