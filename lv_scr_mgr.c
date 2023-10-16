/**
  *******************************CopyRight  ************************************
  * @file    lv_scr_mgr.c
  * @author  mian2018
  * @date    2023-10-11 13:4:36
  * @brief   &#&
  *          
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "lvgl.h"
#include "lv_scr_mgr.h"
#if !LV_SCR_MGR_REG_ENABLE
#include "lv_scr_mgr_port.c"
#endif

typedef struct
{
    uint32_t               scr_cnt; 
    void*                  param;
    lv_scr_mgr_handle_t    **handles;
#if LV_SCR_MGR_PRINTF_MEM
    uint32_t               *max_mem;
#endif
}scr_mgr_list_handle_t;


static scr_mgr_list_handle_t     mgr_list;
static lv_scr_mgr_stack_node_t*      mgr_stack_top = NULL;
static lv_scr_mgr_stack_node_t*      mgr_stack_root = NULL;

static lv_scr_mgr_handle_t* find_handle_by_id(uint32_t id)
{
    for (int i = 0; i < mgr_list.scr_cnt; i++)
    {
        if (mgr_list.handles[i]->scr_id == id)
        {
            return mgr_list.handles[i];
        }
    }
    return NULL;
}
#if LV_SCR_MGR_PRINTF_MEM
static uint32_t* find_mem_addr_by_id(uint32_t id)
{
    for (int i = 0; i < mgr_list.scr_cnt; i++)
    {
        if (mgr_list.handles[i]->scr_id == id)
        {
            return &mgr_list.max_mem[i];
        }
    }
    return NULL;
}

static void mem_max_printf(uint32_t id)
{
    static uint32_t mem_max = 0;
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    if (mon.total_size - mon.free_size > mem_max)
    {
        mem_max = mon.total_size - mon.free_size;
        LV_LOG_USER("used: %d (%d %%), frag: %d %%, biggest free: %d\n", mem_max,
            mon.used_pct,
            mon.frag_pct,
            (int)mon.free_biggest_size);
    }
}

static void page_mem_max_printf(uint32_t id)
{
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    /* 当前界面最大使用内存 */
    uint32_t* page_max_mem = find_mem_addr_by_id(id);
    if (mon.total_size - mon.free_size > *page_max_mem)
    {
        *page_max_mem = mon.total_size - mon.free_size;
        LV_LOG_USER("page id %d, used: %d (%d %%), frag: %d %%, biggest free: %d\n", id, *page_max_mem,
            mon.used_pct,
            mon.frag_pct,
            (int)mon.free_biggest_size);
    }
}


static void anim_mem_max_printf(lv_event_t* e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    if (event_code == LV_EVENT_SCREEN_LOADED)
    {
        page_mem_max_printf((uint32_t)lv_event_get_user_data(e));
    }
}
#endif

static void scr_mgr_stack_free(void)
{
    lv_scr_mgr_stack_node_t* stack_node = NULL;

    /* 释放界面栈 */
    while (NULL != mgr_stack_top)
    {
        stack_node = mgr_stack_top->prev;
        if(mgr_stack_top->handle->scr_destroy)
            mgr_stack_top->handle->scr_destroy();
        lv_mem_free((void*)mgr_stack_top);
        mgr_stack_top = stack_node;
    }
    mgr_stack_root = NULL;
}

/**
 * @brief 入栈
 * @param tag 要入栈的句柄
 * @return 栈顶句柄
*/
static lv_scr_mgr_stack_node_t* scr_mgr_stack_push(lv_scr_mgr_handle_t* tag)
{
    lv_scr_mgr_stack_node_t* stack_node = NULL;
    stack_node = lv_mem_alloc(sizeof(lv_scr_mgr_stack_node_t));
    LV_ASSERT_MALLOC(stack_node);
    stack_node->handle = tag;
    stack_node->next = NULL;
    if (stack_node->handle->scr_first_create)
    {
        stack_node->handle->scr_first_create();
    }

    if (tag->scr_create)
    {
        stack_node->scr = tag->scr_create(stack_node->handle->scr_id, mgr_list.param);
    }
    else
    {
        LV_LOG_ERROR("no create fun!");
    }

    if (NULL == mgr_stack_top)
    {
        stack_node->prev = NULL;
        mgr_stack_root = stack_node;
    }
    else
    {
        stack_node->prev = mgr_stack_top;
        mgr_stack_top->next = stack_node;
    }
    mgr_stack_top = stack_node;
    return stack_node;
}

