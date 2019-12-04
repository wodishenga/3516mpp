#include <linux/module.h>
#include <linux/of_platform.h>

#include "hi_defines.h"
#include "hi_type.h"
#include "hi_osal.h"
#include "vou_exp.h"

extern VOU_EXPORT_SYMBOL_S g_stVouExpSymbol;

EXPORT_SYMBOL(g_stVouExpSymbol);

extern int VOU_ModInit(void);
extern void VOU_ModExit(void);

extern void * pVoReg;
extern unsigned int vou_irq;

static int hi35xx_vo_probe(struct platform_device *pdev)
{
    struct resource *mem;

    mem = osal_platform_get_resource(pdev, IORESOURCE_MEM, 0);
    pVoReg = devm_ioremap_resource(&pdev->dev, mem);
    if (IS_ERR(pVoReg))
            return PTR_ERR(pVoReg);

    vou_irq = osal_platform_get_irq(pdev, 0);
    if (vou_irq <= 0) {
            dev_err(&pdev->dev, "cannot find vou IRQ\n");
    }
    //printk("++++++++++ pVoReg = %p vou_irq = %d\n",pVoReg,vou_irq);

    VOU_ModInit();

    return 0;
}

static int hi35xx_vo_remove(struct platform_device *pdev)
{
    VOU_ModExit();
    pVoReg = NULL;
    return 0;
}


static const struct of_device_id hi35xx_vo_match[] = {
        { .compatible = "hisilicon,hisi-vo" },
        {},
};
MODULE_DEVICE_TABLE(of, hi35xx_vo_match);

static struct platform_driver hi35xx_vo_driver = {
        .probe          = hi35xx_vo_probe,
        .remove         = hi35xx_vo_remove,
        .driver         = {
                .name   = "hi35xx_vo",
                .of_match_table = hi35xx_vo_match,
        },
};

osal_module_platform_driver(hi35xx_vo_driver);

MODULE_LICENSE("Proprietary");
