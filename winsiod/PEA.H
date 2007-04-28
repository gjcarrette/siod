/*
 From Brian Beckman's siodffi.zip distribution.
*/

    //================================================================
   //
  // PEA.h -- PE File Analyzer
 //
//================================================================

#ifndef __PEA_H__
#define __PEA_H__

#include "bblutil.h"
#include "PageData.h"
#include "ars.h"

#define SIZE_OF_NT_SIGNATURE	sizeof (DWORD)
#define MAXRESOURCENAME 	  13

// global macros to define header offsets into file 
// offset to PE file signature				        
#define NTSIGNATURE(a) ((LPVOID)((BYTE *)a	    + \
			((PIMAGE_DOS_HEADER)a)->e_lfanew))

// DOS header identifies the NT PEFile signature dword
//   the PEFILE header exists just after that dword	      
#define PEFHDROFFSET(a) ((LPVOID)((BYTE *)a		+ \
			 ((PIMAGE_DOS_HEADER)a)->e_lfanew   +  \
			 SIZE_OF_NT_SIGNATURE))

// PE optional header is immediately after PEFile header
#define OPTHDROFFSET(a) ((LPVOID)((BYTE *)a		+ \
			 ((PIMAGE_DOS_HEADER)a)->e_lfanew   +  \
			 SIZE_OF_NT_SIGNATURE		        +	\
			 sizeof (IMAGE_FILE_HEADER)))

// section headers are immediately after PE optional header
#define SECHDROFFSET(a) ((LPVOID)((BYTE *)a	    + \
			 ((PIMAGE_DOS_HEADER)a)->e_lfanew   +  \
			 SIZE_OF_NT_SIGNATURE				+	\
			 sizeof (IMAGE_FILE_HEADER)			+    \
			 sizeof (IMAGE_OPTIONAL_HEADER)))




//
// Make some Three-Letter Acronyms (TLAs) for data-directory
// types in WINNT.H (as needed, rather than altogether)
//
typedef IMAGE_IMPORT_DESCRIPTOR IDD, *PIDD ;	// Same as Import Data Directory
typedef IMAGE_THUNK_DATA        IMPORT_LOOKUP_TABLE, *PIMPORT_LOOKUP_TABLE, ILT, *PILT ;
typedef IMAGE_THUNK_DATA        IMPORT_ADDRESS_TABLE, *PIMPORT_ADDRESS_TABLE, IAT, *PIAT ;
typedef IMAGE_SECTION_HEADER    ISH, *PISH;

//
// The following is a "fixup" directory
//
typedef IMAGE_BASE_RELOCATION   IFX, *PIFX ;    // Entry in BASERELOC Data Directory


TYPEDEF (FIXUP, FXP)

struct s_FIXUP
{
	union
	{
		struct
		{
			WORD    wPageOffset : 12 ;
			WORD	wType       :  4;
		} ;

		WORD wRaw ;
	};
} ;

#define MAX_IMPORTED_MODULES	256
#define MAX_UNKNOWN_SECTIONS	 64


TYPEDEF (PEFILE_ANALYSIS, PEA)

struct s_PEFILE_ANALYSIS
{
	//
	// Member variables
	//
private:
	//
	// File mapping support
	//
	HANDLE					m_hHeap;

    HANDLE					m_hFile;
    HANDLE  				m_hMMFile;
    LPVOID					m_lpMappedFile;
	PIMAGE_OPTIONAL_HEADER  m_poh;
	PIMAGE_SECTION_HEADER   m_textSection;
	PIMAGE_SECTION_HEADER   m_dataSection;
	PIMAGE_SECTION_HEADER   m_rdataSection;

	//
	// Post-loading support
	//
private:
public:
	PARS    m_parsLoadMapComplement;
	ARS     m_arsActualLoadMap;
	BOOL    m_fBreakForData;
	
private:
	LPBYTE  m_pbBase;
	LPBYTE  m_pbText;
	LPBYTE  m_pbRdata;
	LPBYTE  m_pbData;
	LPBYTE  m_pbIdata;
	LPBYTE  m_pbEdata;
	LPBYTE  m_pbReloc;
	LPBYTE  m_pbDebug;
	LPBYTE  m_pbTls;
	LPBYTE  m_pbRsrc;

	DWORD	m_cUnknownSections;
	LPBYTE  * m_rgpbUnknownSections;		// Allocate on the heap

	//
	// management overhead
	//
public:
	inline HANDLE   HHeap                          ()
	{
		return m_hHeap;
	}

	inline HANDLE   HPseudoInstance				   ()
	{
		return m_hMMFile;
	}
	
    s_PEFILE_ANALYSIS							   () {Init(0);}
    ~s_PEFILE_ANALYSIS							   () {CloseModule();}

//
// Called routines allocate documentation buffers to return to 
// callers, so the Alloc routine should be private.
//
private:
	inline  PVOID   Alloc                          (DWORD cb)
	{	
		PVOID pv = HeapAlloc (m_hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, cb);
		//__asm {int 3}
		return pv;
	}

//
// Callers must free documentation strings returned by a PEA, so
// the Free routine should be public.
//
public:
	inline  VOID    Free						   (PVOID pv)	
	{ 
		//__asm {int 3}
		if (0 == HeapFree (m_hHeap, 0, pv))
			SimpleException(SE_HEAPFREE);  
	}