static int32_t scr_mgr_stack_pop(int32_t n)
{
    lv_scr_mgr_stack_node_t* stack_node = NULL;
    int32_t i = n;

    if ((NULL == mgr_stack_top) || (NULL == mgr_stack_top->prev))
    {
        return 0;
    }

    while (i)
    {
        if ((NULL == mgr_stack_top) || (NULL == mgr_stack_top->prev))
        {
            break;
        }

        stack_node = mgr_stack_top->prev;
        if (mgr_stack_top->handle->scr_destroy)
        {
            mgr_stack_top->handle->scr_destroy();
        }
        lv_mem_free((void*)mgr_stack_top);
        mgr_stack_top = stack_node;
        i--;
    }

    if (NULL != mgr_stack_top->handle->scr_create)
    {
        mgr_stack_top->scr = mgr_stack_top->handle->scr_create(mgr_stack_top->handle->scr_id, mgr_list.param);
    }
    else
    {
        LV_LOG_ERROR("no create fun!");
    }

    if (i)
    {
        LV_LOG_WARN("stack pop %d, but stack is %d", n, n-i);
    }
    return n - i;
}

/**
 * @brief 切换界面
 * @param cur_scr 当前界面
 * @param stack_node 目标界面句柄
 * @param anim 切换界面动画开关
 *             关闭界面切换动画，切换界面时会先创建一个新的空界面，切换到空界面后，
 *             删除之前的界面，然后再创建切换到新界面，最后再删除中间界面。会节省内存。
 *             关闭界面切换动画，切换界面时直接创建新界面，然后再用动画切换到新界面。
 *
 * @return true
*/
bool scr_mgr_switch(lv_obj_t* cur_scr, lv_scr_mgr_stack_node_t* stack_node, bool anim)
{
    lv_scr_load_anim_t load_anim = LV_SCR_MGR_LOAD_ANIM_DEFAULT;
    lv_obj_t* tmp_scr = NULL;

    if (anim)
    {
        if ((stack_node->handle->anim_type != LV_SCR_LOAD_ANIM_NONE) && (LV_SCR_LOAD_ANIM_OUT_BOTTOM >= stack_node->handle->anim_type))
        {
            load_anim = stack_node->handle->anim_type;
        }

#if LV_SCR_MGR_PRINTF_MEM
        mem_max_printf(stack_node->handle->scr_id);
        lv_obj_add_event_cb(stack_node->scr, anim_mem_max_printf, LV_EVENT_SCREEN_LOADED, stack_node->handle->scr_id);
#endif

        lv_scr_load_anim(stack_node->scr, load_anim, LV_SCR_MGR_LOAD_ANIM_TIME, LV_SCR_MGR_LOAD_ANIM_DELAY, true);
    }
    else
    {
        if (NULL != cur_scr)
        {
            tmp_scr = lv_obj_create(NULL);
            lv_scr_load(tmp_scr);
            lv_obj_del(cur_scr);
            cur_scr = NULL;
        }

        lv_scr_load(stack_node->scr);

#if LV_SCR_MGR_PRINTF_MEM
        mem_max_printf(stack_node->handle->scr_id);
        page_mem_max_printf(stack_node->handle->scr_id);
#endif
        if (NULL != tmp_scr)
        {
            lv_obj_del(tmp_scr);
        }
    }
    return true;
}

/**
 * @brief 初始化界面管理器
 * @param param 创建界面时的参数
 * @return 
*/
bool lv_scr_mgr_init(void* param)
{
    mgr_list.param = param;
#if LV_SCR_MGR_REG_ENABLE

#else
    mgr_list.scr_cnt = sizeof(scr_mgr_handles) / sizeof(scr_mgr_handles[0]);
    mgr_list.handles  = scr_mgr_handles;
#endif    

    if (0 == mgr_list.scr_cnt)
    {
        LV_LOG_ERROR("no screen!");
        return false;
    }

#if LV_SCR_MGR_PRINTF_MEM
    mgr_list.max_mem = lv_mem_alloc(mgr_list.scr_cnt * sizeof(uint32_t*));
    LV_ASSERT(mgr_list.max_mem);
    memset(mgr_list.max_mem, 0, mgr_list.scr_cnt * sizeof(uint32_t*));
#endif
    return true;
}

void lv_scr_mgr_deinit(void)
{
    mgr_list.param = NULL;
#if LV_SCR_MGR_PRINTF_MEM
    lv_mem_free(mgr_list.max_mem);
#endif
    scr_mgr_stack_free();
}

