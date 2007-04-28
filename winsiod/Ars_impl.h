/*
 From Brian Beckman's siodffi.zip distribution.
*/
    //================================================================
   //
  // ARS_IMPL.h -- Implementation of Address Range Set
 //
//----------------------------------------------------------------
//
// Naive implementation -- a singly linked list of 
// (AddrBase cPage) pairs.
//
//================================================================
    //----------------------------------------------------------------
   //
  // LAND -- Address Range List Node                          
 //
//----------------------------------------------------------------
TYPEDEF (ARG_LIST_NODE, LAND)

struct s_ARG_LIST_NODE
{
	//
	// member variables
	//
public:
	ARG   arg;
	PLAND plandNext;

	//
	// management overhead
	//
public:
	// ctors can have the tla typedef name, but not dtors!
	inline LAND () : arg(0,0,0), plandNext(NULL) {}
	inline LAND (DWORD bs, DWORD cpg, DWORD dwAttr) : arg(bs, cpg, dwAttr), plandNext(NULL) {}
	inline ~s_ARG_LIST_NODE () {plandNext = NULL;}
};

    //----------------------------------------------------------------
   //
  // LARG -- Address Range Lists (yet another bloody list ADT)
 //
//----------------------------------------------------------------
TYPEDEF (ARG_LIST, LARG)

#pragma UNDONE(ARG_LISTs are not thread-safe)

struct s_ARG_LIST
{
	//
	// member variables
	//
private:
	HANDLE m_hHeap;
public:
	PLAND plandHead;

	//
	// management overhead
	//
public:
	inline void Init (HANDLE hHeap) {m_hHeap = hHeap; plandHead = NULL;}
	
	inline void Clear ()
	{
		PLAND p = Pop () ;
		while (p)
		{
			if (! HeapFree (m_hHeap, 0, p))
				SimpleException (SE_HEAPFREE);

			p = Pop () ;
		}
	}

	inline void Clear (HANDLE hHeap)
	{
		PLAND p = Pop () ;
		while (p)
		{
			p->arg.FreeAttributes (hHeap);

			if (! HeapFree (m_hHeap, 0, p))
				SimpleException (SE_HEAPFREE);

			p = Pop () ;
		}
	}

	//
	// semantics
	//
public:
	inline void Push (PLAND ln) 
	{
		ln->plandNext = plandHead; 
	    plandHead     = ln;
	}

	inline PLAND Pop () 
	{
		if (!plandHead)
			return plandHead;

		PLAND p      = plandHead; 
	    plandHead    = plandHead->plandNext; 
		p->plandNext = NULL; 
		
		return p;
	}

	//
	// Maintain the list in increasing order, forbidding overlaps.
	//
	inline void Insert (PLAND ln) 
	{
		if (ln == NULL)
			SimpleException (SE_INVALID_INPUT) ;

		if (plandHead)
		{
			//
			// Linear search
			//
			PLAND pprev = NULL;
			PLAND that = plandHead; 
			while (that)
			{
				if (ln->arg < that->arg)
				{
					//
					// Insert ln just before that. DON'T MERGE!!!! because
					// the attributes may differ (and often do).
					//
					ln->plandNext = that;
					if (pprev)
						pprev->plandNext = ln;
					else
						plandHead = ln;
					return;
				}
				if (ln->arg == that->arg)
				{
					//
					// The candidate is already in the set.
					//
					return;
				}
				if (ln->arg <= that->arg)
				{
					SimpleException(SE_ARS_OVERLAP);
				}
				if (that->plandNext == NULL)
				{
					//
					// Reached the end of the list.
					//
					ln->plandNext = NULL;
					that->plandNext = ln;
					return;
				}

				//
				// Keep searching
				//
				pprev = that;
				that = that->plandNext;
			}
		}
		else
		{
			Push (ln);
		}
	}
};
