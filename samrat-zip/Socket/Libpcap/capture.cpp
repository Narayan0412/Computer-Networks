#include <bits/stdc++.h>
#include <pcap.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <cstring>
#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <memory.h>
using namespace std;

#define PROMISCUOUS_MODE 1	
#define NON_PROMISCUOUS_MODE 0

void error(string s)
{
	perror(s.c_str());
	exit(1);
}

void printETHHeader(struct ethhdr *eth)
{
	cout << "Ethernet Header ############################# " << endl;

	cout << "Destination MAC" << endl;
	printf("%02X:%02X:%02X:%02X:%02X:%02X\n", eth->h_dest[0], eth->h_dest[1], eth->h_dest[2], eth->h_dest[3], eth->h_dest[4], eth->h_dest[5]);

	cout << "Source MAC" << endl;
	printf("%02X:%02X:%02X:%02X:%02X:%02X\n", eth->h_source[0], eth->h_source[1], eth->h_source[2], eth->h_source[3], eth->h_source[4], eth->h_source[5]);

	cout << "Ethernet Type" << endl;
	cout << eth->h_proto << endl;
}

void printIPHeader(struct iphdr *ipHeader)
{
	//struct ip *ipHeader = (struct ip *)(packet);
	struct sockaddr_in src, dest;
    src.sin_addr.s_addr = ipHeader->saddr;
    dest.sin_addr.s_addr = ipHeader->daddr;

	cout << "IP Header: ################################" << endl;
	cout << "   Version: " << (int)ipHeader->version << endl;
	cout << "   Header Length: " << (int)ipHeader->ihl << " words (" << (int)(ipHeader->ihl * 4) << " bytes)" << endl;
	cout << "   Type of Service: " << (int)ipHeader->tos << endl;
	cout << "   Total Length: " << ntohs(ipHeader->tot_len) << " bytes" << endl;
	cout << "   Identification: " << ntohs(ipHeader->id) << endl;
	//cout << "   Flags: " << (int)(ipHeader->ip_off & 0xE000) << endl;
	//cout << "   Fragment Offset: " << (int)(ipHeader->ip_off & 0x1FFF) << endl;
	cout << "   Time to Live (TTL): " << (int)ipHeader->ttl << endl;
	cout << "   Protocol: " << (int)ipHeader->protocol << endl;
	cout << "   Checksum: 0x" << hex << ntohs(ipHeader->check) << dec << endl;
	
	cout << "   Source IP: " << inet_ntoa(src.sin_addr) << endl;
	cout << "   Destination IP: " << inet_ntoa(dest.sin_addr) << endl;
}

void printTCPHeader(struct tcphdr *tcpHeader)
{
	//struct tcphdr *tcpHeader = (struct tcphdr *)packet;

	cout << "TCP Header : ############################\n";   //ntohs(tcp_header->source)
	cout << "    Source port : " << ntohs(tcpHeader->th_sport) << endl;
	//cout << "    Source port : " << htons(tcpHeader->source) << endl;
	cout << "    Destination port : " << ntohs(tcpHeader->th_dport) << endl;
	cout << "    sequence : " << tcpHeader->th_seq << endl;
	cout << "    acknowledge : " << tcpHeader->th_ack << endl;
	cout << "    data offset : " << (tcpHeader->th_off << 2) << " Bytes" << endl;
	
	cout << "    Flags : " << static_cast<int>(tcpHeader->th_flags) << endl;
	cout << "    ----------Flags----------- "<<endl;
	cout << "    	Acknowledgement Flag : " << tcpHeader->ack << endl;
	cout << "    	Synchronise Flag : " << tcpHeader->syn << endl;
	cout << "    	Finish Flag : " << tcpHeader->fin << endl;

	cout << "    window size : " << tcpHeader->th_win << endl;
	cout << "    chesksum : 0x" << tcpHeader->th_sum << endl;
	cout << "    Urgent pointer : " << tcpHeader->th_urp << endl;
}

void capturedPacketHandler(u_char *userdata, const struct pcap_pkthdr *pkthdr, const u_char *packet)
{
	struct ethhdr *ethernet_header = (struct ethhdr *)packet;
	struct iphdr *ipHeader;
	struct tcphdr *tcp_header;
	//struct udphdr *udp_header;

	unsigned int packet_size = pkthdr->len;

	// Extract the IP header
	ipHeader = (struct iphdr *)(packet + sizeof(struct ethhdr));
	tcp_header = (struct tcphdr *)(packet + ipHeader->ihl*4 + sizeof(struct ethhdr));

	printETHHeader(ethernet_header);
	printIPHeader(ipHeader);
	printTCPHeader(tcp_header);

	// Print the payload data as characters
	cout << "Payload (interpreted as a string): ";

	//put udphdr size=8, ethhdr size =14
	for (unsigned int i = sizeof(struct ethhdr) + (ipHeader->ihl * 4) + (tcp_header->doff*4); i < packet_size; i++)
	{
		cout << packet[i];
	}
	cout << endl << "Packet Captured ############################################### " << endl << endl;
}

int main()
{
	char errorBuf[PCAP_ERRBUF_SIZE];

	string device = "eth0"; 

	// pcap handle for capturing packets
	pcap_t *handle = pcap_open_live(device.c_str(), BUFSIZ, PROMISCUOUS_MODE, 1000, errorBuf);
	/*
		BUFSIZ= max number of bytes to capture per packet.
		MODE= 
		(When you open a live capture session without setting promiscuous mode,
		 the capture session will operate in non-promiscuous mode. In non-promiscuous
		 mode, the network interface will capture only the packets that are destined 
		 for or sent from the machine running the capture, as well as broadcast and 
		 multicast packets. Packets not intended for the machine will not be captured.)
		1000= Timeout in milliseconds for capturing packets.
	*/

	if (handle == NULL)
		error("pcap_open_live");


	//add filter--------------------------------
    bpf_u_int32 netp;
    bpf_u_int32 maskp;
    pcap_lookupnet(device.c_str(), &netp, &maskp, errorBuf);				//network number, netmask

    struct bpf_program fp;													//Berkeley Packet Filter
    char filter_exp[] = "ip dst host 10.42.0.224 || ip dst host 10.42.0.162";
    /*
		(ip src host 192.168.1.1) and (tcp dst port 8080 or udp dst port 7070)
		(ip proto 254)
    */

    //Compiles a filter expression into a BPF (Berkeley Packet Filter) program.
    if (pcap_compile(handle, &fp, filter_exp, 0, netp) == -1)				//0->1 if want to optimize
    {
        fprintf(stderr, "Error compiling filter expression: %s\n", pcap_geterr(handle));
        pcap_close(handle);
        return 2;
    }

    //Sets the compiled filter to be used for packet capture.
    if (pcap_setfilter(handle, &fp) == -1)
    {
       fprintf(stderr, "Error setting filter: %s\n", pcap_geterr(handle));
       pcap_close(handle);
       return 3;
    }
    //--------------------------------------------------------


	// Start packet capture loop and process packets with this function
	if (pcap_loop(handle, 0, capturedPacketHandler, nullptr) < 0)				//0=infi, nullptr-user def parameter
		error("pcap_loop");

	pcap_close(handle);
	return 0;
}
