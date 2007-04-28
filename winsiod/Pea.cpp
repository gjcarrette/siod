/*
 From Brian Beckman's siodffi.zip distribution.
*/
    //================================================================
   //
  // PEA.cpp -- implementation of PE File Analyzer
 //
//================================================================
#include <windows.h>
#include <crtdbg.h>

#include "PEA.h"


void PEA::Init(HANDLE hHeap)
{
    m_hHeap = hHeap;
    m_arsActualLoadMap.Init (m_hHeap);

    m_parsLoadMapComplement = 0;

    m_fBreakForData = FALSE;

    m_hFile        = 0;
    m_hMMFile      = 0;
    m_lpMappedFile = 0;
    m_poh          = 0;
    m_textSection  = 0;
    m_dataSection  = 0;
    m_rdataSection = 0;

    m_pbBase  = 0;
    m_pbText  = 0;
    m_pbRdata = 0;
    m_pbData  = 0;
    m_pbIdata = 0;
    m_pbEdata = 0;
    m_pbReloc = 0;
    m_pbDebug = 0;
    m_pbTls   = 0;
    m_pbRsrc  = 0;

    m_cUnknownSections    = 0;
    m_rgpbUnknownSections = 0;
}

//--------------------------------------------------------------------------------
//
//  $$   $$   $$    $$$$$$        $$$$  $$   $$   $$      $$$$  $$$$$$$
//  $$$ $$$  $$$$    $$  $$        $$   $$$ $$$  $$$$    $$  $$  $$   $
//  $$$$$$$ $$  $$   $$  $$        $$   $$$$$$$ $$  $$  $$       $$ $
//  $$$$$$$ $$  $$   $$$$$         $$   $$$$$$$ $$  $$  $$       $$$$
//  $$ $ $$ $$$$$$   $$            $$   $$ $ $$ $$$$$$  $$  $$$  $$ $
//  $$   $$ $$  $$   $$            $$   $$   $$ $$  $$   $$  $$  $$   $
//  $$   $$ $$  $$  $$$$          $$$$  $$   $$ $$  $$    $$$$$ $$$$$$$
//
//--------------------------------------------------------------------------------

void PEA::OpenModule (LPCSTR wszFileName)
{
    /** /
    SYSTEM_INFO si;
    GetSystemInfo (&si);

    /**/
    m_hFile = CreateFileA
    (
        wszFileName,                    // pointer to name of the file
        GENERIC_READ,                   // access (read-write) mode
        FILE_SHARE_READ,                // share mode
        NULL,                           // pointer to security attributes
        OPEN_EXISTING,                  // how to create
        FILE_ATTRIBUTE_NORMAL,          // file attributes
        NULL                            // handle to file with attributes to copy
    );

#pragma UNDONE(cant handle hyper big files - those with sizes bigger than 2G)

    if (INVALID_HANDLE_VALUE == m_hFile)
    {
        DbgLastErrorBox () ;
        SimpleException (SE_OPEN_MODULE) ;
    }

    m_hMMFile = CreateFileMapping
    (
        m_hFile,                        // handle to file to map
        NULL,                           // optional security attributes
//      PAGE_READONLY,                  // only permits a copy to be made
        PAGE_WRITECOPY,                 // allows fixups to be applied, but not execution
                                        //     also, this is necessary for Win95
//      PAGE_WRITECOPY | SEC_IMAGE,     // applies fixups and resolved imports and exports,
                                        //     in other words, does FAR too much...
        0,                              // high-order 32 bits of object size
        0,                              // low-order 32 bits of object size
        NULL                            // name of file-mapping object
    ) ;


    if (m_hMMFile == NULL)
    {
        DbgLastErrorBox () ;
        SimpleException (SE_CREATE_FILE_MAPPING) ;
    }


    BYTE * pbFileView = (PBYTE)MapViewOfFile
    (
        m_hMMFile,                      // file-mapping object to map into address space
        FILE_MAP_COPY,                  // access mode
        0,                              // high-order 32 bits of file offset
        0,                              // low-order 32 bits of file offset
        0                               // number of bytes to map
    );

    m_lpMappedFile = (LPVOID)pbFileView;

    if (m_lpMappedFile == NULL)
    {
        DbgLastErrorBox () ;
        SimpleException (SE_MAP_VIEW_OF_MODULE) ;
    }

    m_poh = (PIMAGE_OPTIONAL_HEADER) OPTHDROFFSET (m_lpMappedFile);
    m_textSection  = (PIMAGE_SECTION_HEADER) PISHSectionHdrByName(".text");
    m_rdataSection = (PIMAGE_SECTION_HEADER) PISHSectionHdrByName(".rdata");
    m_dataSection  = (PIMAGE_SECTION_HEADER) PISHSectionHdrByName(".data");
}

void PEA::CloseModule ()
{
    if (m_lpMappedFile)
        UnmapViewOfFile (m_lpMappedFile);

    if (m_hFile)
        CloseHandle (m_hFile);

    if (m_hMMFile)
        CloseHandle (m_hMMFile);

    if (m_rgpbUnknownSections)
        Free (m_rgpbUnknownSections);

    m_arsActualLoadMap.Clear (m_hHeap);

    if (m_parsLoadMapComplement)
    {
        m_parsLoadMapComplement->Clear();
        Free (m_parsLoadMapComplement) ;
    }


    Init(0);
}

inline BOOL FStrEq(unsigned char s1[8], CHAR* s2)
{
    return 2 == CompareStringA(LOCALE_SYSTEM_DEFAULT, 0, (const CHAR*)s1, 8, s2, -1);
}

//--------------------------------------------------------------------------------
//
//   $$$$  $$$$$$$   $$$$  $$$$$$ $$$$   $$$   $$   $$  $$$$
//  $$  $$  $$   $  $$  $$ $ $$ $  $$   $$ $$  $$$  $$ $$  $$
//  $$$     $$ $   $$        $$    $$  $$   $$ $$$$ $$ $$$
//   $$$    $$$$   $$        $$    $$  $$   $$ $$ $$$$  $$$
//     $$$  $$ $   $$        $$    $$  $$   $$ $$  $$$    $$$
//  $$  $$  $$   $  $$  $$   $$    $$   $$ $$  $$   $$ $$  $$
//   $$$$  $$$$$$$   $$$$   $$$$  $$$$   $$$   $$   $$  $$$$
//
//--------------------------------------------------------------------------------

VOID  PEA::AddUnknownSection (IMAGE_SECTION_HEADER & sh)
{
    //
    // Winword.exe has a zero-length section named .CRT
    //
    if (sh.PointerToRawData == 0 && sh.SizeOfRawData == 0)
    {
        return ;
    }

    if (m_cUnknownSections == MAX_UNKNOWN_SECTIONS)
        SimpleException (SE_TOO_MANY_UNKNOWN_SECTIONS);

    if (m_rgpbUnknownSections == 0)
        m_rgpbUnknownSections = (PBYTE*) Alloc (MAX_UNKNOWN_SECTIONS * sizeof (PBYTE));

    m_rgpbUnknownSections[m_cUnknownSections] = m_pbBase + sh.VirtualAddress;

    SectionAddData
    (
        OTHER_PAGE,
        (DWORD)m_rgpbUnknownSections[m_cUnknownSections],
        DwSpanCount (sh.SizeOfRawData, g_cbPageSize),
        PAGE_EXECUTE_READ
    );

    MoveMemory
    (
        m_rgpbUnknownSections[m_cUnknownSections],
        sh.PointerToRawData + (PBYTE)m_lpMappedFile,
        sh.SizeOfRawData
    );

    m_cUnknownSections ++ ;
}

void PEA::SectionAddData
(
    E_PAGETYPE ePageType,
    DWORD      dwSectionBase,
    DWORD      cPgSectionPages,
    DWORD      dwSectionProtection
)
{
    PBAA pbaa = (PBAA)HeapAlloc (m_hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, sizeof(BAA) + sizeof(BPD));

    //
    // put a single BOBBLE_PAGE_DATA in here.  SetPermissionsOnArg should not expand it.
    //
    pbaa->dwProtectionRequested = dwSectionProtection ;
    pbaa->cBPDs = 1 ;

    PBPD pbpd = & pbaa->rgBPDs[0];

    pbpd->Init
    (
        ePageType,
        dwSectionBase,
        cPgSectionPages,
        dwSectionProtection
    ) ;

    m_arsActualLoadMap.AddAddressRangeToSet
    (
        dwSectionBase,
        cPgSectionPages,
        (DWORD)pbaa
    );
}

