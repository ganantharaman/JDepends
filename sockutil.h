#ifndef __SOCKUTIL__HEADER__
#define __SOCKUTIL__HEADER__
#include <windows.h>

#define ERROR_CHECK( result, code )\
	if( link_error( result )) return code;

struct link;
typedef int ( *pfn_acceptcall_t )(int fd);

/* link error codes */
typedef enum link_error {
	LERR_SUCCESS,
	LERR_BADSOCK,
  LERR_BADCONN,
	LERR_BADWRITE,
	LERR_BADREAD,
	LERR_BADBIND,
	LERR_BADLISTEN,
  LERR_TRANSIENT,
  LERR_BADPARAM,
  LERR_TIMEOUT
} link_error_t;


#ifndef WIN32
extern int errno;
#define LERR_EMFILE EMFILE
#define LERR_EINTR EINTR
#define LERR_EAGAIN EAGAIN
#define LERR_EWOULDBLOCK EWOULDBLOCK
#define LERROR_FUNC errno
#endif 

/* For now support only 2 protocols */
typedef enum proto {
	TCP_IP,
	UDP_IP,
} proto_t;

/* Link data structure */
typedef struct link {
 int     socket;            /* listen or plain socket */
 int     conn_socket;       /* connected socket returned from 'accept' */
 char  local_ip[64];          /* local host IP address */
 char  dest_ip[64];           /* foriegn host IP address */
 unsigned short  local_port;        /* listening port for server, 'ephemeral' port for client */ 
 unsigned short  dest_port;         /* listening port of server(used only for clients) */
 int     backlog;           /* 'listen' calls backlog(used only for servers,usually 5) */
 struct sockaddr_in servaddr; /* servaddr structure */
 struct sockaddr_in clntaddr; /* clntaddr structure */
 proto_t protocol;            /* supported protocols */
 unsigned long send_timeout;      /* send timeout */
 unsigned long recv_timeout;      /* receive timeout */
 int     sock_err;            /* OS socket error code */ 
 pfn_acceptcall_t accept_call;  /* called after 'accept' returns for servers */
 unsigned long data_written;
 unsigned long data_read;
 int state;
 void * ref;
} link_t;


#if defined(FD_SETSIZE)
#undef FD_SETSIZE
#endif
#define FD_SETSIZE 4096

 
// Common definitions for all
#define link_listen(fd, len)               listen(fd, len)
#define link_bind(fd, buf, len)            bind(fd, buf, len)
#define link_netof(addr, mask) (((unsigned int) addr) & ((unsigned int) mask))

// WIN32 definitions
#if defined(WIN32)

#define link_read(fd, buf, len)            recv(fd, buf, len, 0)
#define link_write(fd, buf, len)           send(fd, buf, len, 0)
#define link_close(fd)                     closesocket(fd)
#define link_select(nfd, rmask, wmask, emask, timeout) \
                   select(nfd, rmask, wmask, emask, \
                           (struct timeval *)timeout)

#define link_socket(domain, type, proto)   socket(domain, type, proto)
#define link_connect(fd, name, len)        connect(fd, name, len)
#define link_accept(fd, addr, len)         accept(fd, addr, len)
#define link_iocntl(arg1, arg2, arg3)      ioctlsocket(arg1, arg2, arg3)
#define link_shutdown(x)                   shutdown(x, 2);  closesocket(x)
#define link_sendto(s, buf, len, flags, to, tolen) \
                   sendto(s, buf, len, flags, to, tolen)

#define link_send(s, buf, len, flags) \
                   send(s, buf, len, flags)

#define link_recvfrom(s, buf, len, flags, from, from_len) \
                   recvfrom(s, buf, len, flags, from, from_len)

#define link_recv(s, buf, len, flags)  recv(s, buf, len, flags)
#define link_getsockopt(s, level, optname, optval, optlen) \
                        getsockopt(s, level, optname, optval, (int *)optlen)
#define link_setsockopt(fd, a, b, c, d) \
                        setsockopt(fd, a, b, c, d)

#define link_getsockname(s, addr, addrlen)  getsockname(s, addr, addrlen)
#define link_error_check()     (int)WSAGetLastError()

// Winsock Error codes mapping
#define LERR_EINTR                WSAEINTR
#define LERR_EWOULDBLOCK          WSAEWOULDBLOCK
#define LERR_EAGAIN               WSAEWOULDBLOCK
#define LERR_EINPROGRESS          WSAEINPROGRESS
#define LERR_EADDRINUSE           WSAEADDRINUSE
#define LERR_ECONNREFUSED         WSAECONNREFUSED
#define LERR_ENETDOWN             WSAENETDOWN
#define LERR_ENETRESET            WSAENETRESET
#define LERR_ENOTCONN             WSAENOTCONN
#define LERR_ECONNABORTED         WSAECONNABORTED
#define LERR_ECONNRESET           WSAECONNRESET
#define LERR_ENOTSOCK             WSAENOTSOCK
#define LERR_EINVAL               EINVAL

#define BAD_SOCKET                -1

#else    // !WIN32( All Unix )

#if defined(HP_CMA_THREADS)

