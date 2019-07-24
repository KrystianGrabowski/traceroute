#include <stdio.h>
#include <errno.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>

#include <string.h>
#include <sys/socket.h>
#include <netinet/ip_icmp.h>
#include <arpa/inet.h>
#include <sys/select.h>
//#define ICMP_ECHO 8
#define RECV_TIMEOUT 1

struct ip_icmp{
    struct icmphdr icmp;
    double rtt;
	char addr[20];
};

int get_process_id();

//wysyłanie
struct icmphdr create_echo_request(int id, int seq);
int send_three_with_ttl(int ttl, int id, int seq, int sockfd, char* addr);
struct sockaddr_in address(char* addr);

//odbieranie
int listen_echo(int ttl, int id, int sockfd,  struct ip_icmp *array, int *size, struct timeval *tv);
int receive_all(int ttl, int id, int sockfd, char* addr);
