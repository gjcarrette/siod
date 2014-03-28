/* 
   name:    ss.c
   purpose: simple socket interface for siod
   author:  george j. carrette
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#ifdef unix
#include <unistd.h>
#include <sys/time.h>
#endif
#ifdef SUN5
/* missing from unistd.h */
long gethostid(void); 
int gethostname(char *, int);   
#endif
#ifdef VMS
#include <unixio.h>
#define MULTINET
/* #define TCPWARE */
#if defined(MULTINET)
#include "multinet_root:[multinet.include.sys]types.h"
#include "multinet_root:[multinet.include.sys]socket.h"
#include "multinet_root:[multinet.include.netinet]in.h"
#include "multinet_root:[multinet.include]netdb.h"
#include "multinet_root:[multinet.include.arpa]inet.h"
#include "multinet_root:[multinet.include]errno.h"
#define close socket_close
#define errno socket_errno
#endif
#if defined(TCPWARE)
#include <types.h>
#include <socket.h>
#include <in.h>
#include <netdb.h>
#define send socket_send
#define recv socket_recv
#define close socket_close
#endif
#endif
#ifdef unix
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#ifdef WIN32
#include <winsock.h>
#define close closesocket
#endif

#include <setjmp.h>

#ifdef sun
#define       INADDR_NONE             0xffffffff
#endif

#include "siod.h"
#include "ss.h"

static void init_ss_version(void)
{setvar(cintern("*ss-version*"),
	cintern("$Id: ss.c,v 1.7 1998/03/05 13:26:47 gjc Exp $"),
	NIL);}


static long tc_sock_stream = 0;

LISP lgetproto(LISP lproto)
{long iflag,j;
 LISP result = NIL;
 struct protoent *p;
 iflag = no_interrupt(1);
 if ((p = getprotobynumber(get_c_long(lproto))))
   {result = cons(rintern(p->p_name),NIL);
    for(j=0;p->p_aliases[j];++j)
      result = cons(rintern(p->p_aliases[j]),result);}
 no_interrupt(iflag);
 return(nreverse(result));}

LISP lgetservice(LISP lport,LISP lproto)
{long iflag,j;
 LISP result = NIL;
 struct servent *p;
 iflag = no_interrupt(1);
 if ((p = getservbyport(htons((unsigned short) get_c_long(lport)),
			NULLP(lproto) ? NULL : get_c_string(lproto))))
   {result = cons(rintern(p->s_proto),NIL);
    result = cons(rintern(p->s_name),result);
    for(j=0;p->s_aliases[j];++j)
      result = cons(rintern(p->s_aliases[j]),result);}
 no_interrupt(iflag);
 return(nreverse(result));}

