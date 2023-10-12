/**
  *******************************CopyRight  ************************************
  * @file    lv_scr_mgr.h
  * @author  zyf
  * @date    2023-10-11 9:31:49
  * @brief   &#&
  *          
  ******************************************************************************
  */
#ifndef _LV_SCR_MGR_H_
#define _LV_SCR_MGR_H_

/* Includes ------------------------------------------------------------------*/
#include "stdint.h"
#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif
 
/*!<  界面切换动画默认值
 */
#define LV_SCR_MGR_LOAD_ANIM_DEFAULT   LV_SCR_LOAD_ANIM_MOVE_LEFT
#define LV_SCR_MGR_LOAD_ANIM_TIME      500
#define LV_SCR_MGR_LOAD_ANIM_DELAY     0

/*!< 内存泄漏检测 */
#define LV_SCR_MGR_PRINTF_MEM          1   

#define LV_SCR_MGR_REG_ENABLE          0


#if LV_SCR_MGR_REG_ENABLE

#else
     
#endif

typedef struct
{
    uint32_t scr_id;                       /*!< id */
    lv_scr_load_anim_t      anim_type;     /*!< 切换动画类型 如果为空，则使用 LV_SCR_MGR_LOAD_ANIM_DEFAULT */
    void (*scr_first_create)(void);        /*!< lv_scr_mgr_switch  lv_scr_mgr_push 函数会调用该创建函数 pop则不会调用 可以方便实现pop记住焦点 而push使用默认焦点 */
    lv_obj_t* (*scr_create) (const uint32_t id, void* param); /*!< 创建界面，创建界面时不要使用 lv_scr_mgr_xxx 函数 */
    void (*scr_destroy)(void);             /*!< 删除界面的回调函数，一般用于删除如 lv_timer 等不会随界面自动删除的资源 */
}lv_scr_mgr_handle_t;

typedef struct _lv_scr_mgr_stack_node_t
{
    lv_scr_mgr_handle_t* handle;
    lv_obj_t* scr;
    struct _lv_scr_mgr_stack_node_t* prev;
    struct _lv_scr_mgr_stack_node_t* next;
}lv_scr_mgr_stack_node_t;

bool lv_scr_mgr_init(void* param);
void lv_scr_mgr_deinit(void);
void lv_scr_mgr_param_set(void* param);
void* lv_scr_mgr_param_get(void);

bool lv_scr_mgr_switch(uint32_t id, bool anim);
bool lv_scr_mgr_push(uint32_t id, bool anim);
bool lv_scr_mgr_popn(uint32_t n, bool anim);
bool lv_scr_mgr_pop(bool anim);
bool lv_scr_mgr_pop_root(bool anim);
int32_t lv_scr_mgr_get_cur_id(void);
int32_t lv_scr_mgr_get_root_id(void);
#ifdef __cplusplus
}
#endif

#endif /* _LV_SCR_MGR_H_ */

/************************ (C) COPYRIGHT *****END OF FILE*****************/



