#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

#ifndef __MINGW32__
  #include <netdb.h>
  #include <netinet/in.h>
  #include <sys/socket.h>
  #include <arpa/inet.h>
  #include <sys/wait.h>

  #define DS_EINPROGRESS EINPROGRESS
  #define DS_ETIMEDOUT   ETIMEDOUT
  #define DS_EWOULDBLOCK EWOULDBLOCK
#else
  #define _WIN32_WINNT 0x0501
  #include <windows.h>
  #include <winsock2.h>
  #include <ws2tcpip.h>

  #define DS_EINPROGRESS WSAEINPROGRESS
  #define DS_ETIMEDOUT   WSAETIMEDOUT
  #define DS_EWOULDBLOCK WSAEWOULDBLOCK
#endif

#include "timeout_sem.h"
#ifdef __MINGW32__
  #undef DS_SEMAPHORE
#endif

#ifdef DS_SEMAPHORE
#include <time.h>
#include <semaphore.h>
#endif

struct sem {
  int type;
  int initialized;
  int sockfd;
  int listenfd;
  void *id;
  char mutex_rx[1];
  char mutex_tx[1];
#ifdef DS_SEMAPHORE  
  sem_t *sem_rd;
  sem_t *sem_wr;
#endif /*DS_SEMAPHORE*/
};

#ifdef DS_SEMAPHORE
#ifdef __APPLE__
  void ALRMhandler(int sig) {
  }
  int sem_twait(sem_t *sem, int t) {
    int ret;
    alarm(t);
    ret = sem_wait(sem);
    printf("twait complete\n");
    return ret;
  }
  void init_twait() {
    sigset_t none;
    struct sigaction sa;
    sigemptyset(&none);
    sigprocmask(SIG_SETMASK, &none, 0);

    sa.sa_handler = ALRMhandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    sigaction(SIGALRM, &sa, 0);
  }
#else
  int sem_twait(sem_t *sem, int t) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += t;
    return(sem_timedwait(sem, &ts));
  }
  void init_twait() {}
#endif
#endif /*DS_SEMAPHORE */

int setblocking(int sock, int block)
{
	unsigned long opts;
#ifndef __MINGW32__
	opts = fcntl(sock,F_GETFL);
	if (opts < 0) {
		perror("fcntl(F_GETFL)");
		exit(EXIT_FAILURE);
	}
	opts = block ? (opts & ~O_NONBLOCK)
                     : (opts | O_NONBLOCK);
	if (fcntl(sock,F_SETFL,opts) < 0) {
		perror("fcntl(F_SETFL)");
		exit(EXIT_FAILURE);
	}
#else
	opts = !(block);
	if ( ioctlsocket( sock, FIONBIO, &opts ) == SOCKET_ERROR )
	{
		perror("ioctlsocket");
		exit(EXIT_FAILURE);
	}
#endif
	return 0;
}

