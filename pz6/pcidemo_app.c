#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>

struct eth_frame {
    struct ethhdr hdr;
    unsigned char payload[ETH_DATA_LEN];
};

int main() {
    const char *iface = "vnet0";   // интерфейс создается драйвером
    const unsigned char target_mac[ETH_ALEN] = {0,0,0,0,0,0};
    const char *msg = "hello world";
    int sockfd;

    sockfd = socket(AF_PACKET, SOCK_RAW, htons(0x1234));
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, iface, IFNAMSIZ);

    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) == -1) {
        perror("SIOCGIFINDEX");
        return 1;
    }
    int index = ifr.ifr_ifindex;

    unsigned char src_mac[ETH_ALEN];
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == -1) {
        perror("SIOCGIFHWADDR");
        return 1;
    }
    memcpy(src_mac, ifr.ifr_hwaddr.sa_data, ETH_ALEN);

    struct eth_frame frame;
    memcpy(frame.hdr.h_dest, target_mac, ETH_ALEN);
    memcpy(frame.hdr.h_source, src_mac, ETH_ALEN);
    frame.hdr.h_proto = htons(0x1234);

    size_t msglen = strlen(msg);
    memcpy(frame.payload, msg, msglen);

    struct sockaddr_ll addr = {0};
    addr.sll_family = AF_PACKET;
    addr.sll_ifindex = index;
    addr.sll_halen = ETH_ALEN;
    memcpy(addr.sll_addr, target_mac, ETH_ALEN);

    if (sendto(sockfd, &frame, msglen + ETH_HLEN, 0,
               (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("sendto");
    } else {
        printf("Frame sent successfully\n");
    }

    close(sockfd);
    return 0;
}
