#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>

#define MODNAME "rawdemo_driver"

static struct net_device *vnet;

static int drv_open(struct net_device *ndev)
{
    pr_info(MODNAME ": device opened\n");
    netif_start_queue(ndev);
    return 0;
}

static int drv_stop(struct net_device *ndev)
{
    pr_info(MODNAME ": device stopped\n");
    netif_stop_queue(ndev);
    return 0;
}

static netdev_tx_t drv_send(struct sk_buff *skb, struct net_device *ndev)
{
    unsigned int i;

    pr_info(MODNAME ": transmit invoked\n");
    pr_info(MODNAME ": skb length=%u\n", skb->len);

    pr_info(MODNAME ": RAW HEX: ");
    for (i = 0; i < skb->len; i++)
        pr_cont("%02x ", skb->data[i]);
    pr_cont("\n");

    pr_info(MODNAME ": ASCII: ");
    for (i = 0; i < skb->len; i++)
        pr_cont("%c ", skb->data[i]);
    pr_cont("\n");

    ndev->stats.tx_packets++;
    ndev->stats.tx_bytes += skb->len;

    dev_kfree_skb(skb);
    return NETDEV_TX_OK;
}

static const struct net_device_ops drv_ops = {
    .ndo_open        = drv_open,
    .ndo_stop        = drv_stop,
    .ndo_start_xmit  = drv_send,
};

static int __init drv_init(void)
{
    vnet = alloc_etherdev(0);
    if (!vnet)
        return -ENOMEM;

    vnet->netdev_ops = &drv_ops;

    if (register_netdev(vnet)) {
        pr_err(MODNAME ": failed to register device\n");
        free_netdev(vnet);
        return -1;
    }

    pr_info(MODNAME ": driver loaded\n");
    return 0;
}

static void __exit drv_exit(void)
{
    unregister_netdev(vnet);
    free_netdev(vnet);
    pr_info(MODNAME ": driver unloaded\n");
}

module_init(drv_init);
module_exit(drv_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rewritten");
MODULE_DESCRIPTION("Unique version of raw Ethernet demo driver");