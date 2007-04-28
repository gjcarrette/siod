/*
 From Brian Beckman's siodffi.zip distribution.
*/
    //================================================================
   //
  // ARS.h -- Address Range Set
 //
//================================================================
//
// Client code will not link if precompiled headers used here.
//
//================================================================

#ifndef __ARS_H__
#define __ARS_H__

#include "bblutil.h"

    //----------------------------------------------------------------
   //
  // ARG -- Address Range
 //
//----------------------------------------------------------------
TYPEDEF(ADDRESS_RANGE, ARG)

struct s_ADDRESS_RANGE
{
    //
    // Member variables
    //
private:
    union
    {
        PVOID   m_pvBase;
        DWORD   m_dwBase;
    };
    DWORD   m_cPages;
    DWORD   m_dwAttributes; // Use as page protection or pointer to other struct

    //
    // management overhead
    //
private:
    inline void CheckInvariant () 
    {
        if (DwLastByte() < m_dwBase) 
            SimpleException (SE_ADDRESS_WRAP);
    }

public:
    //
    // An invariant must be maintained: DwLastByte >= dwBase.  In other words, you
    // can't have so many pages in an ARG that it wraps the address space.
    //
    inline ARG (DWORD i_dwBase, DWORD i_cPages, DWORD dwAttrsIn)  
    {
        m_dwBase       = DwRoundDown(i_dwBase); 
        m_cPages       = i_cPages;
        m_dwAttributes = dwAttrsIn;
        
        CheckInvariant ();
    }

    outline void Clear () ;
    outline void Clear (HANDLE hHeap);
    
    inline ARG              () {Clear();}

    inline ~s_ADDRESS_RANGE () {Clear();}

    //
    // Deep-copy operators
    //
    ARG(CARG & that) 
    {
        m_dwBase       = that.m_dwBase; 
        m_cPages       = that.m_cPages;
        m_dwAttributes = that.m_dwAttributes ;
    }

    inline ARG & operator=(CARG & that) 
    {
        m_dwBase       = that.m_dwBase; 
        m_cPages       = that.m_cPages;
        m_dwAttributes = that.m_dwAttributes ;
        return *this;
    }

    //
    // semantics
    //
private:
    inline DWORD DwRoundDown(DWORD dwByteInPage) {return dwByteInPage & (~(g_cbPageSize-1));}

public:
    
    //
    // Setters & Getters
    //
    inline DWORD CPages          ()           {return m_cPages                ; }
    inline DWORD DwBase          ()           {return m_dwBase                ; }
    inline PVOID PvBase          ()           {return m_pvBase                ; }
    inline DWORD CBytes          ()           {return m_cPages * g_cbPageSize ; }
    inline DWORD DwLastByte      ()           {return m_dwBase + CBytes()-1   ; }
    inline DWORD DwLastPage      ()           {return m_dwBase + m_cPages-1   ; }
    inline DWORD DwAttributes    ()           {return m_dwAttributes          ; }
    inline PVOID PvAttributes    ()           {return (PVOID)m_dwAttributes   ; }
    inline VOID  SetDwBase       (DWORD dwIn) {m_dwBase       = dwIn          ; }
    inline VOID  SetCPages       (DWORD dwIn) {m_cPages       = dwIn          ; }
    inline VOID  SetDwAttributes (DWORD dwIn) {m_dwAttributes = dwIn          ; }
    inline VOID  SetPvAttributes (PVOID pvIn) {m_dwAttributes = (DWORD)pvIn   ; }
    inline VOID  FreeAttributes  (HANDLE hHp) 
    {
        if (m_dwAttributes && (! HeapFree (hHp, 0, (PVOID)m_dwAttributes)))
            SimpleException (SE_HEAPFREE);
		m_dwAttributes = 0 ;
    }

