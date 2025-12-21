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

// Структура Ethernet-кадра
struct eth_frame {
    struct ethhdr hdr;
    // максимальный размер полезной нагрузки Ethernet-кадра (payload)
    unsigned char payload[ETH_DATA_LEN];
};  

int main() {
    // Имя интерфейса, через который отправляем кадр
    const char *iface = "eth0";
    // MAC назначения eth_alen длина Ethernet-адреса (MAC-адреса) в байтах
    const unsigned char target_mac[ETH_ALEN] = {0,0,0,0,0,0};
    const char *msg = "hello world";
    // Дескриптор сокета это обычное целое число (int), которое указывает на сокет внутри ядра Linux.
    int sockfd;

    // Создание RAW socket
    // AF_PACKET	Работаем напрямую с Ethernet
    // SOCK_RAW	Сырые кадры
    // htons(0x1234)	EtherType
    sockfd = socket(AF_PACKET, SOCK_RAW, htons(0x1234));
    //если не создался
    if (sockfd < 0) {
        perror("socket");
        return 1;
    }
    // Это универсальная структура ioctl для передачи данных между user-space -> ядром
    struct ifreq ifr;
    // Команда заполняет область памяти одинаковым байтом То есть: обнуляем всю структуру / Предотвращаем мусор
    memset(&ifr, 0, sizeof(ifr));

    // Говорим ядру: «Я хочу работать с интерфейсом eth0»
    strncpy(ifr.ifr_name, iface, IFNAMSIZ);

    // Получить индекс интерфейса
    if (ioctl(sockfd, SIOCGIFINDEX, &ifr) == -1) {
        perror("SIOCGIFINDEX");
        return 1;
    }
    int index = ifr.ifr_ifindex;
    // Буфер под MAC источника
    unsigned char src_mac[ETH_ALEN];
    // Получить mac - адрес
    if (ioctl(sockfd, SIOCGIFHWADDR, &ifr) == -1) {
        perror("SIOCGIFHWADDR");
        return 1;
    }
    // скопировать mac
    memcpy(src_mac, ifr.ifr_hwaddr.sa_data, ETH_ALEN);

    struct eth_frame frame;
    // MAC назначения
    memcpy(frame.hdr.h_dest, target_mac, ETH_ALEN);
    // MAC источника
    memcpy(frame.hdr.h_source, src_mac, ETH_ALEN);
    // Тип протокола
    frame.hdr.h_proto = htons(0x1234);

    // длина сообщения
    size_t msglen = strlen(msg);
    // Копируем "hello world" в payload
    memcpy(frame.payload, msg, msglen);

    struct sockaddr_ll addr = {0};
    // Тип адреса
    addr.sll_family = AF_PACKET;
    // Через какой интерфейс отправлять
    addr.sll_ifindex = index;
    // Длина MAC-адреса
    addr.sll_halen = ETH_ALEN;
    // MAC назначенияx
    memcpy(addr.sll_addr, target_mac, ETH_ALEN);

    // отправка кадра
    // sockfd	raw socket
    // &frame	указатель на Ethernet-кадр
    // msglen + ETH_HLEN	длина кадра
    // addr	куда отправлять
    if (sendto(sockfd, &frame, msglen + ETH_HLEN, 0,
               (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("sendto");
    } else {
        printf("Frame sent successfully\n");
    }
    // Закрываем сокет
    close(sockfd);
    return 0;
}
