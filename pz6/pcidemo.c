#include <linux/module.h>
#include <linux/pci.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#define DRV_NAME "pci_eth_demo"

/* Подставлено твое устройство */
static const struct pci_device_id pci_tbl[] = {
    { PCI_DEVICE(0x8086, 0x100e) },
    { 0 }
};
MODULE_DEVICE_TABLE(pci, pci_tbl);

static struct net_device *vnet;

static int drv_open(struct net_device *ndev)
{
    pr_info(DRV_NAME ": netdev open\n");
    netif_start_queue(ndev);
    return 0;
}

static int drv_stop(struct net_device *ndev)
{
    pr_info(DRV_NAME ": netdev stop\n");
    netif_stop_queue(ndev);
    return 0;
}

static netdev_tx_t drv_send(struct sk_buff *skb, struct net_device *ndev)
{
    unsigned int i;

    pr_info(DRV_NAME ": transmit invoked, skb=%u bytes\n", skb->len);

    pr_info(DRV_NAME ": RAW HEX: ");
    for (i = 0; i < skb->len; i++)
        pr_cont("%02x ", skb->data[i]);
    pr_cont("\n");

    pr_info(DRV_NAME ": ASCII: ");
    for (i = 0; i < skb->len; i++)
        pr_cont("%c ", skb->data[i]);
    pr_cont("\n");

    ndev->stats.tx_packets++;
    ndev->stats.tx_bytes += skb->len;

    dev_kfree_skb(skb);
    return NETDEV_TX_OK;
}

static const struct net_device_ops drv_ops = {
    .ndo_open       = drv_open,
    .ndo_stop       = drv_stop,
    .ndo_start_xmit = drv_send,
};

/* PCI probe */
static int pci_eth_probe(struct pci_dev *pdev, const struct pci_device_id *id)
{
    int err;
    u8 mac[6];

    pr_info(DRV_NAME ": PCI probe started\n");

    err = pci_enable_device(pdev);
    if (err) {
        pr_err(DRV_NAME ": pci_enable_device failed\n");
        return err;
    }

    /* Чтение MAC адреса из PCI config space */
    pci_read_config_byte(pdev, 0, &mac[0]);
    pci_read_config_byte(pdev, 1, &mac[1]);
    pci_read_config_byte(pdev, 2, &mac[2]);
    pci_read_config_byte(pdev, 3, &mac[3]);
    pci_read_config_byte(pdev, 4, &mac[4]);
    pci_read_config_byte(pdev, 5, &mac[5]);

    pr_info(DRV_NAME ": MAC read from PCI config: %pM\n", mac);

    /* Создаем виртуальный сетевой интерфейс */
    vnet = alloc_etherdev(0);
    if (!vnet)
        return -ENOMEM;

    memcpy(vnet->dev_addr, mac, ETH_ALEN);
    vnet->netdev_ops = &drv_ops;

    err = register_netdev(vnet);
    if (err) {
        pr_err(DRV_NAME ": Failed to register netdevice\n");
        free_netdev(vnet);
        return err;
    }

    pci_set_drvdata(pdev, vnet);

    pr_info(DRV_NAME ": PCI probe OK, virtual NIC ready\n");
    return 0;
}

/* PCI remove */
static void pci_eth_remove(struct pci_dev *pdev)
{
    struct net_device *ndev = pci_get_drvdata(pdev);

    unregister_netdev(ndev);
    free_netdev(ndev);

    pci_disable_device(pdev);

    pr_info(DRV_NAME ": PCI device removed\n");
}

static struct pci_driver pci_eth_driver = {
    .name     = DRV_NAME,
    .id_table = pci_tbl,
    .probe    = pci_eth_probe,
    .remove   = pci_eth_remove,
};

module_pci_driver(pci_eth_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivan");
MODULE_DESCRIPTION("PCI + Ethernet demo driver for 82540EM");