LISP s_open(LISP lhost,LISP lport,LISP aflag)
     /* to make these "easy" we have over-encapsulated things
	a bit. at some point rework things, but for now the
	aflag means get things ready to do a listen
	followed by an accept. */
{long iflag;
 int sd,status,save_errno,iv;
 short port;
 LISP s;
 char *hname;
 struct sockaddr_in local, remote;
 struct hostent *hostinfo;
 struct servent *servinfo;
 struct sock_stream *ss;
 iflag = no_interrupt(1);
 if FLONUMP(lport)
   port = (short) get_c_long(lport);
 else if ((servinfo = getservbyname(get_c_string(lport),"tcp")))
   port = ntohs(servinfo->s_port);
 else
   return(err("getservbyname",llast_c_errmsg(-1)));
 memset(&remote,0,sizeof(remote));
 remote.sin_port = htons(port);
 if FLONUMP(lhost)
   {remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = htonl(get_c_long(lhost));}
 else if ((remote.sin_addr.s_addr =
	   inet_addr(hname = get_c_string(lhost))) != INADDR_NONE)
   remote.sin_family = AF_INET;
 else
   {if (!(hostinfo = gethostbyname(hname)))
      err("could not get hostinfo",lhost);
    remote.sin_family = hostinfo->h_addrtype;
    memcpy(&remote.sin_addr.s_addr,hostinfo->h_addr_list[0],
	   hostinfo->h_length);}
 if ((sd = socket(AF_INET,SOCK_STREAM,0)) < 0)
   err("error creating socket",llast_c_errmsg(-1));
 memset(&local,0,sizeof(local));
 local.sin_family = AF_INET;
 local.sin_port = 0;
 local.sin_addr.s_addr = 0;
 if NULLP(aflag)
   {if ((status = bind(sd,(struct sockaddr *)&local,sizeof(local))))
      {save_errno = errno;
       close(sd);
       err("binding socket",llast_c_errmsg(save_errno));}
    if ((status = connect(sd,(struct sockaddr *)&remote,sizeof(remote))))
      {save_errno = errno;
       close(sd);
       err("connect socket",llast_c_errmsg(save_errno));}}
 else
   {iv = 1;
    if (setsockopt(sd,SOL_SOCKET,SO_REUSEADDR,(char*)&iv,sizeof(iv)))
      {save_errno = errno;
       close(sd);
       err("setsockopt SO_REUSEADDR",llast_c_errmsg(save_errno));}
    if ((status = bind(sd,(struct sockaddr *)&remote,sizeof(remote))))
      {save_errno = errno;
       close(sd);
       err("binding socket",llast_c_errmsg(save_errno));}
    if ((status = listen(sd,get_c_long(aflag))))
      {save_errno = errno;
       close(sd);
       err("listen socket",llast_c_errmsg(save_errno));}}
 s = cons(NIL,NIL);
 if (!(ss = (struct sock_stream *) malloc(sizeof(struct sock_stream))))
   {close(sd);
    err("connect, cannot allocate",NIL);}
 ss->sd = sd;
 ss->icnt = 0;
 ss->bufsiz = 1024;
 if (!(ss->ibase = (unsigned char *) malloc(ss->bufsiz)))
   {close(sd);
    free(ss);
    err("connect, cannot allocate",NIL);}
 ss->iptr = ss->ibase;
 if (!(ss->obase = (unsigned char *) malloc(ss->bufsiz)))
   {close(sd);
    free(ss->ibase);
    free(ss);
    err("connect, cannot allocate",NIL);}
 ss->ocnt = ss->bufsiz;
 ss->optr = ss->obase;
 s->type = (short) tc_sock_stream;
 s->storage_as.string.data = (char *)ss;
 s->storage_as.string.dim = 1;
 no_interrupt(iflag);
 return(s);}

LISP gethostbyaddr_l(LISP addr)
{struct hostent *hostinfo;
 unsigned int x;
 x = get_c_long(addr);
 x = htonl(x);
 if (!(hostinfo = gethostbyaddr((char *)&x,sizeof(x),AF_INET)))
   return(NIL);
 return(strcons(strlen(hostinfo->h_name),hostinfo->h_name));}

LISP decode_hostent(struct hostent *p)
{LISP name;
 LISP aliases = NIL,addr_list = NIL,addr;
 int j;
 name = strcons(strlen(p->h_name),p->h_name);
 for(j=0;p->h_aliases && p->h_aliases[j];++j)
   aliases = strcons(strlen(p->h_aliases[j]),p->h_aliases[j]);
 aliases = nreverse(aliases);
 for(j=0;p->h_addr_list && p->h_addr_list[j];++j)
   {addr = arcons(tc_byte_array,p->h_length,0);
    memcpy(addr->storage_as.string.data,p->h_addr_list[j],p->h_length);
    addr_list = cons(addr,addr_list);}
 addr_list = nreverse(addr_list);
 return(listn(4,
	      name,
	      cons(cintern("aliases"),aliases),
	      cons(cintern("addr_list"),addr_list),
	      cons(cintern("addrtype"),flocons(p->h_addrtype))));}

LISP gethostbyname_l(LISP name)
{struct hostent *hostinfo;
 if (!(hostinfo = gethostbyname(get_c_string(name))))
   return(NIL);
 return(decode_hostent(hostinfo));}

LISP inet_addr_l(LISP str)
{unsigned int x;
 double g;
 switch TYPE(str)
   {case tc_byte_array:
      if (str->storage_as.string.dim != 4)
	err("address must be 4 bytes",str);
      x = *((int *)str->storage_as.string.data);
      break;
    default:
      x = inet_addr(get_c_string(str));
      break;}
 if (x != INADDR_NONE)
   {x = ntohl(x);
    g = x;
    return(flocons(g));}
 else
   return(NIL);}