    //
    // Algebra (Comparison operators)
    //
    // Theory: The algebra of ARGS is not the same as integers, since overlap is possible.  
    // For example, Let A and B be two ARGs.  If A < B, then A is not <= B, unlike numbers.  
    // The reason is that the latter condition implies overlap.  
    //
    // The following conditions may obtain:
    //
    // A <  B       A.LastByte < B.base
    // A <= B       A.base <= B.base && A.LastByte >= B.base (i.e., there is some overlap)
    // A == B       A.base == B.base && A.cPages == B.cPages
    // A >= B       A.base >= B.base && A.base <= B.LastByte (i.e., there is some overlap)
    // A >  B       A.base > B.LastByte
    //
    // Note: a zero ARG is allowed.  It has a base of 0 and a cPages of 0.  A zero-length
    // ARG is also allowed.  It has a non-zero base and a cPages of 0.
    //
    // Here's a bit more of the algebra (though not completely exhaustive -- that would 
    // require all 25 combinations of integer >= and <= operations on the member variables:
    //
    //  integer algebra                                      ARG algebra
    // ==============================================       ==================================
    // A.base <  B.base  &&  A.lastByte <  B.lastByte  -->  (A <  B  ||  A <= B)  &&  (A != B)
    // A.base == B.base  &&  A.lastByte <  B.lastByte  -->  (A <= B  &&  B != 0)  &&  (A != B)
    // A.base >  B.base  &&  A.lastByte <  B.lastByte  -->  (B <= A  &&  B != 0)  &&  (A != B)
    // ----------------------------------------------       ----------------------------------
    // A.base <  B.base  &&  A.lastByte == B.lastByte  -->  (A <= B  &&  A != B)
    // A.base == B.base  &&  A.lastByte == B.lastByte  -->  (A == B)
    // A.base >  B.base  &&  A.lastByte == B.lastByte  -->  (A >= B  &&  A != B)
    // ----------------------------------------------       ----------------------------------
    // A.base <  B.base  &&  A.lastByte >  B.lastByte  -->  (A <= B  &&  A != 0)  &&  (A != B)
    // A.base == B.base  &&  A.lastByte >  B.lastByte  -->  (A >= B) &&  A != 0)  &&  (A != B)
    // A.base >  B.base  &&  A.lastByte >  B.lastByte  -->  (A >= B  ||  A >  B)  &&  (A != B)
    // ==============================================       ==================================
    //
    // Here are some invariants to ASSERT:
    //
    // NOT { (A == B) and [ (A < B) or (A > B) ] }
    // NOT { (A == B) and [ (FIsEmpty(A) && NOT FIsEmpty(B))  ||  
    //                      (NOT FIsEmpty(A) && FIsEmpty(B)) ] }
    // NOT { (A <  B) and (A <= B) }
    // NOT { (A >  B) and (A >= B) }
    // NOT { (A <  B) and (A >  B) }
    //
    //   the following CAN happen if two ARGs have the same base
    //     { (A <= B) and (A >= B) }
    //
private:
    inline void AssertPackage(ARG & that)
    {
        _ASSERT
        (
            ! ( ( *this == that ) && (( *this < that ) || ( *this > that )) )
        );
        _ASSERT
        (
            ! (    ( *this == that )
                && (    (!FIsEmpty() &&  that.FIsEmpty())
                     || ( FIsEmpty() && !that.FIsEmpty())
                   )
              )
        );
        _ASSERT (   ! ( (*this <  that) && (*this <= that))   );
        _ASSERT (   ! ( (*this >  that) && (*this >= that))   );
        _ASSERT (   ! ( (*this >  that) && (*this <  that))   );
    }

public:
    inline BOOL operator< (ARG & that) {return DwLastByte() < that.m_dwBase;}
    inline BOOL operator<=(ARG & that) {return (m_dwBase <= that.m_dwBase) && (DwLastByte() >= that.m_dwBase);}
    inline BOOL operator==(ARG & that) {return (m_dwBase == that.m_dwBase) && (m_cPages == that.m_cPages);}
    inline BOOL operator!=(ARG & that) {return ! (*this == that);}
    inline BOOL operator>=(ARG & that) {return (m_dwBase >= that.m_dwBase) && (m_dwBase <= that.DwLastByte());}
    inline BOOL operator> (ARG & that) {return m_dwBase > that.DwLastByte();}

    inline BOOL FIsNull  () {return (m_dwBase == 0) && (m_cPages == 0);}
    inline BOOL FIsEmpty () {return m_cPages == 0;}
    inline BOOL FOverlaps (ARG & that) {AssertPackage(that); return ((*this >= that) || (*this <= that));}
    inline BOOL FContains (DWORD dwPoint) {return (dwPoint >= m_dwBase) && (dwPoint <= DwLastByte());}
};

    //----------------------------------------------------------------
   //
  // ARS -- The main dude
 //
//----------------------------------------------------------------
TYPEDEF (ADDRESS_RANGE_SET, ARS)

typedef void ((*PfnParg)(PARG));        // for the "ForEach" function

//
// Unfortunately, we must now pollute the inteface with the implementation
// (or go to COM)
//
#pragma UNDONE(Clean up ARS definition dependence on implementation)

#include "ars_impl.h"

struct s_ADDRESS_RANGE_SET
{ 
    //
    // member variables
    //
private:
    HANDLE m_hHeap ;
public:
    LARG larg; // singly-linked list of ARGs  YIKES!!! This knows about implementation.

    //
    // management overhead
    //
public:
    inline void Init (HANDLE hHeap)  {m_hHeap = hHeap; larg.Init(m_hHeap);}
    inline void Clear (HANDLE hHeap) {larg.Clear(hHeap);}
    inline void Clear ()             {larg.Clear();}

    //
    // semantics
    //
           DWORD Cardinality          ();
           DWORD CBytes               ();
           DWORD CPages               ();
           void  ForEach              (PfnParg pfnParg);
           BOOL  FOverlapsAny         (DWORD dwBase, DWORD cPages, PARG * ppargWhich);
    inline BOOL  FOverlapsAny         (DWORD dwBase, DWORD cPages) {return FOverlapsAny (dwBase, cPages, NULL);}
           BOOL  FContains            (DWORD dwPoint, PARG * ppargWhich);
    inline BOOL  FContains            (DWORD dwPoint) {return FContains (dwPoint, 0);}
           void  AddAddressRangeToSet (DWORD dwBase, DWORD cPages, DWORD dwAttributesIn);
           PARS  Complement           (DWORD dwAttributesInOptional = 0);
           void  DeleteComplement     (PARS that);
           PARS  Intersection         (LPCARS that);
           PARS  Union                (LPCARS that);
           PARS  BKSRequestFromPEFile (LPCSTR lpModuleFileName);
} ;


#endif // __ARS_H__
