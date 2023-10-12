/**
  *******************************CopyRight  ************************************
  * @file    lv_scr_mgr_port.c
  * @author  zyf
  * @date    2023-10-11 13:51:24
  * @brief   &#&
  *          
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lv_scr_mgr_port.h"

#if LV_SCR_MGR_REG_ENABLE

#else

extern const lv_scr_mgr_handle_t s_scr_mgr_handle_test1;
extern const lv_scr_mgr_handle_t s_scr_mgr_handle_test2;
extern const lv_scr_mgr_handle_t s_scr_mgr_handle_test3;
static const lv_scr_mgr_handle_t* scr_mgr_handles[] = {
    & s_scr_mgr_handle_test1,
    & s_scr_mgr_handle_test2,
    & s_scr_mgr_handle_test3,
};

#endif

/************************ (C) COPYRIGHT ***********END OF FILE*****************/


