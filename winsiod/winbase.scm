;; From Brian Beckman's siodffi.zip distribution.
(require "syslibs.scm")
(require "structs.scm")

(define MAX_PATH 260)

;typedef struct _FILETIME {
;    DWORD dwLowDateTime;
;    DWORD dwHighDateTime;
;} FILETIME, *PFILETIME, *LPFILETIME;

(define FILETIME
  (struct 'FILETIME
          '(dwLowDateTime dwHighDateTime)))


;typedef struct _WIN32_FIND_DATAA {
;    DWORD dwFileAttributes;
;    FILETIME ftCreationTime;
;    FILETIME ftLastAccessTime;
;    FILETIME ftLastWriteTime;
;    DWORD nFileSizeHigh;
;    DWORD nFileSizeLow;
;    DWORD dwReserved0;
;    DWORD dwReserved1;
;    CHAR   cFileName[ MAX_PATH ];
;    CHAR   cAlternateFileName[ 14 ];
;#ifdef _MAC
;    DWORD dwFileType;
;    DWORD dwCreatorType;
;    WORD  wFinderFlags;
;#endif
;} WIN32_FIND_DATAA, *PWIN32_FIND_DATAA, *LPWIN32_FIND_DATAA;

(define WIN32_FIND_DATAA
  (struct 'WIN32_FIND_DATAA
          `(dwFileAttributes
            (ftCreationTime     . 8)
            (ftLastAccessTime   . 8)
            (ftLastWriteTime    . 8)
            nFileSizeHigh
            nFileSizeLow
            dwReserved0
            dwReserved1
            (cFileName          . ,MAX_PATH)
            (cAlternateFileName . 14))))

            