LISP inet_ntoa_l(LISP str)
{char buff[50];
 unsigned int x;
 switch TYPE(str)
   {case tc_byte_array:
      if (str->storage_as.string.dim != 4)
	err("address must be 4 bytes",str);
      sprintf(buff,"%u.%u.%u.%u",
	      str->storage_as.string.data[0],
	      str->storage_as.string.data[1],
	      str->storage_as.string.data[2],
	      str->storage_as.string.data[3]);
      break;
   default:
      x = get_c_long(str);
      sprintf(buff,"%u.%u.%u.%u",
	      ((x & 0xFF000000) >> 24) & 0xFF,
	      ((x & 0x00FF0000) >> 16) & 0xFF,
	      ((x & 0x0000FF00) >> 8) & 0xFF,
	      ((x & 0x000000FF) >> 0) & 0xFF);
      break;}
 return(strcons(-1,buff));}

#ifdef unix

static int select_read_tmo(int sd,double tmo)
{int retval;
 struct timeval timeout;
 fd_set readfds;
 timeout.tv_sec = (time_t) tmo;
 timeout.tv_usec = (long) ((tmo - (double)timeout.tv_sec) * 1.0e6);
 FD_ZERO(&readfds);
 FD_SET(sd,&readfds);
 retval = select(FD_SETSIZE,&readfds,NULL,NULL,&timeout);
 if (retval == 0)
   return(1);
 else if (retval < 0)
   err("select",llast_c_errmsg(-1));
 return(0);}
#else
static int select_read_tmo(int sd,double tmo)
{return(0);}
#endif

LISP s_accept(LISP as,LISP tmo)
{struct sock_stream *ss;
 int iflag,sd;
 LISP s;
 iflag = no_interrupt(1);
 ss = get_ss(as,1);
 if (NNULLP(tmo) && select_read_tmo(ss->sd,get_c_double(tmo)))
   {no_interrupt(iflag);
    return(NULL);}
 if ((sd = accept(ss->sd,NULL,NULL)) < 0)
   err("accept",llast_c_errmsg(-1));
 s = cons(NIL,NIL);
 if (!(ss = (struct sock_stream *) malloc(sizeof(struct sock_stream))))
   {close(sd);
    err("accept, cannot allocate",NIL);}
 ss->sd = sd;
 ss->icnt = 0;
 ss->bufsiz = 1024;
 if (!(ss->ibase = (unsigned char *) malloc(ss->bufsiz)))
   {close(sd);
    free(ss);
    err("connect, cannot allocate",NIL);}
 ss->iptr = ss->ibase;
 if (!(ss->obase = (unsigned char *) malloc(ss->bufsiz)))
   {close(sd);
    free(ss->ibase);
    free(ss);
    err("connect, cannot allocate",NIL);}
 ss->ocnt = ss->bufsiz;
 ss->optr = ss->obase;
 s->type = (short) tc_sock_stream;
 s->storage_as.string.data = (char *)ss;
 s->storage_as.string.dim = 1;
 no_interrupt(iflag);
 return(s);}

struct sock_stream *get_ss(LISP s,long openchk)
{if NTYPEP(s,tc_sock_stream)
   err("not a socket stream",s);
 if (openchk && !s->storage_as.string.dim)
   err("socket is closed",s);
 return((struct sock_stream *) s->storage_as.string.data);}

LISP s_close(LISP s)
{struct sock_stream *ss;
 int iflag,sd;
 iflag = no_interrupt(1);
 ss = get_ss(s,1);
 free(ss->ibase);
 free(ss->obase);
 sd = ss->sd;
 free(ss);
 s->storage_as.string.data = NULL;
 s->storage_as.string.dim = 0;
 if (close(sd))
   err("socket close",llast_c_errmsg(-1));
 no_interrupt(iflag);
 return(NIL);}


LISP s_shutdown(LISP s,LISP flag)
{struct sock_stream *ss;
 int iflag,sd,how;
 if (NULLP(flag))
   how = 2;
 else if (EQ(flag,cintern("read")))
   how = 0;
 else if (EQ(flag,cintern("write")))
   how = 1;
 else
   return(err("shutdown must be () read or write",flag));
 iflag = no_interrupt(1);
 ss = get_ss(s,1);
 sd = ss->sd;
 if (shutdown(sd,how))
   err("socket shutdown",llast_c_errmsg(-1));
 no_interrupt(iflag);
 return(NIL);}

