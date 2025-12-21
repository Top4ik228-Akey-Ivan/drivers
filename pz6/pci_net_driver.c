#include <linux/module.h>
#include <linux/pci.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/io.h>

#define DRV_NAME "pci_net_lab"

/* Intel 82540EM */
#define PCI_VENDOR_ID_LAB 0x8086
#define PCI_DEVICE_ID_LAB 0x100e

// приватные данные драйвера
struct lab_priv {
    // mmio — адрес памяти устройства
    void __iomem *mmio;
    // netdev — сетевой интерфейс Linux
    struct net_device *netdev;
};

/* ---------------- net_device ops ---------------- */

// Включение интерфейса
static int lab_open(struct net_device *dev)
{
    // ip link set eth0 up => netif_start_queue(dev) => «можно принимать пакеты»
    netif_start_queue(dev);
    return 0;
}

static int lab_stop(struct net_device *dev)
{
    // прекратить передачу пактов / не пополнять очередь
    netif_stop_queue(dev);
    return 0;
}

static const struct net_device_ops lab_netdev_ops = {
    // вызывается при ip link set eth0 up
    .ndo_open = lab_open,
    // вызывается при ip link set eth0 down
    .ndo_stop = lab_stop,
};

/* ---------------- PCI PROBE ---------------- */

// Эта функция вызывается автоматически, когда: в системе есть PCI-устройство и его ID совпадает с таблицей драйвера

static int lab_pci_probe(struct pci_dev *pdev,
                         const struct pci_device_id *id)
{
    struct net_device *netdev;
    struct lab_priv *priv;
    int err;
    u8 mac[ETH_ALEN];

    //id производителя / id устройства
    pr_info(DRV_NAME ": probe called for %04x:%04x\n",
            pdev->vendor, pdev->device);

    // Включаем устройство
    err = pci_enable_device(pdev);
    if (err)
        return err;
    
    // резервируем память
    err = pci_request_regions(pdev, DRV_NAME);
    if (err)
        goto err_disable;

    // Создаётся eth-устройство + место для lab_priv
    netdev = alloc_etherdev(sizeof(struct lab_priv));
    if (!netdev) {
        err = -ENOMEM;
        goto err_regions;
    }

    // Связываем структуру драйвера с интерфейсом 
    // Вот память под твои драйверные данные, пользуйся!
    priv = netdev_priv(netdev);
    // сохраняем ссылку на сам интерфейс.
    priv->netdev = netdev;

    // pdev — это структура struct pci_dev, которая представляет PCI-устройство в системе.
    // priv — это приватная структура драйвера (lab_priv), где хранится всё, что нужно драйверу для работы с устройством.
    // Функция pci_set_drvdata() связывает устройство PCI с приватными данными драйвера.
    pci_set_drvdata(pdev, priv);

    // pci_iomap() — функция ядра Linux, которая позволяет драйверу получить доступ к памяти устройства PCI
    // pdev — PCI-устройство. / 0 - номер устройства, где находится память. . 0 - длина маппинга; 0 значит «весь доступный размер».
    priv->mmio = pci_iomap(pdev, 0, 0);
    if (!priv->mmio) {
        err = -EIO;
        // прыгает к метке 
        goto err_free;
    }

    /*
     * Intel e1000:
     * MAC address stored in registers RAL/RAH
     * RAL = 0x5400
     */
    // mac адрес он лежит в памяти по адресу 0x5400 / читаем 6 байт
    mac[0] = ioread8(priv->mmio + 0x5400);
    mac[1] = ioread8(priv->mmio + 0x5401);
    mac[2] = ioread8(priv->mmio + 0x5402);
    mac[3] = ioread8(priv->mmio + 0x5403);
    mac[4] = ioread8(priv->mmio + 0x5404);
    mac[5] = ioread8(priv->mmio + 0x5405);

    // задаем реальный mac
    eth_hw_addr_set(netdev, mac);

    // связываем netdev с функциями 
    netdev->netdev_ops = &lab_netdev_ops;

    err = register_netdev(netdev);
    if (err)
        goto err_iounmap;

    pr_info(DRV_NAME ": netdev %s registered\n", netdev->name);
    pr_info(DRV_NAME ": REAL MAC address %pM\n", netdev->dev_addr);

    return 0;

// Если ошибка произошла после pci_iomap(), нужно сначала размэпить память:
err_iounmap:
    pci_iounmap(pdev, priv->mmio);
// Если ошибка произошла после создания net_device, нужно её освободить:
err_free:
    free_netdev(netdev);
// Если ошибка произошла после запроса ресурсов PCI, нужно их отпустить:
err_regions:
    pci_release_regions(pdev);
// Если ошибка произошла после включения устройства, нужно его выключить:
err_disable:
    pci_disable_device(pdev);
    return err;
}

/* ---------------- PCI REMOVE ---------------- */

// Эта функция вызывается, когда PCI-устройство удаляется из системы или драйвер выгружается.
static void lab_pci_remove(struct pci_dev *pdev)
{
    // Получение приватных данных драйвера
    struct lab_priv *priv = pci_get_drvdata(pdev);

    pr_info(DRV_NAME ": remove called\n");
    // Убираем интерфейс из системы (ip link больше не покажет его).
    unregister_netdev(priv->netdev);
    // Отвязываем память MMIO, которую мы мапили при probe.
    pci_iounmap(pdev, priv->mmio);
    // Удаляем структуру net_device и её приватные данные (lab_priv).
    free_netdev(priv->netdev);
    // Освобождаем ресурсы PCI (порты, память), которые мы захватывали через pci_request_regions
    pci_release_regions(pdev);
    // говорим ядру, что PCI-устройство больше не используется.
    pci_disable_device(pdev);
}

/* ---------------- PCI ID TABLE ---------------- */

// Таблица PCI устройств
static const struct pci_device_id lab_pci_ids[] = {
    { PCI_DEVICE(PCI_VENDOR_ID_LAB, PCI_DEVICE_ID_LAB) },
    // маркер конца таблицы
    { 0 }  
};
// сообщает ядру, какие устройства могут быть автоматически загружены с этим драйвером.
MODULE_DEVICE_TABLE(pci, lab_pci_ids);

// Структура драйвера pci_driver
static struct pci_driver lab_pci_driver = {
    .name = DRV_NAME,
    .id_table = lab_pci_ids,
    .probe = lab_pci_probe,
    .remove = lab_pci_remove,
};

// Регистрация драйвера / insmod → драйвер загружается
module_pci_driver(lab_pci_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ivan");
MODULE_DESCRIPTION("Real PCI network driver (educational)");
