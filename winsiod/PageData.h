/*
 From Brian Beckman's siodffi.zip distribution.
*/
#ifndef __BBL_PAGEDATA__
#define __BBL_PAGEDATA__

typedef enum E_PAGETYPE
{
	NO_PAGETYPE =0,
	HOST_PAGE,
	// 
	// The following are SUSPECT pages in the Bobble
	//
	CODE_PAGE,
	STACK_PAGE,
	HEAP_PAGE,
	IDATA_PAGE,
	EDATA_PAGE,
	RDATA_PAGE,
	DATA_PAGE,
	RSRC_PAGE,
	RELOC_PAGE,
	TLS_PAGE,
	DEBUG_PAGE,
	OTHER_PAGE
} ;

//
// The following struct tracks what the kernel may do with our requests to 
// protect and unprotect pages.  It can be inspected in the debugger or 
// printed to output.
//

TYPEDEF(BBL_PAGEDATA, BPD)

struct s_BBL_PAGEDATA
{
public:
	E_PAGETYPE ePageType ;

	DWORD      dwBaseRequested ;		// input
	DWORD      dwBaseActual ;			// Discovered in SetPermission
	DWORD      cPgRequested ;			// input
	DWORD      cPgActual ;				// Discovered in SetPermission
	DWORD      dwProtectionRequested ;  // input
	DWORD      dwProtectionOriginal ;   // Discovered in SetPermission
	DWORD      dwProtectionActual ;     // Discovered in ResetPermission

	DWORD      dwAllocationBase ;
	DWORD      dwMemType ;
	DWORD      dwMemState ;

	BOOL       fProtectionSuccessful ;  
	BOOL       fUnprotectionSuccessful ;
	BOOL       fSizeChangedWhileProtected ;
	BOOL       fSizeChangedWhileUnprotected ;
	BOOL	   fBaseChangedWhileProtected ;
	BOOL	   fBaseChangedWhileUnprotected ;

public:

	inline  void Init  
	(
		E_PAGETYPE i_ePageType,
		DWORD      i_dwBaseRequested,
		DWORD      i_cPgRequested,
		DWORD      i_dwProtectionRequested
	)
	{
		ePageType               = i_ePageType             ;
		dwBaseRequested         = i_dwBaseRequested       ;
		dwBaseActual            = 0                       ;
		cPgRequested            = i_cPgRequested          ;
		cPgActual               = 0                       ;
		dwProtectionRequested   = i_dwProtectionRequested ;
		dwProtectionOriginal    = 0                       ;
		dwProtectionActual      = 0                       ;

		dwAllocationBase        = 0 ;
		dwMemType               = 0 ;
		dwMemState              = 0 ;

		fProtectionSuccessful        = IRRELEVANT ;
		fUnprotectionSuccessful      = IRRELEVANT ;
		fSizeChangedWhileProtected   = IRRELEVANT ;
		fSizeChangedWhileUnprotected = IRRELEVANT ;
		fBaseChangedWhileProtected   = IRRELEVANT ;
		fBaseChangedWhileUnprotected = IRRELEVANT ;
	}
} ;

//
//  Unfortunately, because we must call the bobble switcher under Structured Exception
//  Handling (SEH), and because SEH is not compatible with C++ destructors (for a very
//  (good reason), we must manage the memory devoted to instances of BOBBLE_PAGE_DATA
//  explicitly.  To make things worse, ADDRESS_RANGE_SET (ARG) has a slot for an 
//  Attributes pointer where we would like transparently to store a pointer to a BPD.
//  Transparently means that ARG gets to control the lifetime of its Attribute slot, 
//  that is, gets to call HeapFree on it, without having to know what type it is.  The
//  clean way to do this is with virtual destructors, but they don't work under SEH.
//  Sooooooo... (take a deep breath)  we can't use a chained data structure, because
//  ARG doesn't know to HeapFree recursively, because it shouldn't know the type of 
//  its Attributes slot.  So, we're going to use a clumsy array of BOBBLE_PAGE_DATAs
//  and HeapReAlloc it as we need to grow it.  Whew.  That's why the following is the
//  way it is...

TYPEDEF(BBL_ARG_ATTRIBUTES, BAA)

struct s_BBL_ARG_ATTRIBUTES
{
public:    
    DWORD  dwProtectionRequested ;
    DWORD  cBPDs     ;
    BPD    rgBPDs[0] ;

public:
    inline PBPD   PBPDLastBPD () { return (cBPDs > 0) ? (& (rgBPDs[cBPDs - 1])) : 0 ; }
} ;

inline PBAA PBAABumpRgBPDs (PBAA that, HANDLE hHeap)
{
	PBAA r = 0 ;

	__try
	{
		r = (PBAA)HeapReAlloc
		(
			hHeap,
			HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY,
			(PVOID) that,
			sizeof (BAA) + (that->cBPDs + 1) * sizeof (BPD)
		) ;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DbgLastErrorBox () ;
		SimpleException (SE_BBL_PAGE_DATA) ;
	}

    r->cBPDs ++ ;

    return r ;
}
                          

#endif // __BBL_PAGEDATA__