int ss_filbuf(struct sock_stream *ss)
{int status;
 ss->icnt = 0;
 status = recv(ss->sd,ss->ibase,ss->bufsiz,0);
 if (status > 0)
   {ss->iptr = ss->ibase;
    ss->icnt = status;
    --ss->icnt;
    return(*ss->iptr++);}
 else if (status == 0)
   return(EOF);
 else
   {err("recv",llast_c_errmsg(-1));
    return(EOF);}}

static void sent_zero(void)
/* this should never happen, so should be an error */
{
#if defined(WIN32)
	Sleep(100);
#else
	sleep(1);
#endif
}

void ss_force(struct sock_stream *ss)
{int status,size,j;
 size = ss->bufsiz - ((ss->ocnt > 0) ? ss->ocnt : 0);
 ss->ocnt = ss->bufsiz;
 ss->optr = ss->obase;
 for(j=0;size > 0;j += status, size -= status)
   if ((status = send(ss->sd,&ss->obase[j],size,0)) < 0)
     err("send",llast_c_errmsg(-1));
   else if (status == 0)
	 sent_zero();}
 
int ss_flsbuf(int c,struct sock_stream *ss)
{ss_force(ss);
 --(ss)->ocnt;
 *(ss)->optr++ = c;
 return(c);}
 
LISP s_getc(LISP s)
{struct sock_stream *ss = get_ss(s,1);
 int c,iflag;
 iflag = no_interrupt(1);
 c = SS_GETC(ss);
 no_interrupt(iflag);
 return((c == EOF) ? NIL : flocons(c));}

LISP s_icnt(LISP s)
{return(flocons(get_ss(s,1)->icnt));}

LISP s_putc(LISP lc,LISP s)
{struct sock_stream *ss = get_ss(s,1);
 int c = get_c_long(lc),iflag;
 iflag = no_interrupt(1);
 SS_PUTC(c,ss);
 no_interrupt(iflag);
 return(NIL);}

LISP s_puts(LISP str,LISP s)
{struct sock_stream *ss = get_ss(s,1);
 char *data = get_c_string(str);
 int c,iflag;
 iflag = no_interrupt(1);
 while ((c = *data++)) SS_PUTC(c,ss);
 no_interrupt(iflag);
 return(NIL);}

LISP s_write(LISP string,LISP file)
{long flag;
 char *data;
 struct sock_stream *ss = get_ss(file,1);
 long j,dim,len,status;
 data = get_c_string_dim(CONSP(string) ? car(string) : string,&dim);
 len = CONSP(string) ? get_c_long(cadr(string)) : dim;
 if (len <= 0) return(NIL);
 if (len > dim) err("write length too long",string);
 flag = no_interrupt(1);
 if (len < ss->bufsiz)
   /* might as well copy the data to the large buffer */
   for(j=0;j<len;++j)
     SS_PUTC(data[j],ss);
 else
   {ss_force(ss);
    for(j=0;len > 0;j += status, len -= status)
      if ((status = send(ss->sd,&data[j],len,0)) < 0)
	err("send",llast_c_errmsg(-1));
      else if (status == 0)
	sent_zero();}
 no_interrupt(flag);
 return(NIL);}

LISP s_drain(LISP s)
{struct sock_stream *ss = get_ss(s,1);
 int c,iflag;
 iflag = no_interrupt(1);
 while((c = SS_GETC(ss)) != EOF);
 no_interrupt(iflag);
 return(NIL);}

LISP s_gets(LISP str,LISP s)
{struct sock_stream *ss;
 int c,iflag,j;
 char buffer[4096];
 if NULLP(s)
   {s = str;
    str = NIL;}
 ss = get_ss(s,1);
 iflag = no_interrupt(1);
 for(j=0;j<sizeof(buffer);++j)
   {c = SS_GETC(ss);
    if (c == EOF)
      {if (j == 0)
	 {no_interrupt(iflag);
	  return(NIL);}
       break;}
    else if (c == '\n')
      {buffer[j] = c;
       ++j;
       break;}
    else
      buffer[j] = c;}
 no_interrupt(iflag);
 return(strcons(j,buffer));}