int timed_connect(int sockfd, const struct sockaddr *serv_addr,
                   socklen_t addrlen, int secs) {
  //Socket should already be non-blocking
  int res; 
  fd_set myset; 
  struct timeval tv; 
  int valopt; 
  socklen_t lon; 

  // Trying to connect with timeout 
  res = connect(sockfd, serv_addr, addrlen); 
  if (res < 0 ) { 
     if (errno == DS_EINPROGRESS || errno == DS_EWOULDBLOCK || errno == 0) { 
        fprintf(stderr, "EINPROGRESS in connect() - selecting\n"); 
        do { 
           tv.tv_sec = secs; 
           tv.tv_usec = 0; 
           FD_ZERO(&myset); 
           FD_SET(sockfd, &myset); 
           res = select(sockfd+1, NULL, &myset, &myset, &tv); 
           if (res < 0 && errno != EINTR) { 
              fprintf(stderr, "Error connecting (select) %d - %s\n", errno, strerror(errno)); 
              return -1; 
           } 
           else if (res > 0) { 
              // Socket selected for write 
              lon = sizeof(int); 
              if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon) < 0) { 
                 fprintf(stderr, "Error in getsockopt() %d - %s\n", errno, strerror(errno)); 
                 return -1; 
              } 
              // Check the value returned... 
              if (valopt) { 
                 fprintf(stderr, "Error in delayed connection() %d - %s\n", valopt, strerror(valopt) 
); 
                 return -1; 
              } 
              break; 
           } 
           else { 
              fprintf(stderr, "Timeout in select() - Cancelling!\n"); 
              return -1; 
           } 
        } while (1); 
     } 
     else { 
        fprintf(stderr, "Error connecting (connect) %d - %s\n", errno, strerror(errno)); 
        return -1; 
     } 
  } 
  // I hope that is all 
  return 0;
}
int timed_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen, int secs) {
  //Socket should already be non-blocking
  int res; 
  fd_set myset; 
  struct timeval tv; 

  tv.tv_sec = secs; 
  tv.tv_usec = 0; 
  FD_ZERO(&myset); 
  FD_SET(sockfd, &myset); 
  res = select(sockfd+1, &myset, NULL, NULL, &tv); 
  if (res < 0 && errno != EINTR) { 
    fprintf(stderr, "Error accepting %d - %s\n", errno, strerror(errno)); 
    return -1; 
  } 
  else if (res > 0) { 
    // Socket selected for read
    return accept(sockfd, NULL, NULL);
  }
  errno = DS_ETIMEDOUT;
  return -1;
}
int timed_recv(int sockfd, void *buf, size_t len, int flags, int secs) {
  //Socket should already be non-blocking
  int res; 
  fd_set myset; 
  struct timeval tv; 

  tv.tv_sec = secs; 
  tv.tv_usec = 0; 
  FD_ZERO(&myset); 
  FD_SET(sockfd, &myset); 
  res = select(sockfd+1, &myset, NULL, NULL, &tv); 
  if (res < 0 && errno != EINTR) { 
    fprintf(stderr, "Error accepting %d - %s\n", errno, strerror(errno)); 
    return -1; 
  } 
  else if (res > 0) { 
    // Socket selected for read
    return recv(sockfd, buf, len, flags);
  }
  errno = DS_ETIMEDOUT;
  return -1;
}

int timed_sockinit(int *port, int is_server)
{
    int sockfd;  // listen on sock_fd
    struct sockaddr_in my_addr;
    socklen_t peer_addr_size = sizeof(struct sockaddr_in);
    int yes=1;

#ifdef __MINGW32__
    WSADATA wsaData;
    if(WSAStartup(MAKEWORD(2, 2), &wsaData) !=0) {
      printf("WSAStartup failed\n");
      exit(1);
    }
#endif

    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sin_family = AF_INET;
    my_addr.sin_addr.s_addr=INADDR_ANY;
    my_addr.sin_port = *port;
    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) {
            perror("server: socket");
            exit(1);
    }

    setblocking(sockfd, 0);
    if (is_server) {
      if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&yes,
              sizeof(int)) == -1) {
        perror("setsockopt");
        exit(1);
      }

      if (bind(sockfd, (struct sockaddr *) &my_addr,  sizeof(struct sockaddr_in)) == -1) {
              close(sockfd);
              perror("server: bind");
              exit(1);
      }
      if (listen(sockfd, 1) == -1) {
        perror("listen");
        exit(1);
      }
      if (getsockname(sockfd, (struct sockaddr *)&my_addr, &peer_addr_size) == -1) {
        perror("getsockname");
        exit(1);
      }
      if(my_addr.sin_port == 0) {
        printf("Failed to get port\n");
        exit(1);
      }
      *port = my_addr.sin_port;
    } else {
      if (timed_connect(sockfd, (struct sockaddr *) &my_addr,  sizeof(struct sockaddr_in), 10) == -1)
      {
        close(sockfd);
        perror("client: connect");
        exit(1);
      }
    }

    return sockfd;
}

