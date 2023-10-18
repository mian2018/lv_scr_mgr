/**
  *******************************CopyRight  ************************************
  * @file    lv_scr_mgr_port.c
  * @author  zyf
  * @date    2023-10-11 13:51:24
  * @brief   lvgl 页面管理器 接口
  *          
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lv_scr_mgr.h"
#include "lv_scr_mgr_port.h"


#if LV_SCR_MGR_REG_ENABLE

/* 无需用户修改 */
const lv_scr_mgr_handle_t  scr_mgr_start = {0};
_LV_SCR_MGR_REG(scr_mgr_start, "0");
const lv_scr_mgr_handle_t  scr_mgr_end = {0};
_LV_SCR_MGR_REG(scr_mgr_end, "2");

#else

/* 需要用户添加自己的界面句柄 */
extern const lv_scr_mgr_handle_t s_scr_mgr_handle_test1;
extern const lv_scr_mgr_handle_t s_scr_mgr_handle_test2;
extern const lv_scr_mgr_handle_t s_scr_mgr_handle_test3;
static const lv_scr_mgr_handle_t* scr_mgr_handles[] = {
    & s_scr_mgr_handle_test1,
    & s_scr_mgr_handle_test2,
    & s_scr_mgr_handle_test3,
};

uint32_t lv_scr_mgr_get_cnt(void)
{
    return sizeof(scr_mgr_handles) / sizeof(scr_mgr_handles[0]);
}

const lv_scr_mgr_handle_t** lv_scr_mgr_get_handles(void)
{
    return scr_mgr_handles;
}

#endif

/************************ (C) COPYRIGHT ***********END OF FILE*****************/