LISP s_read(LISP size,LISP file)
{long flag,n,ret,m,maxlen;
 char *buffer;
 LISP s;
 struct sock_stream *ss;
 int c;
 ss = get_ss(file,1);
 flag = no_interrupt(1);
 switch(TYPE(size))
   {case tc_string:
    case tc_byte_array:
      s = size;
      buffer = s->storage_as.string.data;
      n = s->storage_as.string.dim;
      m = 0;
      break;
    case tc_cons:
      s = car(size);
      buffer = get_c_string_dim(s,&maxlen);
      n = get_c_long(cadr(size));
      if (n > maxlen)
	err("buffer not that big",size);
      m = 0;
      break;
    default:
      n = get_c_long(size);
      buffer = (char *) must_malloc(n+1);
      buffer[n] = 0;
      m = 1;}
 ret = 0;
 while((ret < n) && ((c = SS_GETC(ss)) != EOF))
   buffer[ret++] = c;

 if (ret < n) buffer[ret] = 0;

 if (ret == 0)
   {if (m)
      free(buffer);
    no_interrupt(flag);
    return(NIL);}

 if (m)
   {if (ret == n)
      {s = cons(NIL,NIL);
       s->type = tc_string;
       s->storage_as.string.data = buffer;
       s->storage_as.string.dim = n;}
    else
      {s = strcons(ret,NULL);
       memcpy(s->storage_as.string.data,buffer,ret);
       free(buffer);}
    no_interrupt(flag);
    return(s);}
 no_interrupt(flag);
 return(flocons((double)ret));}


LISP s_force_output(LISP s)
{struct sock_stream *ss = get_ss(s,1);
 int iflag;
 iflag = no_interrupt(1);
 ss_force(ss);
 no_interrupt(iflag);
 return(NIL);}

void ss_gc_free(LISP s)
{struct sock_stream *ss;
 ss = get_ss(s,0);
 if (s->storage_as.string.dim)
   {free(ss->ibase);
    free(ss->obase);
    close(ss->sd);
    free(ss);}}

void ss_prin1(LISP s,struct gen_printio *f)
{char buff[512];
 unsigned char *p;
 struct sock_stream *ss;
 struct sockaddr_in a;
 size_t len,j;
 ss = get_ss(s,0);
 if (s->storage_as.string.dim)
   {sprintf(buff,"#{SOCKET %d",ss->sd);
    gput_st(f,buff);
    p = ((unsigned char *)&a.sin_addr.s_addr);
    for(j = 0; j < 2; ++j)
      {len = sizeof(a);
       if (((j == 0)
	    ? getsockname(ss->sd,(struct sockaddr *)&a,&len)
	    : getpeername(ss->sd,(struct sockaddr *)&a,&len))
	   == 0)
	 {sprintf(buff," %d.%d.%d.%d:%d",p[0],p[1],p[2],p[3],
		  ntohs(a.sin_port));
	  gput_st(f,buff);}}
    gput_st(f,"}");}
 else
   gput_st(f,"#{SOCKET CLOSED}");}

#ifndef WIN32

LISP l_getname(int (*fcn)(int fn, struct sockaddr *,size_t *),char *msg,LISP s)
{struct sock_stream *ss = get_ss(s,1);
 struct sockaddr_in a;
 size_t len;
 char buff[512];
 unsigned char *p;
 len = sizeof(a);
 if ((*fcn)(ss->sd,(struct sockaddr *)&a,&len))
   return(err(msg,llast_c_errmsg(-1)));
 p = ((unsigned char *)&a.sin_addr.s_addr);
 sprintf(buff,"%d.%d.%d.%d:%d",(int)p[0],(int)p[1],(int)p[2],(int)p[3],
	 (int)ntohs(a.sin_port));
 return(strcons(-1,buff));}

LISP l_getsockname(LISP s)
{return(l_getname(getsockname,"getsockname",s));}

LISP l_getpeername(LISP s)
{return(l_getname(getpeername,"getpeername",s));}

#else
/* even with declarations cleaned up there are some inscrutable stack
   related data errors in release mode for WIN32 VC++ 6.0 with the
   above code. Things work in DEBUG mode but fail in release mode.
   Unrolling it is a quick fix. */


LISP l_getsockname(LISP s)
{struct sock_stream *ss = get_ss(s,1);
 struct sockaddr_in a;
 size_t len;
 char buff[512];
 unsigned char *p;
 len = sizeof(a);
 if (getsockname(ss->sd,(struct sockaddr *)&a,&len))
   return(err("getsockname",llast_c_errmsg(-1)));
 p = ((unsigned char *)&a.sin_addr.s_addr);
 sprintf(buff,"%d.%d.%d.%d:%d",(int)p[0],(int)p[1],(int)p[2],(int)p[3],
	 (int)ntohs(a.sin_port));
 return(strcons(-1,buff));}

