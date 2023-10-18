/**
  *******************************CopyRight  ************************************
  * @file    lv_scr_mgr.h
  * @author  zyf
  * @date    2023-10-11 9:31:49
  * @brief   lvgl 页面管理器
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

/*!< 内存泄漏检测 注意，页面管理器的push操作也会申请动态内存
 * 如果使用动画，则需要将 lv_disp.c 中的 scr_anim_ready 改为如下
    static void scr_anim_ready(lv_anim_t * a)
    {
        lv_disp_t * d = lv_obj_get_disp(a->var);

        lv_event_send(d->prev_scr, LV_EVENT_SCREEN_UNLOADED, NULL);

        if(d->prev_scr && d->del_prev) lv_obj_del(d->prev_scr);
        d->prev_scr = NULL;
        d->draw_prev_over_act = false;
        d->scr_to_load = NULL;
        lv_event_send(d->act_scr, LV_EVENT_SCREEN_LOADED, NULL);
        lv_obj_remove_local_style_prop(a->var, LV_STYLE_OPA, 0);
        lv_obj_invalidate(d->act_scr);
    }
 */
#define LV_SCR_MGR_PRINTF_MEM          1   

/* 使用分散加载的方式 将 lv_scr_mgr_handle_t 存放在特定段，
 * 用户创建 lv_scr_mgr_handle_t 后可以在后面直接 使用 LV_SCR_MGR_REG() 对界面句柄进行注册
 * 而不用将 界面句柄 添加到 lv_scr_mgr_port.c 数组中
 */
#define LV_SCR_MGR_REG_ENABLE          0





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
    const lv_scr_mgr_handle_t* handle;
    lv_obj_t* scr;
    struct _lv_scr_mgr_stack_node_t* prev;
    struct _lv_scr_mgr_stack_node_t* next;
}lv_scr_mgr_stack_node_t;

#include "lv_scr_mgr_port.h"
#if LV_SCR_MGR_REG_ENABLE
#define  ANONY_CONN(type, var)   type  var
#define  ANONY_DEF(type,prefix)  ANONY_CONN(type, prefix)
#define  ANONY_TYPE(type,prefix) ANONY_DEF(type, prefix)

#if defined(__CC_ARM) || defined(__GNUC__) /* ARM,GCC*/
#define SECTION(x)                  __attribute__((section(x)))
#define USED                        __attribute__((used))
#elif defined (__ICCARM__)              /*IAR */
#define SECTION(x)                  @ x
#define USED                        __root
#else
#error "Current tool chain haven't supported yet!"
#endif


#define _LV_SCR_MGR_REG(handle, level)           \
    USED ANONY_TYPE(const lv_scr_mgr_handle_t*, scr_mgr_##handle)\
    SECTION("scr_mgr."level) = &(handle)
    
#define LV_SCR_MGR_REG(handle)  _LV_SCR_MGR_REG(handle, "1") 
#else
#define LV_SCR_MGR_REG(handle)     
#endif
    

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