void lv_scr_mgr_param_set(void* param)
{
    mgr_list.param = param;
}

void* lv_scr_mgr_param_get(void)
{
    return mgr_list.param;
}

/**
 * @brief 设置根界面
 * @param id 根界面序号
 * @param anim 动画开关
 * @return 
*/
bool lv_scr_mgr_switch(uint32_t id, bool anim)
{
    lv_scr_mgr_handle_t* tag_handle = find_handle_by_id(id);
    lv_scr_mgr_handle_t* cur_handle = NULL;
    lv_scr_mgr_stack_node_t* stack_node = NULL;
    lv_obj_t* cur_scr = NULL;
    
    
    if (NULL == tag_handle)
    {
        LV_LOG_ERROR("no screen, id %d", id);
        return false;
    }


    if (NULL != mgr_stack_top)
    {
        /* 栈内有界面 */
        cur_handle = mgr_stack_top->handle;
        cur_scr = mgr_stack_top->scr;
    }
    else
    {
        cur_scr = lv_scr_act();
    }

    scr_mgr_stack_free();

    if ((NULL == cur_handle) || (tag_handle->scr_id == cur_handle->scr_id))
    {
        /* 没有界面切换，不使用动画效果 */
        anim = false;
    }
    stack_node = scr_mgr_stack_push(tag_handle);

    return scr_mgr_switch(cur_scr, stack_node, anim);
}

/**
 * @brief 入栈一个新的界面
 * @param id 
 * @param anim 
 * @return 
*/
bool lv_scr_mgr_push(uint32_t id, bool anim)
{
    lv_scr_mgr_handle_t* tag_handle = find_handle_by_id(id);
    lv_scr_mgr_stack_node_t* stack_node = NULL;
    lv_obj_t* cur_scr = NULL;

    if (NULL == tag_handle)
    {
        LV_LOG_ERROR("no screen, id %d", id);
        return false;
    }

    if ((NULL == mgr_stack_top) || (NULL == mgr_stack_root))
    {
        LV_LOG_ERROR("no root screen, please use lv_scr_mgr_switch create root screen");
        return false;
    }
    cur_scr = mgr_stack_top->scr;
    stack_node = scr_mgr_stack_push(tag_handle);

    return scr_mgr_switch(cur_scr, stack_node, anim);
}

/**
 * @brief 出栈n个界面
 * @param n 如果栈内界面没有n个，则返回根界面
 * @param anim 
 * @return 
*/
bool lv_scr_mgr_popn(uint32_t n, bool anim)
{
    lv_obj_t* cur_scr = NULL;

    if ((mgr_stack_top == NULL) || (mgr_stack_top->prev == NULL))
    {
        return false;
    }
    cur_scr = mgr_stack_top->scr;

    scr_mgr_stack_pop(n);

    return scr_mgr_switch(cur_scr, mgr_stack_top, anim);
}

/**
 * @brief 出栈一个界面
 * @param anim 
 * @return 
*/
bool lv_scr_mgr_pop(bool anim)
{
    return lv_scr_mgr_popn(1, anim);
}


/**
 * @brief 退回到根界面
 * @param anim 
 * @return 
*/
bool lv_scr_mgr_pop_root(bool anim)
{
    lv_scr_mgr_stack_node_t* stack_node = NULL;
    lv_scr_mgr_stack_node_t* stack_top = NULL;
    uint32_t cnt = 0;
    if (NULL == mgr_stack_root || NULL == mgr_stack_top)
    {
        return false;
    }

    stack_top = mgr_stack_top;

    while (stack_top != NULL)
    {
        cnt++;
        stack_node = stack_top->prev;
        stack_top = stack_node;
    }

    return lv_scr_mgr_popn(cnt-1, anim);
}

/**
 * @brief 获取当前界面id
 * @param  
 * @return 
*/
int32_t lv_scr_mgr_get_cur_id(void)
{
    if (NULL != mgr_stack_top && NULL != mgr_stack_top->handle)
    {
        return mgr_stack_top->handle->scr_id;
    }
    else
    {
        return -1;
    }
}

/**
 * @brief 获取根界面id
 * @param
 * @return
*/
int32_t lv_scr_mgr_get_root_id(void)
{
    if (NULL != mgr_stack_root && NULL != mgr_stack_root->handle)
    {
        return mgr_stack_root->handle->scr_id;
    }
    else
    {
        return -1;
    }
}
/************************ (C) COPYRIGHT ***********END OF FILE*****************/