LISP l_getpeername(LISP s)
{struct sock_stream *ss = get_ss(s,1);
 struct sockaddr_in a;
 size_t len;
 char buff[512];
 unsigned char *p;
 len = sizeof(a);
 if (getpeername(ss->sd,(struct sockaddr *)&a,&len))
   return(err("getpeername",llast_c_errmsg(-1)));
 p = ((unsigned char *)&a.sin_addr.s_addr);
 sprintf(buff,"%d.%d.%d.%d:%d",(int)p[0],(int)p[1],(int)p[2],(int)p[3],
	 (int)ntohs(a.sin_port));
 return(strcons(-1,buff));}



#endif
     

int ss_getc_fcn(struct sock_stream *ss)
{int c,iflag;
 iflag = no_interrupt(1);
 c = SS_GETC(ss);
 no_interrupt(iflag);
 return(c);}

void ss_ungetc_fcn(int c,struct sock_stream *ss)
{int iflag;
 if (c == EOF) return;
 iflag = no_interrupt(1);
 --ss->iptr;
 if ((ss->iptr < ss->ibase) || (*ss->iptr != c))
   err("inconsistent s_ungetc",NIL);
 ++ss->icnt;
 no_interrupt(iflag);}
 
LISP s_read_sexp(LISP s)
{struct gen_readio r;
 r.getc_fcn = (int (*)(void *)) ss_getc_fcn;
 r.ungetc_fcn = (void (*)(int,void *)) ss_ungetc_fcn;
 r.cb_argument = get_ss(s,1);
 return(readtl(&r));}

LISP lgethostname(void)
{char buff[256];
 if (gethostname(buff,sizeof(buff)))
   return(err("gethostname",llast_c_errmsg(-1)));
 else
   return(strcons(strlen(buff),buff));}

#if defined(unix)

LISP lgethostid(void)
{return(flocons(gethostid()));}

#endif

#ifdef WIN32
static struct WSAData wsa_data;
LISP lwsa_data(void)
{return(symalist("version",cons(flocons(wsa_data.wHighVersion),
		                        flocons(wsa_data.wVersion)),
				 "description",strcons(-1,wsa_data.szDescription),
				 "system-status",strcons(-1,wsa_data.szSystemStatus),
				 "max-sockets",flocons(wsa_data.iMaxSockets),
				 "max-udp-dg",flocons(wsa_data.iMaxUdpDg),
				 NULL));}
#endif

void init_ss(void)
{long j;
#ifdef WIN32
 int status;
 status = WSAStartup(MAKEWORD(1,1), &wsa_data); 
 if (status != 0)
  err("WSAStartup",flocons(status));
 init_subr_0("wsa-data",lwsa_data);
#endif
 tc_sock_stream = allocate_user_tc();
 set_gc_hooks(tc_sock_stream,
	      NULL,
	      NULL,
	      NULL,
	      ss_gc_free,
	      &j);
 set_print_hooks(tc_sock_stream,ss_prin1);
 init_subr_3("s-open",s_open);
 init_subr_1("s-close",s_close);
 init_subr_1("s-getc",s_getc);
 init_subr_1("s-icnt",s_icnt);
 init_subr_2("s-putc",s_putc);
 init_subr_1("s-force-output",s_force_output);
 init_subr_2("s-puts",s_puts);
 init_subr_1("s-drain",s_drain);
 init_subr_2("s-gets",s_gets);
 init_subr_1("get-protocol-name",lgetproto);
 init_subr_2("get-service-name",lgetservice);
 init_subr_2("s-accept",s_accept);
 init_subr_1("s-read-sexp",s_read_sexp);
 init_subr_1("inet_addr",inet_addr_l);
 init_subr_1("inet_ntoa",inet_ntoa_l);
 init_subr_1("gethostbyaddr",gethostbyaddr_l);
 init_subr_1("gethostbyname",gethostbyname_l);
 init_subr_0("gethostname",lgethostname);
 init_subr_1("getsockname",l_getsockname);
 init_subr_1("getpeername",l_getpeername);
 init_subr_2("s-read",s_read);
 init_subr_2("s-write",s_write);
 init_subr_2("s-shutdown",s_shutdown);
#if defined(unix)
 init_subr_0("gethostid",lgethostid);
#endif
 init_ss_version();}
