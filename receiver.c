#include"traceroute.h"
/*
Krystian Grabowski 288282
*/

int listen_echo(int ttl, int id, int sockfd,  struct ip_icmp *array, int *size, struct timeval *tv){
    fd_set descriptors;
    FD_ZERO (&descriptors);
    FD_SET (sockfd, &descriptors);
    int ready = select (sockfd+1, &descriptors, NULL, NULL, tv);

    if (ready > 0){
        struct sockaddr_in sender;
        socklen_t sender_len = sizeof(sender);
        u_int8_t buffer[IP_MAXPACKET];
        ssize_t packet_len;

        packet_len = recvfrom (sockfd, buffer, IP_MAXPACKET, 0, (struct sockaddr*)&sender, &sender_len);
        if (packet_len < 0) {
			fprintf(stderr, "recvfrom error: %s\n", strerror(errno)); 
			return -2;
		}
        struct ip_icmp ip_icmp0;
        struct iphdr* 		ip_header = ( struct iphdr* ) buffer;
	    u_int8_t*			icmp_packet = buffer + 4 * ip_header->ihl;
	    struct icmphdr*		icmp_header = ( struct icmphdr* ) icmp_packet;

        if(icmp_header->type == ICMP_TIME_EXCEEDED ){
            //dodajemy 8 bajtów 
            icmp_packet += 8;
            //wyciągamu ihl IP headera i mnożymy razy 32bity(4 bajty) aby dostać początek oryginalnych danych; 
            icmp_packet += 4 * ((struct iphdr*) icmp_packet)->ihl;
            struct icmphdr* original_data = (struct icmphdr*) icmp_packet;
            if (original_data->un.echo.id != id || original_data->un.echo.sequence/3 != ttl){
               return -1;
            }
        }
        else if (icmp_header->type == ICMP_ECHOREPLY){
            if (icmp_header->un.echo.sequence/3 != ttl || icmp_header->un.echo.id != id){
                return -1;
            }
        }
        else{
            return -1;
        }

        struct icmphdr pp = *icmp_header;
        char sender_ip_str[20];
        if (!inet_ntop(AF_INET, &(sender.sin_addr), sender_ip_str, sizeof(sender_ip_str))){
			fprintf(stderr, "inet_ntop error: %s\n", strerror(errno)); 
			return -2;
        }
		memcpy(&ip_icmp0.addr , sender_ip_str, sizeof(sender_ip_str));
        memcpy(&ip_icmp0.icmp, &pp, sizeof(pp));
        ip_icmp0.rtt = (1000000 - tv->tv_usec)/1000.0;
        array[(*size)++] = ip_icmp0;
        return 1;
    }   
    if (ready == 0){
        return 0;
    }
    else{
		fprintf(stderr, "select error: %s\n", strerror(errno)); 
		return -2;
    }
}

int cmpf(char* a, char* b){
    if(strcmp(a, b)==0){
       return 1; 
    }
    return 0;
}


int receive_all(int ttl, int id, int sockfd, char* addr){
    struct timeval tv; tv.tv_sec = RECV_TIMEOUT; tv.tv_usec = 0;
    int cmp = 0;
    int size = 0;
    struct ip_icmp array[3];
    while(size < 3){
        int res = listen_echo(ttl, id, sockfd, array, &size, &tv);
        if(res == 0){
            break;
        }
        if (res < -1){
            return -1;
        }
    }
    if (size == 0){
        printf("%d. *\n", ttl);
    }
    else if (size < 3){
        printf("%d. %s ", ttl, array[0].addr);
        cmp += cmpf(array[0].addr, addr);
        if (size>1){
            if(strcmp(array[1].addr, array[0].addr) != 0){
                printf("%s \n", array[1].addr);
                cmp += cmpf(array[1].addr, addr);
            }
        }
        printf("???\n");
        
    }
    else if (size == 3){
        printf("%d. %s ", ttl, array[0].addr);
        cmp += cmpf(array[0].addr, addr);
        if (strcmp(array[1].addr, array[0].addr) != 0){
            printf("%s ", array[1].addr);
            cmp += cmpf(array[1].addr, addr);
        }
        if (strcmp(array[2].addr, array[1].addr) != 0 && strcmp(array[2].addr, array[1].addr) != 0){
            printf("%s ", array[2].addr);
            cmp += cmpf(array[2].addr, addr);
        }
        double sum = 0;
        for (int i=0; i<size; i++){
            sum += array[i].rtt;
        }
        printf("%dms\n", (int)(sum/3));
    }
    if(cmp>0){
        return 0;
    }
    return 1;   
}
