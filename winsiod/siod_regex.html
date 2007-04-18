<!doctype html public "-//IETF//DTD HTML//EN//2.0">
<!-- $Id: siod_regex.html,v 1.1 1997/12/01 15:33:57 gjc Exp $ -->
<HEAD>
<TITLE>Notes on the SIOD port of Henry Spencer's REGEX</TITLE>
</HEAD>
<BODY>
<H1>Notes on the SIOD port of Henry Spencer's REGEX</H1>

<P>See <a href="http://alum.mit.edu/www/gjc/winregex.html">winregex.html</a>
for a description of regular expressions and
why this port was originally
done, which was to support the use of regular expressions
from C programs such as SIOD in the WIN32 environment.

<P>However, because system-supplied regular expression support is
still generally missing from some Unix environments, and also from
VMS, it has proven to be more convenient to bundle the hs_regex
package with siod itself.  Yes, you'll find nearly the same code in
the Apache web server, and perhaps a dozen other places.

<P>The following source files are utilized and are covered by
the copyright noted at the end of this document:
<BLOCKQUOTE><PRE><TT>
cclass.h      regcomp.c     regex2.h      regfree.c
cname.h       regerror.c    utils.h       engine.c      
regex.h       regexec.c
</TT></PRE></BLOCKQUOTE>

<P>Preformatted manual-style pages:
<PRE><TT>

NAME
     regcomp, regexec,  regerror,  regfree  -  regular-expression
     library

SYNOPSIS
     #include &lt;sys/types.h&gt;
     #include &lt;regex.h&gt;

     int regcomp(regex_t *preg, const char *pattern, int cflags);

     int regexec(const regex_t *preg,         const char *string,
               size_t nmatch, regmatch_t pmatch[], int eflags);

     size_t regerror(int errcode,            const regex_t *preg,
               char *errbuf, size_t errbuf_size);

     void regfree(regex_t *preg);