#include <dce/cma.h>
#include <dce/cma_ux.h>
#define link_read(fd, buf, len)      cma_read(fd, buf, len)
#define link_write(fd, buf, len)     cma_write(fd, buf, len)
#define link_close(fd)          cma_close(fd)
#define link_pipe(fd)           cma_pipe(fd)
#define link_select(nfd, rmask, wmask, emask, timeout) \
                cma_select(nfd, (int *)rmask, (int *)wmask, (int *)emask, \
                           (struct timeval *)timeout)
#define link_socket(domain, type, proto)  cma_socket(domain, type, proto)
#define link_connect(fd, name, len)       cma_connect(fd, name, len)
#define link_accept(fd, addr, len)        cma_accept(fd, addr, len)
#define link_iocntl(arg1, arg2, arg3)     cma_ioctl(arg1, arg2, arg3)
#define link_fcntl(arg1, arg2, arg3)      cma_fcntl(arg1, arg2, arg3)
#define link_shutdown(x)                   shutdown(x, 2); cma_close(x)
#define link_sendto(s, buf, len, flags, to, tolen) \
                  cma_sendto(s, buf, len, flags, to, tolen)
#define link_send(s, buf, len, flags) cma_send(s, buf, len, flags)
#define link_recvfrom(s, buf, len, flags, from, from_len) \
          cma_recvfrom(s, buf, len, flags, from, from_len)
#define link_recv(s, seq, flags) cma_recv(s, buf, len, flags)
#define link_poll(fds, nfds, timeout) cma_poll(fds, nfds, timeout)

#else    // UNIX other than HP_CMA_THREADS

#define link_read(fd, buf, len)     read(fd, buf, len)
#define link_write(fd, buf, len)    write(fd, buf, len)
#define link_close(fd)              close(fd)
#define link_pipe(fd)         	  pipe(fd)
#if defined(HPUX_aCC)
#define link_select(nfd, rmask, wmask, emask, timeout) \
         select(nfd, (fd_set *)rmask, (fd_set *)wmask, (fd_set *)emask, \
                  (struct timeval *)timeout)
#elif defined(__hpux)
#define link_select(nfd, rmask, wmask, emask, timeout) \
         select(nfd, (int *)rmask, (int *)wmask, (int *)emask, \
                 (struct timeval *)timeout)
#else
#define link_select(nfd, rmask, wmask, emask, timeout) \
         select(nfd, rmask, wmask, emask, (struct timeval *)timeout)
#endif
#define link_socket(domain, type, proto)     socket(domain, type, proto)
#define link_connect(fd, name, len)          connect(fd, name, len)
#if defined(AIXV4_2)
#define link_accept(fd, addr, len)           naccept(fd, addr, (size_t *)len)
#elif defined(AIXV4_1)
#define link_accept(fd, addr, len)           naccept(fd, addr, (int *)len)
#else
#define link_accept(fd, addr, len)           accept(fd, addr, len)
#endif
#define link_iocntl(arg1, arg2, arg3)        ioctl(arg1, arg2, arg3)
#define link_fcntl(arg1, arg2, arg3)         fcntl(arg1, arg2, arg3)
#define link_shutdown(x)   shutdown(x, 2);  close(x)
#define link_sendto(s, buf, len, flags, to, tolen) \
                   sendto(s, buf, len, flags, to, tolen)
#define link_send(s, buf, len, flags)  send(s, buf, len, flags)
#define link_recvfrom(s, buf, len, flags, from, from_len) \
           recvfrom(s, buf, len, flags, from, from_len)
#define link_recv(s, buf, len, flags)  recv(s, buf, len, flags)
#define link_poll(fds, nfds, timeout)  poll(fds, nfds, timeout)

#endif    // HP_CMA_THREADS

// All UNIX 
#if defined(AIXV4_2)
#define link_getsockopt(s, level, optname, optval, optlen) \
            getsockopt(s, level, optname, optval, (size_t *)optlen)
#define link_getsockname(s, addr, addrlen) \
            getsockname(s, addr, (size_t *)addrlen)
#else
#define link_getsockopt(s, level, optname, optval, optlen) \
            getsockopt(s, level, optname, optval, (int *)optlen)
#define link_getsockname(s, addr, addrlen) \
            getsockname(s, addr, (int *)addrlen)
#endif

#define link_error_check()        (int)errno

#define LERR_EINTR                EINTR
#define LERR_EWOULDBLOCK          EWOULDBLOCK
#define LERR_EAGAIN               EAGAIN
#define LERR_EINPROGRESS          EINPROGRESS
#define LERR_EADDRINUSE           EADDRINUSE
#define LERR_ECONNREFUSED         ECONNREFUSED
#define LERR_ENETDOWN             ENETDOWN
#define LERR_ENETRESET            ENETRESET
#define LERR_ENOTCONN             ENOTCONN
#define LERR_ECONNABORTED         ECONNABORTED
#define LERR_ECONNRESET           ECONNRESET
#define LERR_EPIPE                EPIPE
#define LERR_ENOTSOCK             EBADF
#define LERR_EINVAL               EINVAL

#endif  // !WIN32


#endif /* __SOCKUTIL__HEADER__ */


