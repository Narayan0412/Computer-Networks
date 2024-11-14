#define _DEFAULT_SOURCE
#include "../basic/header.h"

unsigned short 
csum(unsigned short *ptr, int nbytes) {
    unsigned long sum;
    unsigned short oddbyte;
    unsigned short answer;

    sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1) {
        oddbyte = 0;
        *((unsigned char *)&oddbyte) = *(unsigned char *)ptr;
        sum += oddbyte;
    }

    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    answer = (short)~sum;
    return answer;
}

int
main() {

	int rsfd = socket(AF_INET, SOCK_RAW, 254);

	int OPT = 1;
	if (setsockopt(rsfd, IPPROTO_IP, IP_HDRINCL, &OPT,sizeof(OPT)) < 0) {
		perror("setsockopt() error ");
		exit(EXIT_FAILURE);
	}

	struct sockaddr_in ca;
	ca.sin_family = AF_INET;
	ca.sin_port = htons(0);
	inet_pton(AF_INET, "192.168.60.158", &(ca.sin_addr));

	unsigned char* data = (unsigned char*)malloc(sizeof(struct iphdr));

	struct iphdr *iph = (struct iphdr *) data;
	iph->ihl = 5;
	iph->version = 4;
	iph->tos = 0;
	iph->id = htons(10201);
	iph->ttl = 64;
	iph->frag_off = 0;
	iph->protocol = 254;
	iph->saddr = inet_addr("192.168.60.36"); 
	iph->daddr = ca.sin_addr.s_addr; 
	iph->tot_len = 20;
	iph->check =  csum((unsigned short*)data,iph->tot_len);
	
	printf("Raw data : \n");
	print_payload(data,sizeof(struct iphdr));

	if (sendto(rsfd, data,sizeof(struct iphdr),0,(struct sockaddr*)&ca,sizeof(ca)) < 0) {
		perror("sendto error ");
		exit(EXIT_FAILURE);
	}
	
	close(rsfd);

	return 0;
}
