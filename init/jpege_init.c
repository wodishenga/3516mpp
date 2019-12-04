/******************************************************************************

  Copyright (C), 2017, Hisilicon Tech. Co., Ltd.

 ******************************************************************************
  File Name     : jpege_init.c
  Version       : Initial Draft
  Author        : Hisilicon multimedia software group
  Created       : 2017
  Description   :
******************************************************************************/

#include <linux/module.h>


extern int JPEGE_ModInit(void);
extern void JPEGE_ModExit(void);


static int __init jpege_mod_init(void){
    JPEGE_ModInit();
    return 0;
}
static void __exit jpege_mod_exit(void){
    JPEGE_ModExit();
}

module_init(jpege_mod_init);
module_exit(jpege_mod_exit);

MODULE_LICENSE("Proprietary");