BOOL PEA::FScatterFile ()
{
    DWORD i;
    BOOL  f;
    DWORD dwMacRVA = 0;
    DWORD dwSectionMacRVA = 0;
    IMAGE_SECTION_HEADER sh;

    for (i=0; i < NumOfSections(); i++)
    {
        f = GetSectionHdrByIndex(&sh, i);
        if (f)
        {
            dwSectionMacRVA = sh.VirtualAddress + sh.SizeOfRawData;
            dwMacRVA        = MAX(dwMacRVA, dwSectionMacRVA);
        }
        else
        {
            SimpleException(SE_MALFORMED_SECTION_HEADERS);
        }
    }

    //
    // Now, VirtualAlloc enough CONTIGUOUS, writeable memory.  For now,
    // don't put the the thing where it wants to go, because we need to
    // test fixups
    //

    DWORD cbTotal = DwRoundUp(dwMacRVA, g_cbPageSize);

    m_pbBase = (PBYTE)VirtualAlloc
    (
        0,
        cbTotal,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (!m_pbBase)
        SimpleException (SE_OUTOFMEMORY);

    //
    // Paint each section into its new place
    //

    if (! m_textSection)
        SimpleException (SE_CANT_HAPPEN);

    for (i=0; i < NumOfSections(); i++)
    {
        f = GetSectionHdrByIndex(&sh, i);
        if (f)
        {
            if (FStrEq (sh.Name, ".text"))
            {
                m_pbText = m_pbBase + m_textSection->VirtualAddress;

                SectionAddData
                (
                    CODE_PAGE,
                    (DWORD)m_pbText,
                    DwSpanCount (m_textSection->SizeOfRawData, g_cbPageSize),
                    PAGE_EXECUTE_READ
                ) ;

                MoveMemory
                (
                    m_pbText,
                    m_textSection->PointerToRawData + (PBYTE)m_lpMappedFile,
                    m_textSection->SizeOfRawData
                );
            }
            else if (FStrEq (sh.Name, ".rdata"))
            {
                m_pbRdata = m_pbBase + m_rdataSection->VirtualAddress;

                SectionAddData
                (
                    RDATA_PAGE,
                    (DWORD)m_pbRdata,
                    DwSpanCount (m_rdataSection->SizeOfRawData, g_cbPageSize),
                    PAGE_READWRITE
                ) ;

                MoveMemory
                (
                    m_pbRdata,
                    m_rdataSection->PointerToRawData + (PBYTE)m_lpMappedFile,
                    m_rdataSection->SizeOfRawData
                );
            }
            else if (FStrEq (sh.Name, ".data"))
            {
                m_pbData = m_pbBase + m_dataSection->VirtualAddress;

                SectionAddData
                (
                    DATA_PAGE,
                    (DWORD)m_pbData,
                    DwSpanCount (m_dataSection->SizeOfRawData, g_cbPageSize),
                    PAGE_READWRITE
                );

                MoveMemory
                (
                    m_pbData,
                    m_dataSection->PointerToRawData + (PBYTE)m_lpMappedFile,
                    m_dataSection->SizeOfRawData
                );
            }
            else if (FStrEq (sh.Name, ".idata"))
            {
                m_pbIdata = m_pbBase + sh.VirtualAddress;

                SectionAddData
                (
                    IDATA_PAGE,
                    (DWORD)m_pbIdata,
                    DwSpanCount (sh.SizeOfRawData, g_cbPageSize),
                    PAGE_READONLY // DON'T ALLOW SUSPECT TO SET ITS OWN IMPORTS!!!
                );

                MoveMemory
                (
                    m_pbIdata,
                    sh.PointerToRawData + (PBYTE)m_lpMappedFile,
                    sh.SizeOfRawData
                );
            }
            else if (FStrEq (sh.Name, ".edata"))
            {
                m_pbEdata = m_pbBase + sh.VirtualAddress;

                SectionAddData
                (
                    EDATA_PAGE,
                    (DWORD)m_pbEdata,
                    DwSpanCount (sh.SizeOfRawData, g_cbPageSize),
                    PAGE_READONLY // DON'T ALLOW SUSPECT TO CHANGE ITS EXPORTS (PARANOIA)
                );

                MoveMemory
                (
                    m_pbEdata,
                    sh.PointerToRawData + (PBYTE)m_lpMappedFile,
                    sh.SizeOfRawData
                );
            }
            else if (FStrEq (sh.Name, ".reloc"))
            {
                m_pbReloc = m_pbBase + sh.VirtualAddress;

                SectionAddData
                (
                    RELOC_PAGE,
                    (DWORD)m_pbReloc,
                    DwSpanCount (sh.SizeOfRawData, g_cbPageSize),
                    PAGE_READONLY // DON'T ALLOW SUSPECT TO MODIFY ITS FIXUPS (PARANOIA)
                );

                MoveMemory
                (
                    m_pbReloc,
                    sh.PointerToRawData + (PBYTE)m_lpMappedFile,
                    sh.SizeOfRawData
                );
            }
            else if (FStrEq (sh.Name, ".debug"))
            {
                m_pbDebug = m_pbBase + sh.VirtualAddress;

                SectionAddData
                (
                    DEBUG_PAGE,
                    (DWORD)m_pbDebug,
                    DwSpanCount (sh.SizeOfRawData, g_cbPageSize),
                    PAGE_READONLY // DON'T ALLOW SUSPECT TO MODIFY ITS DEBUG INFO (PARANOIA)
                );

                MoveMemory
                (
                    m_pbDebug,
                    sh.PointerToRawData + (PBYTE)m_lpMappedFile,
                    sh.SizeOfRawData
                );
            }
            else if (FStrEq (sh.Name, ".tls"))
            {
                m_pbTls = m_pbBase + sh.VirtualAddress;

                SectionAddData
                (
                    TLS_PAGE,
                    (DWORD)m_pbTls,
                    DwSpanCount (sh.SizeOfRawData, g_cbPageSize),
                    PAGE_READWRITE
                );

                MoveMemory
                (
                    m_pbTls,
                    sh.PointerToRawData + (PBYTE)m_lpMappedFile,
                    sh.SizeOfRawData
                );
            }
            else if (FStrEq (sh.Name, ".rsrc"))
            {
                m_pbRsrc = m_pbBase + sh.VirtualAddress;

#pragma UNDONE (Should page permissions for RESOURCES be READWRITE?)
                SectionAddData
                (
                    RSRC_PAGE,
                    (DWORD)m_pbRsrc,
                    DwSpanCount (sh.SizeOfRawData, g_cbPageSize),
                    PAGE_READONLY
                );

                MoveMemory
                (
                    m_pbRsrc,
                    sh.PointerToRawData + (PBYTE)m_lpMappedFile,
                    sh.SizeOfRawData
                );
            }
            else
            {
                AddUnknownSection (sh);
            }
        }
        else
        {
            SimpleException(SE_MALFORMED_SECTION_HEADERS);
        }
    }

    return TRUE;
}


//--------------------------------------------------------------------------------
//
//  $$$$$$  $$$$$$$ $$$$$$  $$   $$ $$$$  $$$$   $$$$  $$$$   $$$   $$   $$  $$$$
//   $$  $$  $$   $  $$  $$ $$$ $$$  $$  $$  $$ $$  $$  $$   $$ $$  $$$  $$ $$  $$
//   $$  $$  $$ $    $$  $$ $$$$$$$  $$  $$$    $$$     $$  $$   $$ $$$$ $$ $$$
//   $$$$$   $$$$    $$$$$  $$$$$$$  $$   $$$    $$$    $$  $$   $$ $$ $$$$  $$$
//   $$      $$ $    $$ $$  $$ $ $$  $$     $$$    $$$  $$  $$   $$ $$  $$$    $$$
//   $$      $$   $  $$  $$ $$   $$  $$  $$  $$ $$  $$  $$   $$ $$  $$   $$ $$  $$
//  $$$$    $$$$$$$ $$$  $$ $$   $$ $$$$  $$$$   $$$$  $$$$   $$$   $$   $$  $$$$
//
//--------------------------------------------------------------------------------

static HANDLE g_hHeap ;         // sorry; this has to be made global for this iterator callback.
static BOOL   g_fBreakForData ; // this too (debugger can't deal with two copies of this
                                // library in memory, so we must break explicitly with int 3.)

static void InitializeComplementPageData (PARG parg)
{
    PBBL_ARG_ATTRIBUTES pbaa = (PBAA)HeapAlloc      // See PageData.* for all this
    (
        g_hHeap,
        HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY,
        sizeof(BAA)
    ) ;

    pbaa->dwProtectionRequested = PAGE_NOACCESS ;
    pbaa->cBPDs = 0 ;

    parg->SetDwAttributes ((DWORD)pbaa) ;
}



static void SetPermissionOnARG (PARG parg)
{
    DWORD  dwOldProtection = 0 ;

    PBAA   pbaa = (PBAA)parg->DwAttributes () ;

    PBPD   pbpd = 0 ;
    PVOID  pv   = 0 ;
    DWORD  cPg  = 0 ;
    DWORD  cb   = 0 ;
    DWORD  prot = 0 ;

    DWORD  dwRet  ;
    DWORD  dwBase ;
    MBI    mbi    ;

    //
    // At top level, find out whether we're denying access to a host region
    // or setting desired access on a suspect region.
    //

    if (pbaa->dwProtectionRequested != PAGE_NOACCESS)
    {
        // We're deprotecting a suspect page

        _ASSERTE(pbaa->cBPDs == 1) ;
        pbpd = & (pbaa->rgBPDs[0]) ;

        pbpd->fUnprotectionSuccessful      = IRRELEVANT ;
        pbpd->fSizeChangedWhileUnprotected = IRRELEVANT ;
        pbpd->fSizeChangedWhileProtected   = IRRELEVANT ;
        pbpd->fBaseChangedWhileUnprotected = IRRELEVANT ;
        pbpd->fBaseChangedWhileProtected   = IRRELEVANT ;

        // initialize, and change later.

        pbpd->dwBaseActual                 = 0     ;
        pbpd->cPgActual                    = 0     ;
        pbpd->dwProtectionOriginal         = 0     ;
        pbpd->fProtectionSuccessful        = FALSE ;

        pv   = (PVOID)pbpd->dwBaseRequested ;
        cPg  = pbpd->cPgRequested           ;
        cb   = cPg * g_cbPageSize           ;
        prot = pbpd->dwProtectionRequested  ;

        //
        // For now, don't call VirtualProtect -- just cruise through everything,
        // find out what's what, and report it in the dialog box.
        //

        /*/if (VirtualProtect (pv, cb, prot, &dwOldProtection))
        /*/if (1)
        /**/
        {
            // We got it!

            pbpd->dwBaseActual          = (DWORD)pv       ;
            pbpd->cPgActual             = cPg             ;
            pbpd->dwProtectionOriginal  = dwOldProtection ;
            // determine actual protection later, in ResetPermissions.
            pbpd->fProtectionSuccessful = TRUE            ;

            if (g_fBreakForData) // debugger can't deal...
                __asm {int 3}
        }
        return ;
    }
    else
    {
        // We're protecting a host region.  We must "discover"
        // its attributes by calling VirtualQuery in a loop.
        pv = parg->PvBase () ;

        _ASSERTE(parg->CPages() != 0) ;

        while (1)
        {
            dwRet = VirtualQuery (pv, &mbi, sizeof(mbi)) ;

            if (dwRet != sizeof(mbi))
            {
                DWORD e = GetLastError () ;
                if (    (e == 0x57) //parameter incorrect
                     //
                     // This is almost certainly due to asking for information about a
                     // region in non-user space.  We need a good test for this, but
                     // heuristically, it's whenever pv >= 0x7FFF0000
                     //
                     && (((DWORD)pv) >= 0x7FFF0000)
                   )
                {
                    return ; // we're done with this loop
                }
                else
                {
                    DbgLastErrorBox () ;
                    SimpleException (SE_VIRTUAL_QUERY) ;
                }
            }

            switch (mbi.State)
            {
            case MEM_FREE :
                // do NOTHING!!!
                pbpd = 0 ;  // don't confuse the debugger with stale values
                break ; // and loop

            default:
                // protect it!!!
                pbaa = PBAABumpRgBPDs (pbaa, g_hHeap) ;
                parg->SetDwAttributes ((DWORD)pbaa) ;
                pbpd = pbaa->PBPDLastBPD () ;

                pbpd->dwBaseActual                 = (DWORD)pv  ;
                pbpd->fUnprotectionSuccessful      = IRRELEVANT ;
                pbpd->fSizeChangedWhileUnprotected = IRRELEVANT ;
                pbpd->fSizeChangedWhileProtected   = IRRELEVANT ;
                pbpd->fBaseChangedWhileUnprotected = IRRELEVANT ;
                pbpd->fBaseChangedWhileProtected   = IRRELEVANT ;

                pbpd->cPgActual             = mbi.RegionSize / g_cbPageSize ;
                pbpd->dwProtectionRequested = pbaa->dwProtectionRequested   ;
                // determine actual protection later, in ResetPermissions.

                pbpd->dwProtectionOriginal  = 0;

                pbpd->dwAllocationBase = (DWORD)mbi.AllocationBase ;
                pbpd->dwMemState       = mbi.State ;
                pbpd->dwMemType        = mbi.Type ;

                //
                // For now, don't call VirtualProtect -- just cruise through everything,
                // find out what's what, and report it in the dialog box.
                //

                /*/pbpd->fProtectionSuccessful = VirtualProtect
                (
                    pv,
                    mbi.RegionSize,
                    pbaa->dwProtectionRequested,
                    &dwOldProtection
                ) ;
                /*/pbpd->fProtectionSuccessful = 1 ;
                /**/

                pbpd->dwProtectionOriginal  = dwOldProtection ;
                break ; // and loop
            }

            //
            // Move on up to the next region
            //

            dwBase = (DWORD) pv ;
            dwBase += mbi.RegionSize ;
            if (dwBase < parg->DwLastByte ())
            {
                pv = (PVOID)dwBase ;
                continue ;
            }
            else
            {
                return ;
            }
        }
    }

    SimpleException (SE_CANT_HAPPEN);
}



static void ResetPermissionOnARG (PARG parg)
{
    /**/
    DWORD dwOldProtection = 0                 ;
    PBAA   pbaa = (PBAA)parg->DwAttributes () ;

    if (pbaa == 0)
    {
        // Not sure how this could legitimately happen, but
        return ;
    }

    PBPD   pbpd = 0 ;
    PVOID  pv   = 0 ;
    DWORD  cbIn = 0 ;
    DWORD  i ;

    //
    // Walk all the PBPDs in the PBAA
    //

    for (i = 0; i < pbaa->cBPDs; i++)
    {
        pbpd = & (pbaa->rgBPDs[i]) ;
        _ASSERTE(pbpd) ;
        pv = (PVOID) pbpd->dwBaseActual ;
        cbIn = pbpd->cPgActual * g_cbPageSize ;

        //
        // For now, don't call VirtualProtect -- just cruise through everything,
        // find out what's what, and report it in the dialog box.
        //

        /*/if (VirtualProtect (pv, cbIn, pbpd->dwProtectionOriginal, &dwOldProtection))
        /*/if (1)
        /**/
        {
            pbpd->fBaseChangedWhileProtected = (pbpd->dwBaseActual != ((DWORD)pv)) ;
            pbpd->dwBaseActual               = ((DWORD)pv)                         ;
            pbpd->dwProtectionActual         = dwOldProtection                     ;

            pbpd->fUnprotectionSuccessful    = TRUE                                ;
        }
    }
}



void PEA::ComputeComplement  ()
{
    m_parsLoadMapComplement = m_arsActualLoadMap.Complement (0) ;

    g_hHeap = m_hHeap ; // sorry, this has to be made global for the following callback.

    g_fBreakForData = m_fBreakForData ; // this too.

#pragma UNDONE(PEA is not thread-safe)

    m_parsLoadMapComplement->ForEach (InitializeComplementPageData);
}



void PEA::RestrictComplement ()
{
    m_parsLoadMapComplement->ForEach (SetPermissionOnARG) ;
}


void PEA::UnrestrictComplement ()
{
    m_parsLoadMapComplement->ForEach (ResetPermissionOnARG) ;
}


//
// The following is almost exactly the same as SetPermisssions.  It uses the
// fact that Set stores the existing page permissions in the slot that stored
// the desired permissions: so desired and existing toggle back and forth.
//
void PEA::ResetPermissions ()
{
    g_fBreakForData = m_fBreakForData ;

    m_arsActualLoadMap.ForEach (ResetPermissionOnARG);

    if (m_parsLoadMapComplement == 0)
    {
        SimpleException (SE_MISSING_COMPLEMENT) ;
    }

    UnrestrictComplement ();
}



void PEA::SetPermissions ()
{
    g_fBreakForData = m_fBreakForData ;
    g_hHeap = m_hHeap ;

    m_arsActualLoadMap.ForEach (SetPermissionOnARG);

    if (m_parsLoadMapComplement == 0)
    {
        ComputeComplement () ;
    }

    RestrictComplement () ;
}



DWORD PEA::DwCallBobbleEntryPoint ()
{
    LPVOID      pvDllEntryPoint        = GetModuleEntryPoint();
    LPVOID      pvRebasedDllEntryPoint = PvRebaseCodeAddress(pvDllEntryPoint);
    PFN_DLLMAIN pfnDllMain             = (PFN_DLLMAIN)pvRebasedDllEntryPoint;

    DWORD       dwRet                  = 0 ;

    dwRet = (DWORD)(*pfnDllMain)(HPseudoInstance(), DLL_PROCESS_ATTACH, 0);

    return dwRet;
}



//--------------------------------------------------------------------------------
//
//  $$$$$     $$$    $$$$       $$  $$  $$$$$   $$$$$$
//   $$ $$   $$ $$  $$  $$      $$  $$   $$ $$   $$  $$
//   $$  $$ $$   $$ $$$         $$  $$   $$  $$  $$  $$
//   $$  $$ $$   $$  $$$        $$$$$$   $$  $$  $$$$$
//   $$  $$ $$   $$    $$$      $$  $$   $$  $$  $$ $$
//   $$ $$   $$ $$  $$  $$      $$  $$   $$ $$   $$  $$
//  $$$$$     $$$    $$$$       $$  $$  $$$$$   $$$  $$
//
//--------------------------------------------------------------------------------


BOOL  PEA::GetDosHeader (PIMAGE_DOS_HEADER pHeader)
{
    /* dos header rpresents first structure of bytes in file */
    USHORT usDosSig ;

    usDosSig = *((USHORT*)m_lpMappedFile);

    if (usDosSig == IMAGE_DOS_SIGNATURE)
        CopyMemory ((LPVOID)pHeader, m_lpMappedFile, sizeof (IMAGE_DOS_HEADER));
    else
        return FALSE;

    return TRUE;
}




DWORD  PEA::ImageFileType ()
{
    /* dos file signature comes first */
    if (*(USHORT *)m_lpMappedFile == IMAGE_DOS_SIGNATURE)
    {
        WORD   wSig ;
        DWORD dwSig ;

        dwSig = *((DWORD*) (NTSIGNATURE (m_lpMappedFile)));
        wSig  = LOWORD (dwSig) ;

        /* determine location of PE File header from dos header */
        if (wSig == IMAGE_OS2_SIGNATURE ||
            wSig == IMAGE_OS2_SIGNATURE_LE)
            return (DWORD)wSig;

        else if (dwSig == IMAGE_NT_SIGNATURE)
            return IMAGE_NT_SIGNATURE;

        else
            return IMAGE_DOS_SIGNATURE;
    }

    else
        /* unknown file type */
        return 0;
}


//--------------------------------------------------------------------------------
//
//  $$$$$$  $$$$$$$     $$  $$  $$$$$   $$$$$$   $$$$
//   $$  $$  $$   $     $$  $$   $$ $$   $$  $$ $$  $$
//   $$  $$  $$ $       $$  $$   $$  $$  $$  $$ $$$
//   $$$$$   $$$$       $$$$$$   $$  $$  $$$$$   $$$
//   $$      $$ $       $$  $$   $$  $$  $$ $$     $$$
//   $$      $$   $     $$  $$   $$ $$   $$  $$ $$  $$
//  $$$$    $$$$$$$     $$  $$  $$$$$   $$$  $$  $$$$
//
//--------------------------------------------------------------------------------


BOOL  PEA::GetPEFileHeader (PIMAGE_FILE_HEADER pHeader)
{
    /* file header follows dos header */
    if (ImageFileType () == IMAGE_NT_SIGNATURE)
        CopyMemory ((LPVOID)pHeader, PEFHDROFFSET (m_lpMappedFile), sizeof (IMAGE_FILE_HEADER));

    else
        return FALSE;

    return TRUE;
}





BOOL PEA::GetPEOptionalHeader (PIMAGE_OPTIONAL_HEADER pHeader)
{
    /* optional header follows file header and dos header */
    if (ImageFileType () == IMAGE_NT_SIGNATURE)
        CopyMemory ((LPVOID)pHeader, m_poh, sizeof (IMAGE_OPTIONAL_HEADER));

    else
        return FALSE;

    return TRUE;
}




LPVOID  PEA::GetModuleEntryPoint ()
{
    if (m_poh != NULL)
        return (LPVOID)(m_poh->AddressOfEntryPoint);
    else
        return NULL;
}




DWORD   PEA::NumOfSections ()
{
    /* number os sections is indicated in file header */
    return ((DWORD)((PIMAGE_FILE_HEADER)PEFHDROFFSET (m_lpMappedFile))->NumberOfSections);
}




LPVOID  PEA::GetImageBase ()
{
    if (m_poh != NULL)
        return (LPVOID)(m_poh->ImageBase);
    else
        return NULL;
}


DWORD PEA::GetSectionNames (char **pszSections)
{
    DWORD                    nSections = NumOfSections ();
    DWORD                    i, nCnt = 0;
    PIMAGE_SECTION_HEADER    psh;
    char                    *ps;


    if (ImageFileType () != IMAGE_NT_SIGNATURE ||
       (psh = (PIMAGE_SECTION_HEADER)SECHDROFFSET (m_lpMappedFile)) == NULL)
        return 0;

    for (i=0; i<nSections; i++)
    {
        char *szTypeSafe = (char*)(&(psh[i].Name[0]));
        nCnt += strlen (szTypeSafe) + 1;
    }

    ps = *pszSections = (char *) Alloc (nCnt);

    for (i=0; i<nSections; i++)
    {
        char *szTypeSafe = (char*)(&(psh[i].Name[0]));

        strcpy (ps, szTypeSafe);

        ps += strlen (szTypeSafe) + 1;
    }

    return nCnt;
}




BOOL    PEA::GetSectionHdrByName (IMAGE_SECTION_HEADER *sh, char *szSection)
{
    PIMAGE_SECTION_HEADER psh;
    DWORD                 nSections = NumOfSections ();
    DWORD                 i;


    if ((psh = (PIMAGE_SECTION_HEADER)SECHDROFFSET (m_lpMappedFile)) != NULL)
    {
        for (i=0; i<nSections; i++)
        {
            char *szTypeSafe = (char*)(&(psh->Name[0]));

            if (!strcmp (szTypeSafe, szSection))
            {
                CopyMemory ((LPVOID)sh, (LPVOID)psh, sizeof (IMAGE_SECTION_HEADER));
                return TRUE;
            }
            else
            {
                psh++;
            }
        }
    }

    return FALSE;
}




PISH    PEA::PISHSectionHdrByName (char * szSection)
{
    PIMAGE_SECTION_HEADER psh;
    DWORD                 nSections = NumOfSections ();
    DWORD                 i;

    if ((psh = (PIMAGE_SECTION_HEADER)SECHDROFFSET (m_lpMappedFile)) != NULL)
    {
        for (i=0; i<nSections; i++)
        {
            char *szTypeSafe = (char*)(&(psh->Name[0]));

            if (!strcmp (szTypeSafe, szSection))
            {
                return psh;
            }
            else
            {
                psh++;
            }
        }
    }

    return NULL;
}




BOOL    PEA::GetSectionHdrByIndex (IMAGE_SECTION_HEADER *sh, DWORD iWhich)
{
    PIMAGE_SECTION_HEADER    psh;
    DWORD                      nSections = NumOfSections ();

    psh = (PIMAGE_SECTION_HEADER)SECHDROFFSET (m_lpMappedFile) ;

    if (psh && (iWhich <= nSections))
    {
        CopyMemory
        (
            (LPVOID)sh,
            (LPVOID)(& (psh[iWhich])),
            sizeof (IMAGE_SECTION_HEADER)
        );

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


//--------------------------------------------------------------------------------
//
//  $$$$$     $$    $$$$$$   $$        $$$$$    $$$$  $$$$$$   $$$$
//   $$ $$   $$$$   $ $$ $  $$$$        $$ $$    $$    $$  $$ $$  $$
//   $$  $$ $$  $$    $$   $$  $$       $$  $$   $$    $$  $$ $$$
//   $$  $$ $$  $$    $$   $$  $$       $$  $$   $$    $$$$$   $$$
//   $$  $$ $$$$$$    $$   $$$$$$       $$  $$   $$    $$ $$     $$$
//   $$ $$  $$  $$    $$   $$  $$       $$ $$    $$    $$  $$ $$  $$
//  $$$$$   $$  $$   $$$$  $$  $$      $$$$$    $$$$  $$$  $$  $$$$
//
//--------------------------------------------------------------------------------


inline PVOID PEA::PvFromRVABiasedToSection (DWORD RVA, PISH pish)
{
    return (LPVOID)
    (
          (DWORD)m_lpMappedFile
        + (DWORD)RVA
        - (DWORD)pish->VirtualAddress
        + (DWORD)pish->PointerToRawData
    );
}


LPVOID  PEA::ImageDirectoryPointer
(
    PIMAGE_SECTION_HEADER pOptionalSectionHeaderOutput,
    DWORD                 dwWhich
)
{
    PIMAGE_SECTION_HEADER    psh = (PIMAGE_SECTION_HEADER)SECHDROFFSET (m_lpMappedFile);
    DWORD                    nSections = NumOfSections ();
    DWORD                    i = 0;
    DWORD                    VAImageDir;

    if (dwWhich >= m_poh->NumberOfRvaAndSizes)
        return NULL;

    //
    // Get the RVA of the desired Data Directory from
    // the PE File Optional Header.
    //
    VAImageDir = m_poh->DataDirectory[dwWhich].VirtualAddress;

    if (VAImageDir == 0)
    {
        //
        // The requested image directory doesn't exist!
        //
        return 0;
    }

    //
    // The data directory is located inside some section,
    // for which we must search by RVA, by iterating over
    // all the section headers in the Section Header section
    // of the PE file header (whew).
    //
    for (i=0; i<nSections; i++, psh++)
    {
        if
        (
               (psh->VirtualAddress <= VAImageDir)
            && (psh->VirtualAddress + psh->SizeOfRawData > VAImageDir)
        )
        {
            //
            // If caller wants the Section Header containing the
            // the desired Data Directory, then copy out the Section Header
            //
            if (pOptionalSectionHeaderOutput)
            {
                CopyMemory
                (
                    pOptionalSectionHeaderOutput,
                    psh,
                    sizeof(IMAGE_SECTION_HEADER)
                );
            }

            //
            // Found it!
            //
            break;
        }
    }

    if (i > nSections)
    {
        //
        // Didn't find it!
        //
        return NULL;
    }

    //
    // return memory-mapped pointer of the actual data directory pointer
    //
    return PvFromRVABiasedToSection (VAImageDir, psh);
}


//--------------------------------------------------------------------------------
//
//  $$$$$$$  $$   $$ $$$$$$    $$$   $$$$$$  $$$$$$  $$$$
//   $$   $  $$   $$  $$  $$  $$ $$   $$  $$ $ $$ $ $$  $$
//   $$ $     $$ $$   $$  $$ $$   $$  $$  $$   $$   $$$
//   $$$$      $$$    $$$$$  $$   $$  $$$$$    $$    $$$
//   $$ $      $$$    $$     $$   $$  $$ $$    $$      $$$
//   $$   $   $$ $$   $$      $$ $$   $$  $$   $$   $$  $$
//  $$$$$$$  $$   $$ $$$$      $$$   $$$  $$  $$$$   $$$$
//
//--------------------------------------------------------------------------------


BOOL PEA::GetImageExportDirectory (PIMAGE_EXPORT_DIRECTORY pedOut)
{
    if (pedOut == NULL)
        return FALSE;

    PIMAGE_EXPORT_DIRECTORY ped;

    ped = (PIMAGE_EXPORT_DIRECTORY) ImageDirectoryPointer
    (
        0, // don't need the section header
        IMAGE_DIRECTORY_ENTRY_EXPORT
    );

    if (ped == NULL)
        return FALSE;

    CopyMemory (pedOut, ped, sizeof(IMAGE_EXPORT_DIRECTORY));

    return TRUE;
}




DWORD  PEA::GetExportFunctionNames (char **pszFunctions)
{
    IMAGE_SECTION_HEADER     sh;
    PIMAGE_EXPORT_DIRECTORY  ped;
    char                    *pNames, *pCnt;
    DWORD                    i, nCnt;
	DWORD                    cNames ;

    //
    // Guess what!  Exports sometimes show up in the .rdata section!
    //

    ped = (PIMAGE_EXPORT_DIRECTORY) ImageDirectoryPointer
    (
        &sh,
        IMAGE_DIRECTORY_ENTRY_EXPORT
    );

    if (ped == NULL)
        return 0;

    //
    // determine the offset of the export function names
    //
    DWORD RVAOfNames = (* ((DWORD*) PvFromRVABiasedToSection ((DWORD)ped->AddressOfNames, &sh)));

    pNames = (char*) PvFromRVABiasedToSection (RVAOfNames, &sh);

    /** /
    pNames = (char *)(*(DWORD *)(  (DWORD)ped->AddressOfNames
                                 - (DWORD)sh.VirtualAddress
                                 + (DWORD)sh.PointerToRawData
                                 + (DWORD)m_lpMappedFile
                                )
                      - (DWORD)sh.VirtualAddress
                      + (DWORD)sh.PointerToRawData
                      + (DWORD)m_lpMappedFile);
    /**/

    //
    // figure out how much memory to allocate for all strings
    //
    pCnt = pNames;

	cNames = (DWORD)ped -> NumberOfNames ;

    for (i=0; i<cNames; i++)
    {
        //
        // Guess what !  There are some names containing dots, as in
        // NTDLL.RtlFoobupYourPELoader, that are not counted in the
        // export directory's "NumberOfNames".  Since we're going to
        // block-copy the whole mess, we have to make sure we get these,
        // too
        //
        if (strchr (pCnt, '.'))
        {
            i-- ;
        }

        while (*pCnt++)
        {
            ;
        }
    }

    nCnt = (DWORD)(pCnt - pNames);

    *pszFunctions = (char*) Alloc (nCnt);

    CopyMemory ((LPVOID)*pszFunctions, (LPVOID)pNames, nCnt);

    return nCnt;
}




DWORD     PEA::NumberOfExportedFunctions ()
{
    PIMAGE_EXPORT_DIRECTORY    ped;

    ped = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryPointer
    (
        0,
        IMAGE_DIRECTORY_ENTRY_EXPORT
    ) ;

    if (ped == NULL)
        return 0;
    else
        return (DWORD)ped->NumberOfNames;
}




PDWORD   PEA::GetExportFunctionEntryPoints ()
{
    IMAGE_SECTION_HEADER    sh;
    PIMAGE_EXPORT_DIRECTORY ped;

    ped = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryPointer
    (
        &sh,
        IMAGE_DIRECTORY_ENTRY_EXPORT
    );

    if (ped == NULL)
        return NULL;

    return (PDWORD) PvFromRVABiasedToSection ((DWORD)ped->AddressOfFunctions, &sh);
}




PWORD   PEA::GetExportFunctionOrdinals ()
{
    IMAGE_SECTION_HEADER       sh;
    PIMAGE_EXPORT_DIRECTORY    ped;

    ped = (PIMAGE_EXPORT_DIRECTORY)ImageDirectoryPointer
    (
        &sh,
        IMAGE_DIRECTORY_ENTRY_EXPORT
    );

    if (ped == NULL)
        return NULL;

    return (PWORD) PvFromRVABiasedToSection ((DWORD)ped->AddressOfNameOrdinals, &sh);
}


//--------------------------------------------------------------------------------
//
//  $$$$  $$   $$ $$$$$$    $$$   $$$$$$  $$$$$$  $$$$
//   $$   $$$ $$$  $$  $$  $$ $$   $$  $$ $ $$ $ $$  $$
//   $$   $$$$$$$  $$  $$ $$   $$  $$  $$   $$   $$$
//   $$   $$$$$$$  $$$$$  $$   $$  $$$$$    $$    $$$
//   $$   $$ $ $$  $$     $$   $$  $$ $$    $$      $$$
//   $$   $$   $$  $$      $$ $$   $$  $$   $$   $$  $$
//  $$$$  $$   $$ $$$$      $$$   $$$  $$  $$$$   $$$$
//
//--------------------------------------------------------------------------------


PIDD PEA::PIDDFirstImageImportDirectory  (PIMAGE_SECTION_HEADER pidsh)
{
    return (PIMAGE_IMPORT_DESCRIPTOR) ImageDirectoryPointer
    (
        pidsh,
        IMAGE_DIRECTORY_ENTRY_IMPORT
    );
}




VOID   PEA::FetchImportsFromModule (char* szModuleName, PIDD /*pidFirst*/, PIDD pid, ISH& ish)
{
    IMAGE_THUNK_DATA          itd;
    PIMAGE_IMPORT_BY_NAME     pibn;
    char                     *szImportName;
    DWORD                     dwImportOrdinal;
    DWORD                    *pdwSmack;

    DWORD  dwSourceBase, dwDestinationBase;
    DWORD  i = 0;
    DWORD  dwFunction, dwTarget;

    FARPROC pfn;

    HINSTANCE hModule = LoadLibraryA (szModuleName);
    if (hModule == NULL)
    {
        DbgLastErrorBox();
        SimpleException (SE_NO_BASE_MODULE);
    }

    //
    // POINTER BASEs for other RVAs in the Image Import Descriptor.
    //
    dwSourceBase      = (DWORD)PvFromRVABiasedToSection (0, &ish);
    dwDestinationBase = (DWORD)(m_pbBase);

    for
    (
        (dwFunction = (DWORD)pid->OriginalFirstThunk),
        (dwTarget   = (DWORD)pid->FirstThunk);

        dwFunction;

        (dwFunction += 4), (dwTarget += 4)
    )
    {
        //
        // We have a function.  It's an RVA for an Image Thunk Data,
        // which we will now fetch (it's small)
        //
        itd = *( (PIMAGE_THUNK_DATA)(dwFunction + dwSourceBase) );

        if (itd.u1.Function == 0)
            break; // Is this a legitimate way to be done?

        //
        // Check whether the import is by ordinal or by name
        //
        if (itd.u1.Ordinal & IMAGE_ORDINAL_FLAG)
        {
            dwImportOrdinal = itd.u1.Ordinal & (~IMAGE_ORDINAL_FLAG);

            pfn = GetProcAddress
            (
                hModule,
                (const char*)(dwImportOrdinal & 0x0000FFFF)
            );

            if (pfn == NULL)
            {
                DbgLastErrorBox();
                SimpleException (SE_NO_PROC_ADDRESS_BY_ORDINAL);
            }
        }
        else
        {
            //
            // Fetch the IMAGE_IMPORT_BY_NAME...
            //
            pibn = (PIMAGE_IMPORT_BY_NAME)
                ((DWORD)itd.u1.AddressOfData + dwSourceBase);

            //
            // Get the pointer to the name.  This time, it's
            // not an RVA, but rather an offset in the ibn struct
            // (what a mess...)
            //
            szImportName = (char*) &(pibn->Name[0]);

            //
            // If there is no name, then we blew it somewhere
            //
            if (szImportName[0] == '\0')
                SimpleException (SE_MISSING_IMPORT_NAME);

            pfn = GetProcAddress (hModule, szImportName);

            if (pfn == NULL)
            {
                DbgLastErrorBox();
                SimpleException (SE_NO_PROC_ADDRESS_BY_NAME);
            }
        }

        pdwSmack = (PDWORD)(dwDestinationBase + dwTarget);
        *pdwSmack = (DWORD)pfn;
    }

    FreeLibrary (hModule);
}




VOID   PEA::FetchImports ()
{
    IMAGE_SECTION_HEADER     idsh;
    PIMAGE_IMPORT_DESCRIPTOR pid, pidFirst ;

    DWORD  nCnt  = 0;
    DWORD  i     = 0;
    char  *psz   = 0;

    pidFirst = pid = PIDDFirstImageImportDirectory (&idsh);

    //
    // Each pid points to information about a particular module.
    // Iterate until NULL.
    //
    for (; pid->Name; pid++, nCnt++)
    {
        if (nCnt >= MAX_IMPORTED_MODULES)
            SimpleException (SE_TOO_MANY_MODULES);

        FetchImportsFromModule
        (
            (char*)PvFromRVABiasedToSection (pid->Name, &idsh),
            pidFirst,
            pid,
            idsh
        );
    }
}




DWORD  PEA::GetImportModuleNames (char **pszModules)
{
    IMAGE_SECTION_HEADER     idsh;
    //
    // The following is the gateway to a complex data type describing
    // an import.  Its only important attribute in context of this
    // function is the name of the module.
    //
    PIMAGE_IMPORT_DESCRIPTOR pid ;

    DWORD  nCnt  = 0;
    DWORD  nSize = 0;
    DWORD  i     = 0;
    char  *psz   = 0;

    char  *pModule[MAX_IMPORTED_MODULES];

    ZeroMemory(pModule, sizeof(pModule));

    pid = (PIMAGE_IMPORT_DESCRIPTOR) ImageDirectoryPointer
    (
        &idsh,
        IMAGE_DIRECTORY_ENTRY_IMPORT
    );

    while (pid->Name)
    {
        if (nCnt >= MAX_IMPORTED_MODULES)
            SimpleException (SE_TOO_MANY_MODULES);

        pModule[nCnt] = (char*)PvFromRVABiasedToSection (pid->Name, &idsh);

        nSize += strlen (pModule[nCnt]) + 1;

        pid++;
        nCnt++;
    }

    *pszModules = (char*) Alloc (nSize);

    psz = *pszModules;

    for (i=0; i<nCnt; i++)
    {
        strcpy (psz, pModule[i]);
        psz += strlen (psz) + 1;
    }

    return nCnt;
}




//
// Avoid loading sprintf and all that just to print decimals
//
static DWORD DecimallyStringizeOrdinal(char *sz, DWORD dwOrd)
{
    DWORD i, j, k;
    char szT[16];

    strcpy (sz, "Ordinal(");

    ZeroMemory (szT, 16);

    k = dwOrd;
    i = 0;

    while (k)
    {
        szT[i] = char ( (k % 10) + '0' );
        i ++ ;
        k /= 10;
        if (i >= 16)
            SimpleException (SE_OVERFLOW_STACKVAR);
    }

    j = strlen (sz);

    for (k=0; k<i; k++)
    {
        sz [ j + k ] = szT [i-1-k];
    }
    j += k ;
    sz [j ++] = ')';
    sz [j] = '\0';

    return strlen(sz);
}




DWORD  PEA::GetImportFunctionNamesByModule
(
    char  *pszModule,
    char **pszFunctions
)
{
    IMAGE_SECTION_HEADER      idsh;
    PIMAGE_IMPORT_DESCRIPTOR  pid;
    IMAGE_THUNK_DATA          itd;
    PIMAGE_IMPORT_BY_NAME     pibn;
    char                     *szImportName;
    DWORD                     dwImportOrdinal;

    DWORD  dwBase;
    DWORD  nCnt = 0, nSize = 0, i = 0;
    DWORD  dwFunction;
    char  *psz;

    //
    // Memory-mapped pointer to the first Image Import Descriptor,
    // which happens to the be the Image Data Directory for the import
    // section.  It's a null-terminated array of Image Import
    // Descriptors.
    //
    pid = (PIMAGE_IMPORT_DESCRIPTOR) ImageDirectoryPointer
    (
        &idsh,
        IMAGE_DIRECTORY_ENTRY_IMPORT
    );

    //
    // Add dwBase to any RVA and get a real pointer into virtual memory.
    //
    dwBase = (DWORD)PvFromRVABiasedToSection (0, &idsh) ;

    //
    // linearly search for module's pid
    //
    while
    (
           pid->Name
        && strcmp (pszModule, (char *)(pid->Name + dwBase))
    )
    {
        pid++;
    }

    //
    // exit if the module is not found
    //
    if (!pid->Name)
        return 0;

    //
    // As usual, we have to do everything twice: once to figure out
    // how long it is and to mallocate the memory, again to copy out.
    //
    // Iterate over Image Thunk Datas (in the Import Lookup Table)
    //
    for
    (
        (dwFunction = (DWORD)pid->OriginalFirstThunk) // yet another RVA
        , (nCnt = 0)
        , (nSize = 0)
        ;

        dwFunction;

        (dwFunction += 4), (nCnt++)
    )
    {
        //
        // We have a function.  It's an RVA for an Image Thunk Data,
        // which we will now fetch (it's small)
        //
        itd = *( (PIMAGE_THUNK_DATA)(dwFunction + dwBase) );

        if (itd.u1.Function == 0)
            break; // Is this a legitimate way to be done?

        //
        // Check whether the import is by ordinal or by name
        //
        if (((DWORD)(itd.u1.Function)) & IMAGE_ORDINAL_FLAG)
        {
            //
            // Our choice here is to stringize the ordinal
            // The following is a bit wasteful, since we're going
            // to construct the stringized ordinal, just to throw
            // it away after calculating its length.  Do it the
            // old fashioned way to avoid dragging in sprintf.
            //

            char szTemp[128];

            dwImportOrdinal = ((DWORD)(itd.u1.Function)) & (~IMAGE_ORDINAL_FLAG);

            nSize += DecimallyStringizeOrdinal(szTemp, dwImportOrdinal) + 1;
        }
        else
        {
            //
            // Fetch the IMAGE_IMPORT_BY_NAME...
            //
            pibn = (PIMAGE_IMPORT_BY_NAME)
                ((DWORD)itd.u1.AddressOfData + dwBase);

            //
            // Get the pointer to the name.  This time, it's
            // not an RVA, but rather an offset in the ibn struct
            // (what a mess...)
            //
            szImportName = (char*) &(pibn->Name[0]);

            //
            // If there is no name, then we blew it somewhere
            //
            if (szImportName[0] == '\0')
                SimpleException (SE_MISSING_IMPORT_NAME);

            //
            // Finally, add up the size
            //
            nSize += strlen (szImportName) + 1;
        }
    }

    *pszFunctions = (char*) Alloc (nSize);

    psz = *pszFunctions;

    //
    // Do it again, to copy out the data.. (sigh)
    //
    for
    (
        (dwFunction = (DWORD)pid->OriginalFirstThunk), i=0;

        dwFunction;

        (dwFunction += 4)
    )
    {
        itd = *( (PIMAGE_THUNK_DATA)(dwFunction + dwBase) );

        if (itd.u1.Function == 0)
            break; // Once again, Is this a legitimate way to be done?

        if (((DWORD)(itd.u1.Function)) & IMAGE_ORDINAL_FLAG)
        {
            char szTemp[128];

            dwImportOrdinal = ((DWORD)(itd.u1.Function)) & (~IMAGE_ORDINAL_FLAG);

            i = DecimallyStringizeOrdinal(szTemp, dwImportOrdinal) + 1;

            strcpy (psz, szTemp);

            psz += i;
        }
        else
        {
            pibn = (PIMAGE_IMPORT_BY_NAME)
                ((DWORD)itd.u1.AddressOfData + dwBase);

            szImportName = (char*) &(pibn->Name[0]);

            strcpy (psz, szImportName);

            psz += strlen (szImportName) + 1;
        }
    }

    return nCnt;
}


//--------------------------------------------------------------------------------
//
//  $$$$$$$  $$$$  $$   $$  $$  $$  $$$$$$    $$$$
//   $$   $   $$   $$   $$  $$  $$   $$  $$  $$  $$
//   $$ $     $$    $$ $$   $$  $$   $$  $$  $$$
//   $$$$     $$     $$$    $$  $$   $$$$$    $$$
//   $$ $     $$     $$$    $$  $$   $$         $$$
//   $$       $$    $$ $$   $$  $$   $$      $$  $$
//  $$$$     $$$$  $$   $$  $$$$$$  $$$$      $$$$
//
//--------------------------------------------------------------------------------
//
// Stateless enumerators.
//
//--------------------------------------------------------------------------------



DWORD   PEA::ActualLoadBase ()
{
    return (DWORD)m_pbBase ;
}




LPVOID PEA::PvRebaseCodeAddress(LPVOID pv)
{
    return (DWORD)pv + m_pbBase;
}




DWORD   PEA::PreferredLoadBase()
{
    return m_poh->ImageBase;
}




int     PEA::Delta ()
{
    return ((int)m_pbBase) - ((int)PreferredLoadBase());
}



DWORD PEA::DwRvaFixup (PIFX pifx, PFXP pfxp)
{
    return pifx->VirtualAddress + pfxp->wPageOffset;
}




DWORD PEA::ActualFixupAddress (PIFX pifx, PFXP pfxp)
{
    return (DWORD) (m_pbBase + pifx->VirtualAddress + pfxp->wPageOffset);
}




BOOL PEA::FIsFixupInSection (PISH pish, PIFX pifx, PFXP pfxp)
{
    DWORD dwRVAFixup;
    DWORD dwRVABaseOfSection;
    DWORD dwFirstAddressAfterSection;

    dwRVAFixup = DwRvaFixup (pifx, pfxp);
    dwRVABaseOfSection = pish->VirtualAddress;
    dwFirstAddressAfterSection = dwRVABaseOfSection + pish->SizeOfRawData;

    return (dwRVAFixup >= dwRVABaseOfSection)
        && (dwRVAFixup <  dwFirstAddressAfterSection) ;
}




BOOL PEA::FIsCodeFixup (PIFX pifx, PFXP pfxp)
{
    return FIsFixupInSection (m_textSection, pifx, pfxp);
}




BOOL PEA::FIsDataFixup (PIFX pifx, PFXP pfxp)
{
    return FIsFixupInSection (m_dataSection, pifx, pfxp);
}




BOOL PEA::FIsRdataFixup (PIFX pifx, PFXP pfxp)
{
    return FIsFixupInSection (m_rdataSection, pifx, pfxp);
}




PISH PEA::PISHSectionContainingFixup (PIFX pifx, PFXP pfxp)
{
    PISH   psh;
    DWORD  nSections = NumOfSections ();
    DWORD  i;

    if ((psh = (PISH)SECHDROFFSET (m_lpMappedFile)) != NULL)
    {
        for (i=0; i<nSections; i++)
        {
            char *szTypeSafe = (char*)(&(psh->Name[0]));

            if (FIsFixupInSection (psh, pifx, pfxp))
            {
                return psh;
            }
            else
            {
                psh++;
            }
        }
    }

    return NULL;
}




DWORD   PEA::DataPriorToFixup (PIFX pifx, PFXP pfxp)
{
    switch (pfxp->wType)
    {
    case IMAGE_REL_BASED_HIGHLOW:
        return *((DWORD*)ActualFixupAddress(pifx, pfxp));
        break ;

    case IMAGE_REL_BASED_ABSOLUTE:
        break ;

    default:
        SimpleException (SE_UNSUPPORTED_FIXUP_TYPE);
        break ;
    }

    return 0;
}




DWORD   PEA::DataPostFixup (PIFX pifx, PFXP pfxp, BOOL fWrite)
{
    DWORD * pdwFixupVictim ;
    DWORD   dwDelta = Delta () ;
    DWORD   dwPost;

    switch (pfxp->wType)
    {
    case IMAGE_REL_BASED_HIGHLOW:

        pdwFixupVictim = (DWORD*)(ActualFixupAddress(pifx, pfxp));

        dwPost = 0;

        if (*pdwFixupVictim)
        {
            dwPost = *pdwFixupVictim + dwDelta;
            if (fWrite)
                *pdwFixupVictim = dwPost;
        }

        return dwPost;
        break ;

    case IMAGE_REL_BASED_ABSOLUTE:
        break ;

    default:
        SimpleException (SE_UNSUPPORTED_FIXUP_TYPE);
        break ;
    }

    return 0;
}




BOOL PEA::FApplyAllFixups ()
{
    PIFX pifx;
    PFXP pfxp;
    BOOL f = FALSE ;

    for
    (
        pifx = PIFXFirstFixupDirectory(NULL);
        pifx;
        pifx = PIFXNextFixupDirectory(pifx)
    )
    {
        for
        (
            pfxp = PFXPFirstFixup (pifx);
            pfxp;
            pfxp = PFXPNextFixup (pifx, pfxp)
        )
        {
            if (DataPostFixup (pifx, pfxp, TRUE))
                f = TRUE;
        }
    }

    return f;
}




DWORD   PEA::CFixupDirectories ()
{
    DWORD i = 0;
    PIFX pifx;

    for
    (
        pifx = PIFXFirstFixupDirectory(NULL); // don't need the section header
        pifx;
        pifx = PIFXNextFixupDirectory(pifx)
    )
    {
        i ++ ;
    }

    return i;
}




DWORD   PEA::CFixups (PIFX pifx) // an IFX is a Fixup Directory (pointer)
{
    DWORD i = 0;

    PFXP pfxp;

    for
    (
        pfxp = PFXPFirstFixup (pifx);
        pfxp;
        pfxp = PFXPNextFixup (pifx, pfxp)
    )
    {
        i ++ ;
    }

    return i;
}




PIFX PEA::PIFXFirstFixupDirectory        (PIMAGE_SECTION_HEADER pifsh)
{
    return (PIFX) ImageDirectoryPointer
    (
        pifsh,
        IMAGE_DIRECTORY_ENTRY_BASERELOC
    );
}




PFXP PEA::PFXPFirstFixup (PIFX pifx)
{
    return ((PFXP)(pifx + 1));
}




PFXP PEA::PFXPNextFixup (PIFX pifx, PFXP pfxp)
{
    DWORD dwTooFar = ((DWORD)(pifx)) + pifx->SizeOfBlock ;
    DWORD dwCheck  = ((DWORD)(pfxp + 1));

    if (dwCheck >= dwTooFar)
        return NULL;
    else
        return pfxp + 1;
}




PIFX PEA::PIFXNextFixupDirectory  (PIFX pifx)
{
    //
    // The total bytes in fixup blocks is stored in the optional header
    // data directory header for IMAGE_DIRECTORY_ENTRY_BASERELOC.
    //

    IMAGE_DATA_DIRECTORY   dd       = m_poh->DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC] ;
    DWORD                  cbBlocks = dd.Size ;

    //
    // The base address of all blocks is the Data Directory itself, in
    // the .reloc section
    //

    DWORD dwBaseBlocks = (DWORD) ImageDirectoryPointer (0, IMAGE_DIRECTORY_ENTRY_BASERELOC) ;

    DWORD dwTooFar     = dwBaseBlocks + cbBlocks ;
    DWORD dwCheck      = ((DWORD)(pifx)) + pifx->SizeOfBlock ;

    if (dwCheck >= dwTooFar)
        return NULL;
    else
        return ((PIFX)(dwCheck)) ;
}




//--------------------------------------------------------------------------------
//
//  $$$$$$  $$$$$$$  $$$$     $$$   $$  $$ $$$$$$    $$$$  $$$$$$$   $$$$
//   $$  $$  $$   $ $$  $$   $$ $$  $$  $$  $$  $$  $$  $$  $$   $  $$  $$
//   $$  $$  $$ $   $$$     $$   $$ $$  $$  $$  $$ $$       $$ $    $$$
//   $$$$$   $$$$    $$$    $$   $$ $$  $$  $$$$$  $$       $$$$     $$$
//   $$ $$   $$ $      $$$  $$   $$ $$  $$  $$ $$  $$       $$ $       $$$
//   $$  $$  $$   $ $$  $$   $$ $$   $$$$   $$  $$  $$  $$  $$   $  $$  $$
//  $$$  $$ $$$$$$$  $$$$     $$$     $$   $$$  $$   $$$$  $$$$$$$   $$$$
//
//--------------------------------------------------------------------------------


#pragma UNDONE(Must handle resources since hacker may store code there)

DWORD     PEA::NumberOfResources ()
{
    PIMAGE_RESOURCE_DIRECTORY          prdRoot, prdType;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY    prde;
    DWORD                              nCnt=0, i;


    /* get root directory of resource tree */
    if ((prdRoot = (PIMAGE_RESOURCE_DIRECTORY)ImageDirectoryPointer
                    (0, IMAGE_DIRECTORY_ENTRY_RESOURCE)) == NULL)
        return 0;

    /* set pointer to first resource type entry */
    prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)prdRoot + sizeof (IMAGE_RESOURCE_DIRECTORY));

    /* loop through all resource directory entry types */
    for (i=0; i<prdRoot->NumberOfIdEntries; i++)
    {
        /* locate directory or each resource type */
        prdType = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)prdRoot + (DWORD)prde->OffsetToData);

        /* mask off most significant bit of the data offset */
        prdType = (PIMAGE_RESOURCE_DIRECTORY)((DWORD)prdType ^ 0x80000000);

        /* increment count of name'd and ID'd resources in directory */
        nCnt += prdType->NumberOfNamedEntries + prdType->NumberOfIdEntries;

        /* increment to next entry */
        prde++;
    }

    return nCnt;
}


