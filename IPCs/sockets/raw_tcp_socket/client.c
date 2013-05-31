/* A simple client program in socket */


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<netinet/in.h>

#define BUFFSIZE 32

#define P 25        /* lets flood the sendmail port */
#define DIE(message) { \ 
        perror(message);\
        exit(-1);\
    }

unsigned short      /* this function generates header checksums */
csum (unsigned short *buf, int nwords)
{
  unsigned long sum;
  for (sum = 0; nwords > 0; nwords--)
    sum += *buf++;
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  return ~sum;
}


int main(int argc, char * argv[]){
    int sock;
    struct sockaddr_in echoserver;
    char buffer[BUFFSIZE];
    unsigned int echolen;
    int received=0;
    int ret;
    
    if( argc < 4){
        printf("USAGE: TCPecho <server ip> <word> <port num> \n");
        exit(0);
    }
    
    /* Create the TCP socket */
    
    sock = socket (PF_INET,SOCK_RAW, IPPROTO_TCP);
    if( sock <0 ){
        DIE("Socket creation error\n");
    }
    
    /* Construct the server sockaddr_in structure */
    
    memset(&echoserver,0,sizeof(echoserver));
    echoserver.sin_family = AF_INET; // Any Internet address 
    echoserver.sin_addr.s_addr = inet_addr(argv[1]); // IP address 
    echoserver.sin_port = htons(atoi(argv[3]));  // Server port (Converting little endian to big endian ( Network byte order))

    /* Establish connection */

    ret = connect(sock, (struct sockaddr *)&echoserver, sizeof(echoserver));
    if(ret < 0) {
        DIE(" Connect failed \n");
    }
    

int 
main (void)
{
  int s = socket (PF_INET, SOCK_RAW, IPPROTO_TCP);  /* open raw socket */
  char datagram[4096];  /* this buffer will contain ip header, tcp header,
               and payload. we'll point an ip header structure
               at its beginning, and a tcp header structure after
               that to write the header values into it */
  struct ip *iph = (struct ip *) datagram;
  struct tcphdr *tcph = (struct tcphdr *) datagram + sizeof (struct ip);
  struct sockaddr_in sin;
            /* the sockaddr_in containing the dest. address is used
               in sendto() to determine the datagrams path */

  sin.sin_family = AF_INET;
  sin.sin_port = htons (P);/* you byte-order >1byte header values to network
                  byte order (not needed on big endian machines) */
  sin.sin_addr.s_addr = inet_addr ("127.0.0.1");

  memset (datagram, 0, 4096);   /* zero out the buffer */

/* we'll now fill in the ip/tcp header values, see above for explanations */
  iph->ip_hl = 5;
  iph->ip_v = 4;
  iph->ip_tos = 0;
  iph->ip_len = sizeof (struct ip) + sizeof (struct tcphdr);    /* no payload */
  iph->ip_id = htonl (54321);   /* the value doesn't matter here */
  iph->ip_off = 0;
  iph->ip_ttl = 255;
  iph->ip_p = 6;
  iph->ip_sum = 0;      /* set it to 0 before computing the actual checksum later */
  iph->ip_src.s_addr = inet_addr ("1.2.3.4");/* SYN's can be blindly spoofed */
  iph->ip_dst.s_addr = sin.sin_addr.s_addr;
  tcph->th_sport = htons (1234);    /* arbitrary port */
  tcph->th_dport = htons (P);
  tcph->th_seq = random ();/* in a SYN packet, the sequence is a random */
  tcph->th_ack = 0;/* number, and the ack sequence is 0 in the 1st packet */
  tcph->th_x2 = 0;
  tcph->th_off = 0;     /* first and only tcp segment */
  tcph->th_flags = TH_SYN;  /* initial connection request */
  tcph->th_win = htonl (65535); /* maximum allowed window size */
  tcph->th_sum = 0;/* if you set a checksum to zero, your kernel's IP stack
              should fill in the correct checksum during transmission */
  tcph->th_urp = 0;

  iph->ip_sum = csum ((unsigned short *) datagram, iph->ip_len >> 1);

/* finally, it is very advisable to do a IP_HDRINCL call, to make sure
   that the kernel knows the header is included in the data, and doesn't
   insert its own header into the packet before our data */

    {             /* lets do it the ugly way.. */
        int one = 1;
        const int *val = &one;
        if (setsockopt (s, IPPROTO_IP, IP_HDRINCL, val, sizeof (one)) < 0)
            printf ("Warning: Cannot set HDRINCL!\n");
    }

    while (1)
    {
        if (sendto (s, datagram, iph->ip_len, 0, (struct sockaddr *) &sin,sizeof (sin)) < 0)
            printf ("error\n");
        else
            printf (".");
    }
    return 0;
}

    /* Send and Receive Data */

    /* send the word to the server */
    echolen = strlen(argv[2]);
    ret = send(sock, argv[2], echolen,0);
    if( ret != echolen) {
        DIE("Send error\n");
    }
    
    /*Receive the word back from the server */

    printf("Recieved from server: ");
    while(received < echolen){
        int bytes = 0;
        if((bytes = recv(sock,buffer,BUFFSIZE -1,0)) < 0) {
            DIE("Receive failed\n");
        }
        received = bytes + received;
        buffer[bytes] = '\0'; // Assuming NULL terminated bytes
        printf("%s", buffer);
    }
    
    printf("\n");
    close(sock);
    return 0;
}