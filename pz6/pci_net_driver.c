#include <linux/module.h>
#include <linux/pci.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/io.h>

#define DRV_NAME "pci_net_lab"

/* Intel 82540EM */
#define PCI_VENDOR_ID_LAB 0x8086
#define PCI_DEVICE_ID_LAB 0x100e

struct lab_priv {
    void __iomem *mmio;
    struct net_device *netdev;
};

/* ---------------- net_device ops ---------------- */

static int lab_open(struct net_device *dev)
{
    netif_start_queue(dev);
    return 0;
}

static int lab_stop(struct net_device *dev)
{
    netif_stop_queue(dev);
    return 0;
}

static const struct net_device_ops lab_netdev_ops = {
    .ndo_open = lab_open,
    .ndo_stop = lab_stop,
};

/* ---------------- PCI PROBE ---------------- */

static int lab_pci_probe(struct pci_dev *pdev,
                         const struct pci_device_id *id)
{
    struct net_device *netdev;
    struct lab_priv *priv;
    int err;
    u8 mac[ETH_ALEN];

    pr_info(DRV_NAME ": probe called for %04x:%04x\n",
            pdev->vendor, pdev->device);

    err = pci_enable_device(pdev);
    if (err)
        return err;

    err = pci_request_regions(pdev, DRV_NAME);
    if (err)
        goto err_disable;

    netdev = alloc_etherdev(sizeof(struct lab_priv));
    if (!netdev) {
        err = -ENOMEM;
        goto err_regions;
    }

    priv = netdev_priv(netdev);
    priv->netdev = netdev;

    pci_set_drvdata(pdev, priv);

    priv->mmio = pci_iomap(pdev, 0, 0);
    if (!priv->mmio) {
        err = -EIO;
        goto err_free;
    }

    /*
     * Intel e1000:
     * MAC address stored in registers RAL/RAH
     * RAL = 0x5400
     */
    mac[0] = ioread8(priv->mmio + 0x5400);
    mac[1] = ioread8(priv->mmio + 0x5401);
    mac[2] = ioread8(priv->mmio + 0x5402);
    mac[3] = ioread8(priv->mmio + 0x5403);
    mac[4] = ioread8(priv->mmio + 0x5404);
    mac[5] = ioread8(priv->mmio + 0x5405);

    eth_hw_addr_set(netdev, mac);

    netdev->netdev_ops = &lab_netdev_ops;

    err = register_netdev(netdev);
    if (err)
        goto err_iounmap;

    pr_info(DRV_NAME ": netdev %s registered\n", netdev->name);
    pr_info(DRV_NAME ": REAL MAC address %pM\n", netdev->dev_addr);

    return 0;

err_iounmap:
    pci_iounmap(pdev, priv->mmio);
err_free:
    free_netdev(netdev);
err_regions:
    pci_release_regions(pdev);
err_disable:
    pci_disable_device(pdev);
    return err;
}

/* ---------------- PCI REMOVE ---------------- */

static void lab_pci_remove(struct pci_dev *pdev)
{
    struct lab_priv *priv = pci_get_drvdata(pdev);

    pr_info(DRV_NAME ": remove called\n");

    unregister_netdev(priv->netdev);
    pci_iounmap(pdev, priv->mmio);
    free_netdev(priv->netdev);
    pci_release_regions(pdev);
    pci_disable_device(pdev);
}

/* ---------------- PCI ID TABLE ---------------- */

static const struct pci_device_id lab_pci_ids[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_LAB, PCI_DEVICE_ID_LAB) },
    { 0 }
};
MODULE_DEVICE_TABLE(pci, lab_pci_ids);

static struct pci_driver lab_pci_driver = {
    .name = DRV_NAME,
    .id_table = lab_pci_ids,
    .probe = lab_pci_probe,
    .remove = lab_pci_remove,
};

module_pci_driver(lab_pci_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Lab work");
MODULE_DESCRIPTION("Real PCI network driver (educational)");