DESCRIPTION
     These routines implement POSIX  1003.2  regular  expressions
     (``RE''s); see re_format(7).  Regcomp compiles an RE written
     as a string into an  internal  form,  regexec  matches  that
     internal form against a string and reports results, regerror
     transforms error codes from either into human-readable  mes-
     sages,  and  regfree frees any dynamically-allocated storage
     used by the internal form of an RE.

     The header &lt;regex.h&gt; declares two structure  types,  regex_t
     and  regmatch_t,  the former for compiled internal forms and
     the latter for match reporting.  It also declares  the  four
     functions,  a  type regoff_t, and a number of constants with
     names starting with ``REG_''.

     Regcomp compiles the regular  expression  contained  in  the
     pattern  string,  subject to the flags in cflags, and places
     the results in the regex_t structure  pointed  to  by  preg.
     Cflags  is  the  bitwise OR of zero or more of the following
     flags:

     REG_EXTENDED  Compile modern (``extended'') REs, rather than
                   the  obsolete  (``basic'')  REs  that  are the
                   default.

     REG_BASIC     This is a synonym for 0, provided as  a  coun-
                   terpart  to  REG_EXTENDED to improve readabil-
                   ity.

     REG_NOSPEC    Compile with recognition of all special  char-
                   acters  turned  off.   All characters are thus
                   considered  ordinary,  so  the  ``RE''  is   a
                   literal  string.  This is an extension, compa-
                   tible with but not specified by POSIX  1003.2,
                   and  should  be  used with caution in software
                   intended to  be  portable  to  other  systems.
                   REG_EXTENDED and REG_NOSPEC may not be used in
                   the same call to regcomp.

     REG_ICASE     Compile for matching that ignores  upper/lower
                   case distinctions.  See re_format(7).

     REG_NOSUB     Compile for matching  that  need  only  report
                   success or failure, not what was matched.

     REG_NEWLINE   Compile for  newline-sensitive  matching.   By
                   default,  newline  is  a  completely  ordinary
                   character with no special  meaning  in  either
                   REs  or strings.  With this flag, `[^' bracket
                   expressions and `.' never match newline, a `^'
                   anchor  matches the null string after any new-
                   line in the string in addition to  its  normal
                   function,  and the `$' anchor matches the null
                   string before any newline  in  the  string  in
                   addition to its normal function.

     REG_PEND      The regular expression ends, not at the  first
                   NUL,  but just before the character pointed to
                   by the re_endp member of the structure pointed
                   to  by  preg.   The  re_endp member is of type
                   const char *.  This flag permits inclusion  of
                   NULs  in  the RE; they are considered ordinary
                   characters.  This is an extension,  compatible
                   with  but  not  specified by POSIX 1003.2, and
                   should  be  used  with  caution  in   software
                   intended to be portable to other systems.

     When successful, regcomp returns 0 and fills in  the  struc-
     ture  pointed  to  by  preg.   One  member of that structure
     (other  than  re_endp)  is  publicized:   re_nsub,  of  type
     size_t,  contains the number of parenthesized subexpressions
     within the RE (except that the value of this member is unde-
     fined if the REG_NOSUB flag was used).  If regcomp fails, it
     returns a non-zero error code; see DIAGNOSTICS.

     Regexec matches the compiled RE pointed to by  preg  against
     the  string,  subject  to  the  flags in eflags, and reports
     results using nmatch, pmatch, and the returned  value.   The
     RE  must  have  been  compiled  by  a previous invocation of
     regcomp.  The compiled form is not altered during  execution
     of  regexec,  so a single compiled RE can be used simultane-
     ously by multiple threads.

     By default, the NUL-terminated string pointed to  by  string
     is  considered  to  be the text of an entire line, minus any
     terminating newline.  The eflags argument is the bitwise  OR

NAME
     re_format - POSIX 1003.2 regular expressions

DESCRIPTION
     Regular expressions (``RE''s), as defined in  POSIX  1003.2,
     come  in  two  forms:   modern  REs (roughly those of egrep;
     1003.2  calls  these  ``extended''  REs)  and  obsolete  REs
     (roughly  those  of ed; 1003.2 ``basic'' REs).  Obsolete REs
     mostly exist for backward compatibility  in  some  old  pro-
     grams;  they  will  be  discussed at the end.  1003.2 leaves
     some aspects of RE syntax and semantics  open;  ` - '  marks
     decisions on these aspects that may not be fully portable to
     other 1003.2 implementations.

     A (modern) RE is one- or more non-empty- branches, separated
     by  `|'.   It  matches  anything  that  matches  one  of the
     branches.

     A branch is one- or more pieces, concatenated.  It matches a
     match  for  the  first,  followed by a match for the second,
     etc.

     A piece is an atom possibly followed by a single- `*',  `+',
     `?',  or  bound.  An atom followed by `*' matches a sequence
     of 0 or more matches of the atom.  An atom followed  by  `+'
     matches  a  sequence  of  1 or more matches of the atom.  An
     atom followed by `?' matches a sequence of 0 or 1 matches of
     the atom.

     A bound is `{' followed by an unsigned decimal integer, pos-
     sibly  followed by `,' possibly followed by another unsigned
     decimal integer, always followed by `}'.  The integers  must
     lie  between 0 and RE_DUP_MAX (255-) inclusive, and if there
     are two of them, the first may not exceed  the  second.   An
     atom  followed  by  a  bound containing one integer i and no
     comma matches a sequence of exactly i matches of  the  atom.
     An  atom  followed by a bound containing one integer i and a
     comma matches a sequence of i or more matches of  the  atom.
     An  atom followed by a bound containing two integers i and j
     matches a sequence of i through j (inclusive) matches of the
     atom.

     An atom is a regular expression enclosed in `()' (matching a
     match  for  the  regular  expression),  an empty set of `()'
     (matching the null string) - ,  a  bracket  expression  (see
     below),  `.'  (matching any single character), `^' (matching
     the null string at the beginning of a line),  `$'  (matching
     the null string at the end of a line), a `\' followed by one
     of the characters `^.[$()|*+?{\'  (matching  that  character
     taken as an ordinary character), a `\' followed by any other
     character- (matching that character  taken  as  an  ordinary
     character, as if the `\' had not been present-), or a single
     character with no other significance (matching that  charac-
     ter).   A  `{' followed by a character other than a digit is
     an ordinary character, not the beginning of a bound-.  It is
     illegal to end an RE with `\'.

     A bracket expression is a list  of  characters  enclosed  in
     `[]'.   It  normally  matches  any single character from the
     list (but see below).  If  the  list  begins  with  `^',  it
     matches  any  single  character (but see below) not from the
     rest of the  list.   If  two  characters  in  the  list  are
     separated  by  ` -', this is shorthand for the full range of
     characters between those two (inclusive)  in  the  collating
     sequence,  e.g.  `[0-9]' in ASCII matches any decimal digit.
     It is illegal- for two ranges to  share  an  endpoint,  e.g.
     `a-c-e'.   Ranges are very collating-sequence-dependent, and
     portable programs should avoid relying on them.

     To include a literal `]' in the  list,  make  it  the  first
     character  (following a possible `^').  To include a literal
     `-', make it the first or last character, or the second end-
     point  of  a  range.  To use a literal `-' as the first end-
     point of a range, enclose it in `[.' and `.]' to make  it  a
     collating  element (see below).  With the exception of these
     and some combinations using `[' (see next  paragraphs),  all
     other  special characters, including `\', lose their special
     significance within a bracket expression.

     Within a bracket expression, a collating element (a  charac-
     ter,  a multi-character sequence that collates as if it were
     a single character, or a collating-sequence name for either)
     enclosed in `[.' and `.]' stands for the sequence of charac-
     ters of that collating element.  The sequence  is  a  single
     element of the bracket expression's list.  A bracket expres-
     sion containing a multi-character collating element can thus
     match  more  than  one  character,  e.g.  if  the  collating
     sequence includes a `ch'  collating  element,  then  the  RE
     `[[.ch.]]*c' matches the first five characters of `chchcc'.

     Within a bracket expression, a collating element enclosed in
     `[='  and  `=]'  is  an  equivalence class, standing for the
     sequences of characters of all collating elements equivalent
     to  that  one,  including  itself.   (If  there are no other
     equivalent collating elements, the treatment is  as  if  the
     enclosing delimiters were `[.' and `.]'.)  For example, if o
     and  ^  are  the  members  of  an  equivalence  class,  then
     `[[=o=]]',  `[[=^=]]',  and  `[o^]'  are all synonymous.  An
     equivalence class may not- be an endpoint of a range.

     Within a bracket expression, the name of a  character  class
     enclosed in `[:' and `:]' stands for the list of all charac-
     ters belonging to  that  class.   Standard  character  class
     names are:

          alnum       digit       punct
          alpha       graph       space
          blank       lower       upper
          cntrl       print       xdigit

     These stand for the character classes defined  in  ctype(3).
     A  locale  may provide others.  A character class may not be
     used as an endpoint of a range.

     There are two special cases- of  bracket  expressions:   the
     bracket  expressions  `[[:&lt;:]]' and `[[:&gt;:]]' match the null
     string at the beginning and end of a word  respectively.   A
     word  is  defined  as a sequence of word characters which is
     neither preceded nor followed by word  characters.   A  word
     character  is an alnum character (as defined by ctype(3)) or
     an underscore.  This is an extension,  compatible  with  but
     not  specified by POSIX 1003.2, and should be used with cau-
     tion in software intended to be portable to other systems.

     In the event that an RE could match more than one  substring
     of  a given string, the RE matches the one starting earliest
     in the string.  If the RE could match  more  than  one  sub-
     string  starting  at  that  point,  it  matches the longest.
     Subexpressions also match the longest  possible  substrings,
     subject to the constraint that the whole match be as long as
     possible, with subexpressions starting  earlier  in  the  RE
     taking   priority  over  ones  starting  later.   Note  that
     higher-level subexpressions thus take  priority  over  their
     lower-level component subexpressions.

     Match lengths are measured in characters, not collating ele-
     ments.   A null string is considered longer than no match at
     all.  For example, `bb*' matches the three middle characters
     of  `abbbc',  `(wee|week)(knights|nights)'  matches  all ten
     characters of `weeknights', when `(.*).*' is matched against
     `abc'  the  parenthesized  subexpression  matches  all three
     characters, and when `(a*)*' is matched  against  `bc'  both
     the  whole  RE and the parenthesized subexpression match the
     null string.

     If case-independent matching is  specified,  the  effect  is
     much  as  if  all  case  distinctions  had vanished from the
     alphabet.  When an alphabetic that exists in multiple  cases
     appears  as  an ordinary character outside a bracket expres-
     sion, it is effectively transformed into a  bracket  expres-
     sion  containing  both cases, e.g. `x' becomes `[xX]'.  When
     it appears inside a bracket expression,  all  case  counter-
     parts  of  it  are  added to the bracket expression, so that
     (e.g.) `[x]' becomes `[xX]' and `[^x]' becomes `[^xX]'.

     No particular limit is imposed on the length of REs-.   Pro-
     grams  intended  to be portable should not employ REs longer
     than 256 bytes, as an implementation can  refuse  to  accept
     such REs and remain POSIX-compliant.

     Obsolete (``basic'') regular expressions differ  in  several
     respects.   `|',  `+',  and  `?' are ordinary characters and
     there is no equivalent for their functionality.  The  delim-
     iters  for  bounds  are  `\{'  and `\}', with `{' and `}' by
     themselves ordinary characters.  The parentheses for  nested
     subexpressions  are `\(' and `\)', with `(' and `)' by them-
     selves ordinary characters.  `^' is  an  ordinary  character
     except  at  the  beginning  of the RE or- the beginning of a
     parenthesized subexpression, `$' is  an  ordinary  character
     except  at  the end of the RE or- the end of a parenthesized
     subexpression, and  `*'  is  an  ordinary  character  if  it
     appears  at  the  beginning  of the RE or the beginning of a
     parenthesized subexpression (after a possible leading  `^').
     Finally,  there  is  one new type of atom, a back reference:
     `\' followed by a non-zero decimal digit d matches the  same
     sequence  of  characters  matched  by  the dth parenthesized
     subexpression (numbering subexpressions by the positions  of
     their  opening  parentheses,  left to right), so that (e.g.)
     `\([bc]\)\1' matches `bb' or `cc' but not `bc'.

SEE ALSO
     regex(3)

     POSIX 1003.2, section 2.8 (Regular Expression Notation).

BUGS
     Having two kinds of REs is a botch.

     The current 1003.2 spec says that `)' is an ordinary charac-
     ter in the absence of an unmatched `('; this was an uninten-
     tional result of a wording  error,  and  change  is  likely.
     Avoid relying on it.

     Back references are a dreadful botch, posing major  problems
     for  efficient  implementations.   They  are  also  somewhat
     vaguely defined  (does  `a\(\(b\)*\2\)*d'  match  `abbbd'?).
     Avoid using them.

     1003.2's  specification  of  case-independent  matching   is
     vague.   The ``one case implies all cases'' definition given
     above is current consensus  among  implementors  as  to  the
     right interpretation.

     The syntax for word boundaries is incredibly ugly.


</TT></PRE>

<P>
<BLOCKQUOTE><PRE><TT>
Copyright 1992, 1993, 1994 Henry Spencer.  All rights reserved.
This software is not subject to any license of the American Telephone
and Telegraph Company or of the Regents of the University of California.

Permission is granted to anyone to use this software for any purpose on
any computer system, and to alter it and redistribute it, subject
to the following restrictions:

1. The author is not responsible for the consequences of use of this
   software, no matter how awful, even if they arise from flaws in it.

2. The origin of this software must not be misrepresented, either by
   explicit claim or by omission.  Since few users ever read sources,
   credits must appear in the documentation.

3. Altered versions must be plainly marked as such, and must not be
   misrepresented as being the original software.  Since few users
   ever read sources, credits must appear in the documentation.

4. This notice may not be removed or altered.

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
/*-
 * Copyright (c) 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)COPYRIGHT	8.1 (Berkeley) 3/16/94
 */

</TT></PRE></BLOCKQUOTE>

</BODY>