int timed_semwait(void *_sem, int secs) {
  struct sem *sem = (struct sem *)_sem;
  int ok = -1;
  if(sem->type == DS_SOCKET) {
    if(! sem->initialized) {
      ok = timed_accept(sem->sockfd, NULL, NULL, secs);
      if(ok != -1) {
        sem->listenfd = sem->sockfd;
        sem->sockfd = ok;
        ok = 1;
        sem->initialized = 1;
      }
    } else {
      ok = (timed_recv(sem->sockfd, sem->mutex_rx, 1, 0, secs) == 1);
    }
  }
#ifdef DS_SEMAPHORE
  else if(sem->type == DS_SEMAPHORE) {
    ok = (sem_twait(sem->sem_rd, secs) == 0);
    if(! sem->initialized) {
      timed_semclean(sem);
      sem->initialized = 1;
    }
  }
#endif
  if(!ok && errno == DS_ETIMEDOUT) {
    ok = DS_TIMEOUT;
  }
  return ok;
}

void timed_sempost(void *_sem) {
  struct sem *sem = (struct sem *)_sem;
  if(sem->type == DS_SOCKET) {
    send(sem->sockfd, sem->mutex_tx, 1, 0);
  }
#ifdef DS_SEMAPHORE
  else if(sem->type == DS_SEMAPHORE) {
    sem_post(sem->sem_wr);
  }
#endif
}

void timed_semclean(void * _sem) {
#ifdef DS_SEMAPHORE
  struct sem *sem = (struct sem *) _sem;
  if(sem->type == DS_SEMAPHORE) {
    char sem1[80], sem2[80];
    snprintf(sem1, 80, "/dshow_sem1.%s", (char *)sem->id);
    snprintf(sem2, 80, "/dshow_sem2.%s", (char *)sem->id);
    sem_unlink(sem1);
    sem_unlink(sem2);
  }
#endif
}

void *timed_seminit(unsigned int semtype, void *id, int is_host) {
  struct sem *sem;
  sem = (struct sem *)malloc(sizeof(struct sem));
  memset(sem, 0, sizeof(struct sem));
  sem->type = semtype;
  sem->id = id;
  sem->initialized = !(is_host);
  if(semtype == DS_SOCKET) {
    sem->listenfd = -1;
    sem->sockfd = timed_sockinit((int *)id, is_host);
    if(sem->sockfd == -1) {
      perror("sock_init");
      exit(1);
    }
  }
#ifdef DS_SEMAPHORE  
  else if(semtype == DS_SEMAPHORE) {
    char semrd[80], semwr[80];
    init_twait();
    snprintf(semrd, 80, "/dshow_sem%d.%s", is_host ? 2 : 1, (char *)id);
    snprintf(semwr, 80, "/dshow_sem%d.%s", is_host ? 1 : 2, (char *)id);
    if(is_host) {
      sem->sem_rd = sem_open(semrd, O_CREAT, 0644, 0);
      sem->sem_wr = sem_open(semwr, O_CREAT, 0644, 0);
    } else {
      sem->sem_rd = sem_open(semrd, 0);
      sem->sem_wr = sem_open(semwr, 0);
      sem_unlink(semwr);
      sem_unlink(semrd);
    }
    if(sem->sem_rd == SEM_FAILED) {
      timed_semclean(sem);
      perror("sem_open(1)");
      exit(1);
    }
    if(sem->sem_wr == SEM_FAILED) {
      timed_semclean(sem);
      perror("sem_open(2)");
      exit(1);
    }
    //tell calling procedure that we are awake;
    if(! is_host) {
      sem_post(sem->sem_wr);
    }
  }
#endif /*DS_SEMAPHORE*/
  else {
    fprintf(stderr, "Unknown type specified: %d\n", semtype);
    exit(1);
  }
  return sem;
}

void timed_semdelete(void *_sem) {
  struct sem *sem = (struct sem *) _sem;
  if(sem->type == DS_SOCKET) {
    close(sem->sockfd);
    if(sem->listenfd != -1)
      close(sem->listenfd);
#ifdef DS_SEMAPHORE
  } else if(sem->type == DS_SEMAPHORE) {
    if(! sem->initialized)
      timed_semclean(sem);
    sem_close(sem->sem_wr);
    sem_close(sem->sem_rd);
#endif
  }
  free(sem);
}

