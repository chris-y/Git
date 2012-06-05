#ifndef __COMPAT_AROS_H
#define __COMPAT_AROS_H

#include <libraries/dos.h>
#include <proto/exec.h>
#include <proto/dos.h>

/* FIXME: Verify if unistd.h may work for AROS too, not just AOS4 */
#ifdef __AROS__
#include <defines/bsdsocket.h>
#undef connect
#endif
#include <exec/types.h>
#include <unistd.h>

#define is_dir_sep(c) (((c) == '/') || (c) == ':')

/* Needed for AROS bsdsockets library */
struct Library * SocketBase;

extern int h_errno;


#ifndef POLLIN
struct pollfd {
	int fd;           /* file descriptor */
	short events;     /* requested events */
	short revents;    /* returned events */
};
#define POLLIN 1
#define POLLHUP 2
#endif

#define ITIMER_REAL 0

/*
 * trivial stubs
 */

static inline int fork(void)
{ errno = ENOSYS; return -1; }
static inline unsigned int alarm(unsigned int seconds)
{ return 0; }

/*
 * implementations of missing functions
 */

/* FIXME: Temporary */
int setitimer(int which, const struct itimerval *new_value,
	      struct itimerval *old_value);
char * getpass(const char * prompt);
const char * hstrerror(int err);
int getpagesize();
int poll(struct pollfd *ufds, unsigned int nfds, int timeout);

#ifndef __amigaos4__
static inline int select(int nfds, fd_set *readfds, fd_set *writefds,
	   fd_set *exceptfds, struct timeval *timeout)
{
#ifdef __AROS__
  return WaitSelect(nfds,readfds,writefds,exceptfds, timeout,0);
#else
  return waitselect(nfds,readfds,writefds,exceptfds, timeout,0);
#endif
}
#endif

#define is_absolute_path(path) (strchr(path,':') != 0)

static inline int chk_abort()
{
  /* Check & clear CTRL_C signal */
  return CheckSignal(SIGBREAKF_CTRL_C);
}

#define CHKABORT 1


/**** AROS specific pieces ****/

#ifdef __AROS__
static inline char *inet_ntoa(struct in_addr in)
{
  return Inet_NtoA(*(unsigned long *)&in);
}

/* Workaround for definition of connect as macro in AROS
   headers, which collide with it being used as a structure
   member in git */
static inline int connect(int sockfd, const struct sockaddr *addr,
	    socklen_t addrlen)
{
  return __connect_WB(SocketBase,sockfd,(struct sockaddr *)addr,addrlen);
}

/* Work around a problem with the AROS gcc port */
void __stack_chk_fail();

#endif

#ifdef __amigaos4__
int pipe(int fd[2]);
int execvp(const char *file, char *const argv[]);
#endif

#endif
