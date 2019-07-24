#include"traceroute.h"
/*
Krystian Grabowski 288282
*/

//Suma kontrolna z wykładu
u_int16_t compute_icmp_checksum (const void *buff, int length)
{
	u_int32_t sum;
	const u_int16_t* ptr = buff;
	assert (length % 2 == 0);
	for (sum = 0; length > 0; length -= 2)
		sum += *ptr++;
	sum = (sum >> 16) + (sum & 0xffff);
	return (u_int16_t)(~(sum + (sum >> 16)));
}

//Tworzenie echo request.
struct icmphdr create_echo_request(int id, int seq){
    struct icmphdr icmp_header;
    icmp_header.type = ICMP_ECHO;
    icmp_header.code = 0;
    icmp_header.un.echo.id = id;
    icmp_header.un.echo.sequence = seq;
    icmp_header.checksum = 0;
    icmp_header.checksum = compute_icmp_checksum (
        (u_int16_t*)&icmp_header, sizeof(icmp_header));
    return icmp_header;
}

struct sockaddr_in address(char* addr){
    struct sockaddr_in recipient;
    bzero (&recipient, sizeof(recipient));
    recipient.sin_family = AF_INET;
    recipient.sin_addr.s_addr = inet_addr(addr); 
    return recipient;
}

int send_three_with_ttl(int ttl, int id, int seq, int sockfd, char* addr){
    struct sockaddr_in recipient = address(addr);
    if( (inet_pton(AF_INET, "adres_ip", &recipient.sin_addr)) < 0 ){
		fprintf(stderr, "inet_pton error: %s\n", strerror(errno)); 
		return -1;
    }
    int s = seq;
    for (int i=0; i<3; i++){
        struct icmphdr icmp_header = create_echo_request(id, s++ );
        if (setsockopt (sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int)) < 0) {
			fprintf(stderr, "setsockopt error: %s\n", strerror(errno)); 
			return -1;
		}
        ssize_t bytes_sent = sendto (
            sockfd,
            &icmp_header,
            sizeof(icmp_header),
            0,
            (struct sockaddr*)&recipient,
            sizeof(recipient)
        );
        if (bytes_sent < 0) {
			fprintf(stderr, "sendto error: %s\n", strerror(errno)); 
			return -1;
		}
    }
    return 0;
}
