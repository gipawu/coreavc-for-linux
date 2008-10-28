#define DS_SOCKET 0x01
#define DS_SEMAPHORE 0x02

#define DS_TIMEOUT -1

void *timed_seminit(unsigned int semtype, void *id, int is_host);
void timed_semclean(void *_sem);
void timed_sempost(void *_sem);
int timed_semwait(void *_sem, int secs);