	//
	// Bobble Execution
	//
private:
	outline void    ComputeComplement              ();
	outline void    RestrictComplement             ();
	outline void    UnrestrictComplement           ();
public:
	outline void    SetPermissions				   ();
	outline void    ResetPermissions               ();
	outline DWORD   DwCallBobbleEntryPoint         ();

	//
	// semantics: Image Management
	//
public:
	outline void    Init                           (HANDLE hHeap);
	outline void    OpenModule                     (LPCSTR);
	outline void    SectionAddData                 (E_PAGETYPE, DWORD dwBase, DWORD dwCPg, DWORD dwProt);
	outline BOOL    FScatterFile                   ();
	outline void    CloseModule                    ();

	//
	// PE file header info 
	//
public:
	outline BOOL	GetDosHeader                   (PIMAGE_DOS_HEADER);
	outline DWORD	ImageFileType                  ();
	outline BOOL	GetPEFileHeader                (PIMAGE_FILE_HEADER);

	//
	// PE optional header info
	//
public:
	outline BOOL	GetPEOptionalHeader            (PIMAGE_OPTIONAL_HEADER);
	outline LPVOID	GetModuleEntryPoint            ();
	outline DWORD   NumOfSections                  ();
	outline LPVOID	GetImageBase                   ();
	outline LPVOID	ImageDirectoryPointer          (PIMAGE_SECTION_HEADER, DWORD);

	//
	// PE section header info 
	//
private:
	outline VOID    AddUnknownSection              (IMAGE_SECTION_HEADER & sh);
	inline  PVOID   PvFromRVABiasedToSection       (DWORD RVA, PISH pish);
public:
	outline DWORD   GetSectionNames                (char **);
	outline BOOL	GetSectionHdrByName            (PIMAGE_SECTION_HEADER, char *);
	outline PISH    PISHSectionHdrByName           (char *);
	outline BOOL	GetSectionHdrByIndex		   (PIMAGE_SECTION_HEADER, DWORD);

	//
	// export section info 
	//
public:
	outline BOOL    GetImageExportDirectory        (PIMAGE_EXPORT_DIRECTORY);
	outline DWORD   NumberOfExportedFunctions      ();
	outline DWORD   GetExportFunctionNames         (char **);
	outline PDWORD  GetExportFunctionEntryPoints   ();
	outline PWORD 	GetExportFunctionOrdinals      ();

	//
	// import section info 
	//
	// (the Import Directory is a.k.a. the Import Descriptor)
	//
public:
	outline PIDD    PIDDFirstImageImportDirectory  (PIMAGE_SECTION_HEADER);
	outline VOID    FetchImportsFromModule         (char*, PIDD, PIDD, ISH&);
	outline VOID    FetchImports                   ();
	outline DWORD   GetImportModuleNames           (char **);
	outline DWORD   GetImportFunctionNamesByModule (char *, char  **);

	//
	// reloc (fixup) section info
	//
	// Use nested, stateless enumerators to access Fixup information, as in
	//
	//	PIFX pifx;
	//	PFXP pfxp;
	//
	//	for 
	//	(
	//		pifx = pea.PIFXFirstFixupDirectory(NULL);
	//	    pifx;
	//		pifx = pea.PIFXNextFixupDirectory(pifx)
	//	)
	//	{
	//		DumpFixupDirectory (pifx);
	//		for
	//		(
	//			pfxp = pea.PFXPFirstFixup (pifx);
	//			pfxp;
	//			pfxp = pea.PFXPNextFixup (pifx, pfxp)
	//		)
	//		{
	//			DumpFixup (pfxp);
	//		}
	//	}
	//
public:
	outline DWORD   PreferredLoadBase              ();
	outline int     Delta                          ();
	outline DWORD   ActualLoadBase                 ();
	outline LPVOID  PvRebaseCodeAddress            (LPVOID);

	outline	DWORD   ActualFixupAddress             (PIFX, PFXP);
	outline DWORD   DataPriorToFixup               (PIFX, PFXP);
	outline DWORD   DataPostFixup                  (PIFX, PFXP, BOOL fWrite);
	outline DWORD   DwRvaFixup					   (PIFX, PFXP);
	outline BOOL	FApplyAllFixups                ();
	outline BOOL    FIsFixupInSection              (PISH, PIFX, PFXP);
	outline BOOL    FIsCodeFixup                   (PIFX, PFXP);
	outline BOOL    FIsDataFixup                   (PIFX, PFXP);
	outline BOOL    FIsRdataFixup                  (PIFX, PFXP);
	outline PISH    PISHSectionContainingFixup     (PIFX, PFXP);

	outline DWORD   CFixupDirectories              ();
	outline DWORD   CFixups                        (PIFX); 
	
	outline PIFX    PIFXFirstFixupDirectory        (PIMAGE_SECTION_HEADER);
	outline PFXP    PFXPFirstFixup                 (PIFX);
	outline PFXP    PFXPNextFixup                  (PIFX, PFXP);
	outline PIFX    PIFXNextFixupDirectory         (PIFX);

	//
	// resource section info 
	//
public:
	outline DWORD   NumberOfResources              ();
	outline DWORD   GetListOfResourceTypes         (char **);

	//
	// debug section info 
	//
public:
	outline BOOL	IsDebugInfoStripped            ();
	outline DWORD   RetrieveModuleName             (char **);
	outline BOOL	IsDebugFile                    ();
	outline BOOL	GetSeparateDebugHeader         (PIMAGE_SEPARATE_DEBUG_HEADER);
};



#endif // __PEA_H__