char * rgszRsrcNames[] =
{
    "CURSOR",
    "BITMAP",
    "ICON",
    "MENU",
    "DIALOG",
    "STRING",
    "FONTDIR",
    "FONT",
    "ACCELERATOR",
    "RCDATA",
    "MESSAGETABLE"
};


#define CRSRCNAMES (sizeof(rgszRsrcNames)/sizeof(rgszRsrcNames[0]))

/* name each type of resource in the section */
DWORD     PEA::GetListOfResourceTypes (char **pszResTypes)
{
    PIMAGE_RESOURCE_DIRECTORY           prdRoot;
    PIMAGE_RESOURCE_DIRECTORY_ENTRY     prde;
    char                               *pMem;
    DWORD                               nCnt, i;

    /* get root directory of resource tree */
    if ((prdRoot = (PIMAGE_RESOURCE_DIRECTORY)ImageDirectoryPointer
                    (0, IMAGE_DIRECTORY_ENTRY_RESOURCE)) == NULL)
        return 0;

    /* allocate enuff space from heap to cover all types */
    nCnt = prdRoot->NumberOfIdEntries * (MAXRESOURCENAME + 1);

    *pszResTypes = pMem = (char *) Alloc (nCnt);

    /* set pointer to first resource type entry */
    prde = (PIMAGE_RESOURCE_DIRECTORY_ENTRY)((DWORD)prdRoot + sizeof (IMAGE_RESOURCE_DIRECTORY));

    /* loop through all resource directory entry types */
    for (i=0; i<prdRoot->NumberOfIdEntries; i++)
    {
        if ((prde->Name > 0) && (prde->Name <= CRSRCNAMES))
        {
            strcpy (pMem, rgszRsrcNames[prde->Name - 1]);
            pMem += strlen (pMem) + 1;
        }
        /*/
        if (LoadString (hDll, prde->Name, pMem, MAXRESOURCENAME))
            pMem += strlen (pMem) + 1;
        /**/

        prde++;
    }

    return nCnt;
}


