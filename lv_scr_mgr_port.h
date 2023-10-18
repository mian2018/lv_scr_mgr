/**
  *******************************CopyRight  ************************************
  * @file    lv_scr_mgr_port.h
  * @author  zyf
  * @date    2023-10-11 13:51:23
  * @brief   lvgl 页面管理器 接口
  *          
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _LV_SCR_MGR_PORT_H_
#define _LV_SCR_MGR_PORT_H_

/* Includes ------------------------------------------------------------------*/
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum
{
    eScreenUITest1 = 0,
    eScreenUITest2,
    eScreenUITest3
}lv_scr_mgr_id_e;

#if !LV_SCR_MGR_REG_ENABLE
uint32_t lv_scr_mgr_get_cnt(void);
const lv_scr_mgr_handle_t** lv_scr_mgr_get_handles(void);
#endif
#ifdef __cplusplus
}
#endif

#endif /* _LV_SCR_MGR_PORT_H_ */

/************************ (C) COPYRIGHT ***********END OF FILE*****************/


