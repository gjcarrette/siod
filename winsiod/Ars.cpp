/*
 From Brian Beckman's siodffi.zip distribution.
*/
    //================================================================
   //
  // ARS.cpp -- implementation of Address Range Set and friends
 //
//================================================================

#include <windows.h>
#include <crtdbg.h>

#include "ars.h"

    //----------------------------------------------------------------
   //
  // ARS member function implmentations.
 //
//----------------------------------------------------------------

void ARG::Clear (HANDLE hHeap)
{
	m_dwBase = 0;
	m_cPages = 0;
	//__asm {int 3}
	BOOL f = HeapFree (hHeap, 0, (PVOID)m_dwAttributes);
	if (f == FALSE)
	{
		SimpleException (SE_HEAPFREE);
	}
	m_dwAttributes = 0;
}



void ARG::Clear() 
{
	m_dwBase       = 0; 
	m_cPages       = 0; 
	m_dwAttributes = 0;

	CheckInvariant();
}



DWORD ARS::Cardinality()
{
	DWORD d = 0;

	PLAND p = larg.plandHead;
	while (p)
	{
		d++;
		p = p->plandNext;
	}

	return d;
}



DWORD ARS::CBytes()
{
	DWORD d = 0;

	PLAND p = larg.plandHead;
	while (p)
	{
		d += p->arg.CBytes();
		p = p->plandNext;
	}

	return d;
}



DWORD ARS::CPages()
{
	DWORD d = 0;

	PLAND p = larg.plandHead;
	while (p)
	{
		d += p->arg.CPages ();
		p = p->plandNext;
	}

	return d;
}



BOOL ARS::FOverlapsAny (DWORD dwBase, DWORD cPages, PARG* ppargWhich)
{
	ARG arg(dwBase, cPages, 0);

	PLAND p = larg.plandHead;
	while (p)
	{
		if (arg.FOverlaps(p->arg))
		{
			if (ppargWhich)
				*ppargWhich = &p->arg;
			return TRUE;
		}
		p = p->plandNext;
	}

	if (ppargWhich)
		*ppargWhich = NULL;

	return FALSE;
}



BOOL ARS::FContains (DWORD dwPoint, PARG* ppargWhich)
{
	PLAND p = larg.plandHead;
	while (p)
	{
		if (p->arg.FContains(dwPoint))
		{
			if (ppargWhich)
				*ppargWhich = &p->arg;
			return TRUE;
		}
		p = p->plandNext;
	}

	if (ppargWhich)
		*ppargWhich = NULL;

	return FALSE;
}



void ARS::ForEach (PfnParg pfnParg)
{
	PLAND p = larg.plandHead;
	while (p)
	{
		pfnParg (&p->arg);
		p = p->plandNext;
	}
}



void ARS::AddAddressRangeToSet (DWORD dwBase, DWORD cPages, DWORD dwAttributesIn)
{
	PLAND pland = (PLAND)HeapAlloc (m_hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, sizeof(LAND)); 

	pland->arg.SetDwBase       (dwBase);
	pland->arg.SetCPages       (cPages);
	pland->arg.SetDwAttributes (dwAttributesIn);
	
	larg.Insert (pland);

	return;
}



PARS ARS::Complement (DWORD dwAttributesInOptional)
{
	PARS r = (PARS)HeapAlloc (m_hHeap, HEAP_GENERATE_EXCEPTIONS | HEAP_ZERO_MEMORY, sizeof(ARS));

	//__asm {int 3}
	r->Init (m_hHeap);

	//
	// Since the set is ordered, computing the complement is relatively easy
	// These two variables represent CANDIDATES for the complement. We'll walk
	// the existing set adjusting, rejecting, or accepting candidates.
	//
	DWORD base;
	DWORD cpag;

	DWORD cbDiff;

	//
	// The following is the complement of the empty space (it's the full space)
	//
	base = 0;
	cpag = g_cPagesFullSpace;

	for 
	(
		PLAND p = larg.plandHead;
		p;
		p = p->plandNext
	)
	{
		if (p->arg.DwBase () == base)
		{
			// 
			// Then our set already contains something whose base equals that of our 
			// candidate.  The candidate must not go in the complement.  
			// move on to the next candidate.
			//
		}
		else if (p->arg.DwBase () > base)
		{
			//
			// Adjust and accept this candidate
			//
			cbDiff = p->arg.DwBase () - base;

			if (cbDiff & g_dwPageMask)
				SimpleException (SE_MALFORMED_SET);

			cpag = cbDiff / g_cbPageSize;

			r->AddAddressRangeToSet (base, cpag, dwAttributesInOptional);
		}
		else
		{
			SimpleException(SE_CANT_HAPPEN);
		}

		//
		// Set base & cpag of new candidate
		//
		base = p->arg.DwBase () + (p->arg.CPages () * g_cbPageSize);
		cpag = ((0xFFFFFFFF - base) + 1);

		_ASSERTE ((cpag & g_dwPageMask) == 0);

		cpag /= g_cbPageSize;
	}

	//
	// Don't forget to add the last one to the set
	//

	r->AddAddressRangeToSet (base, cpag, dwAttributesInOptional);

	return r;
}



void ARS::DeleteComplement(PARS pars)
{
	pars->Clear(m_hHeap);
	//__asm {int 3}
	if (! HeapFree (m_hHeap, 0, pars))
		SimpleException (SE_HEAPFREE);
}



PARS ARS::Intersection (LPCARS)
{
	SimpleException (SE_NYI);
	return NULL;
}



PARS ARS::Union (LPCARS)
{
	SimpleException (SE_NYI);
	return NULL;
}



PARS ARS::BKSRequestFromPEFile (LPCSTR)
{
	SimpleException (SE_NYI);
	return NULL;
}
