#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#define MODNAME "rawdemo_driver"

// Это главный объект сетевого интерфейса
static struct net_device *vnet;

// Вызывается, когда делаем ip link set dev ethX up:
static int drv_open(struct net_device *ndev)
{
    pr_info(MODNAME ": device opened\n");
    // Разрешаем ядру передавать пакеты через этот интерфейс
    netif_start_queue(ndev);
    return 0;
}
// Вызывается при ip link set dev ethX down
static int drv_stop(struct net_device *ndev)
{
    pr_info(MODNAME ": device stopped\n");
    // Запрещаем ядру отправлять пакеты
    netif_stop_queue(ndev);
    return 0;
}

// skb — это сетевой пакет
//skb->data — данные пакета
// skb->len — длина
static netdev_tx_t drv_send(struct sk_buff *skb, struct net_device *ndev)
{
    // Счётчик для циклов
    unsigned int i;

    pr_info(MODNAME ": transmit invoked\n");
    pr_info(MODNAME ": skb length=%u\n", skb->len);

    pr_info(MODNAME ": RAW HEX: ");
    // Проходим по каждому байту пакета => Печатаем его в шестнадцатеричном виде
    for (i = 0; i < skb->len; i++)
    // %02x - формат вывода 2-кол-во знаков x - 16сс
        pr_cont("%02x ", skb->data[i]);
    pr_cont("\n");

    // Печать пакета как ASCII
    pr_info(MODNAME ": ASCII: ");
    for (i = 0; i < skb->len; i++)
        pr_cont("%c ", skb->data[i]);
    pr_cont("\n");

    // это статистика сетевого интерфейса.
    // Увеличиваем счётчик отправленных пакетов
    ndev->stats.tx_packets++;
    // Увеличиваем счётчик байтов
    ndev->stats.tx_bytes += skb->len;

    // Мы обязаны освободить skb(пакет), иначе утечка памяти
    dev_kfree_skb(skb);
    return NETDEV_TX_OK;
}

static const struct net_device_ops drv_ops = {
    //Когда интерфейс поднимают
    .ndo_open        = drv_open,
    // Когда интерфейс опускают
    .ndo_stop        = drv_stop,
    // Когда отправляют пакет
    .ndo_start_xmit  = drv_send,
};

static int __init drv_init(void)
{
    // Создаём Ethernet-устройство / назначается MAC-адрес
    vnet = alloc_etherdev(0);
    // Если не хватило памяти — ошибка
    if (!vnet)
        return -ENOMEM;

    // Это привязка драйвера к сетевому устройству.
    vnet->netdev_ops = &drv_ops;

    // Регистрируем интерфейс в ядре После этого он появится в ip link
    if (register_netdev(vnet)) {
        pr_err(MODNAME ": failed to register device\n");
        // Чистим память и выходим
        free_netdev(vnet);
        return -1;
    }

    pr_info(MODNAME ": driver loaded\n");
    return 0;
}

static void __exit drv_exit(void)
{
    // Убираем интерфейс из системы
    unregister_netdev(vnet);
    // Освобождаем память
    free_netdev(vnet);
    pr_info(MODNAME ": driver unloaded\n");
}


//Указываем ядру: что вызывать при загрузке / что при выгрузке
module_init(drv_init);
module_exit(drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rewritten");
MODULE_DESCRIPTION("Unique version of raw Ethernet demo driver");