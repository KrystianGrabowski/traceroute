#include"traceroute.h"
/*
Krystian Grabowski 288282
*/


int get_process_id(){
    return getpid();
}

int main(int argc, char *argv[]){
    if (argc != 2){
        fprintf(stderr, "wrong number of arguments \n"); 
		return EXIT_FAILURE;
    }
    unsigned long test_addr = 0;
    if (0 == inet_pton(AF_INET, argv[1], &test_addr)) {
        fprintf(stderr, "invalid ip address\n"); 
        return EXIT_FAILURE;
    }

    int process_id = get_process_id();
    int sockfd = socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);
	if (sockfd < 0) {
		fprintf(stderr, "socket error: %s\n", strerror(errno)); 
		return EXIT_FAILURE;
	}
    for (int i=1; i<31; i++){
        int sender_status = send_three_with_ttl(i, process_id, (i*3), sockfd, argv[1]);
        if (sender_status == -1){
            return EXIT_FAILURE;
        }
        int receiver_status = receive_all(i, process_id, sockfd, argv[1]);
        if(receiver_status == 0){
            break;
        }
        if(receiver_status == -1){
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
    
}