//--------------------------------------------------------------------------------
//
//  $$$$$   $$$$$$$ $$$$$$   $$  $$    $$$$
//   $$ $$   $$   $  $$  $$  $$  $$   $$  $$
//   $$  $$  $$ $    $$  $$  $$  $$  $$
//   $$  $$  $$$$    $$$$$   $$  $$  $$
//   $$  $$  $$ $    $$  $$  $$  $$  $$  $$$
//   $$ $$   $$   $  $$  $$  $$  $$   $$  $$
//  $$$$$   $$$$$$$ $$$$$$   $$$$$$    $$$$$
//
//--------------------------------------------------------------------------------


/* function indicates whether debug  info has been stripped from file */
BOOL    PEA::IsDebugInfoStripped ()
{
    PIMAGE_FILE_HEADER    pfh;

    pfh = (PIMAGE_FILE_HEADER)PEFHDROFFSET (m_lpMappedFile);

    return (pfh->Characteristics & IMAGE_FILE_DEBUG_STRIPPED);
}




/* retrieve the module name from the debug misc. structure */
DWORD    PEA::RetrieveModuleName (char **pszModule)
{

    PIMAGE_DEBUG_DIRECTORY    pdd;
    PIMAGE_DEBUG_MISC         pdm = NULL;
    DWORD                     nCnt = 0 ;

    pdd = (PIMAGE_DEBUG_DIRECTORY)ImageDirectoryPointer (0, IMAGE_DIRECTORY_ENTRY_DEBUG) ;

    if (! pdd)
        return 0;

    while (pdd->SizeOfData)
    {
        if (pdd->Type == IMAGE_DEBUG_TYPE_MISC)
        {
            pdm = (PIMAGE_DEBUG_MISC)((DWORD)pdd->PointerToRawData + (DWORD)m_lpMappedFile);

            nCnt  = lstrlen((const char*)pdm->Data);
            nCnt *= pdm->Unicode ? 2 : 1;
            nCnt += 1;

            *pszModule = (char *) Alloc (nCnt);

            CopyMemory (*pszModule, pdm->Data, nCnt);

            break;
        }

        pdd ++;
    }

    if (pdm != NULL)
        return nCnt;
    else
        return 0;
}





/* determine if this is a valid debug file */
BOOL    PEA::IsDebugFile ()
{
    PIMAGE_SEPARATE_DEBUG_HEADER    psdh;

    psdh = (PIMAGE_SEPARATE_DEBUG_HEADER)m_lpMappedFile;

    return (psdh->Signature == IMAGE_SEPARATE_DEBUG_SIGNATURE);
}




/* copy separate debug header structure from debug file */
BOOL    PEA::GetSeparateDebugHeader (PIMAGE_SEPARATE_DEBUG_HEADER psdh)
{
    PIMAGE_SEPARATE_DEBUG_HEADER    pdh;

    pdh = (PIMAGE_SEPARATE_DEBUG_HEADER)m_lpMappedFile;

    if (pdh->Signature == IMAGE_SEPARATE_DEBUG_SIGNATURE)
    {
        CopyMemory ((LPVOID)psdh, (LPVOID)pdh, sizeof (IMAGE_SEPARATE_DEBUG_HEADER));
        return TRUE;
    }

    return FALSE;
}

