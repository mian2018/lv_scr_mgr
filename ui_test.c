#include "ui_test.h"
#include "lv_scr_mgr.h"
#include "lv_scr_mgr_port.h"

static uint32_t root_id = 0;
void lv_scr_mgr_test(void)
{
    lv_scr_mgr_init(NULL);

    lv_scr_mgr_switch(eScreenUITest1, true);
}



static void btn_set_root_cb(lv_event_t* e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        root_id = (uint32_t)lv_event_get_user_data(e);
        lv_scr_mgr_switch(root_id, true);
    }
}

static void btn_go_root_cb(lv_event_t* e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        lv_scr_mgr_pop_root(true);
    }
}

static void btn_pop_cb(lv_event_t* e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        lv_scr_mgr_popn((uint32_t)lv_event_get_user_data(e), true);
    }
}

static void btn_push_cb(lv_event_t* e)
{
    lv_event_code_t event_code = lv_event_get_code(e);
    lv_obj_t* obj = lv_event_get_target(e);
    if (event_code == LV_EVENT_CLICKED)
    {
        lv_scr_mgr_push((uint32_t)lv_event_get_user_data(e), true);
    }
}

static lv_obj_t* scr_create(const uint32_t id, void* param)
{
    lv_obj_t* scr = lv_obj_create(NULL);

    lv_obj_t* cont = lv_obj_create(scr);
    lv_obj_set_size(cont, lv_obj_get_width(scr), lv_obj_get_height(scr));
    lv_obj_center(cont);
    lv_obj_set_flex_flow(cont, LV_FLEX_FLOW_ROW_WRAP);

    lv_obj_t* lab = lv_label_create(cont);
    lv_obj_set_size(lab, 200, 50);
    lv_obj_align(lab, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text_fmt(lab, "current is page%d", id);

    lab = lv_label_create(cont);
    lv_obj_set_size(lab, 200, 50);
    lv_obj_align(lab, LV_ALIGN_CENTER, 0, 0);
    lv_label_set_text_fmt(lab, "root is page%d", root_id);

    lv_obj_t* btns[8];

    for (int i = 0; i < sizeof(btns) / sizeof(btns[0]); i++)
    {
        btns[i] = lv_btn_create(cont);
        lv_obj_set_size(btns[i], 100, 50);
        lv_obj_t* lab = lv_label_create(btns[i]);
        lv_obj_align(lab, LV_ALIGN_CENTER, 0, 0);
    }

    lv_label_set_text_fmt(lv_obj_get_child(btns[0], 0), "set root");
    lv_obj_add_event_cb(btns[0], btn_set_root_cb, LV_EVENT_CLICKED, (void*)id);

    lv_label_set_text_fmt(lv_obj_get_child(btns[1], 0), "go root");
    lv_obj_add_event_cb(btns[1], btn_go_root_cb, LV_EVENT_CLICKED, NULL);

    lv_label_set_text_fmt(lv_obj_get_child(btns[2], 0), "pop 1");
    lv_obj_add_event_cb(btns[2], btn_pop_cb, LV_EVENT_CLICKED, (void*)1);

    lv_label_set_text_fmt(lv_obj_get_child(btns[3], 0), "pop2");
    lv_obj_add_event_cb(btns[3], btn_pop_cb, LV_EVENT_CLICKED, (void*)2);

    lv_label_set_text_fmt(lv_obj_get_child(btns[4], 0), "pop3");
    lv_obj_add_event_cb(btns[4], btn_pop_cb, LV_EVENT_CLICKED, (void*)3);

    lv_label_set_text_fmt(lv_obj_get_child(btns[5], 0), "push page0");
    lv_obj_add_event_cb(btns[5], btn_push_cb, LV_EVENT_CLICKED, (void*)eScreenUITest1);

    lv_label_set_text_fmt(lv_obj_get_child(btns[6], 0), "push page1");
    lv_obj_add_event_cb(btns[6], btn_push_cb, LV_EVENT_CLICKED, (void*)eScreenUITest2);

    lv_label_set_text_fmt(lv_obj_get_child(btns[7], 0), "push page2");
    lv_obj_add_event_cb(btns[7], btn_push_cb, LV_EVENT_CLICKED, (void*)eScreenUITest3);


    return scr;
}


const lv_scr_mgr_handle_t s_scr_mgr_handle_test1 =
{
    .scr_id = eScreenUITest1,
    .scr_create = scr_create,
};
LV_SCR_MGR_REG(s_scr_mgr_handle_test1);

const lv_scr_mgr_handle_t s_scr_mgr_handle_test2 =
{
    .scr_id = eScreenUITest2,
    .scr_create = scr_create,
};
LV_SCR_MGR_REG(s_scr_mgr_handle_test2);

const lv_scr_mgr_handle_t s_scr_mgr_handle_test3 =
{
    .scr_id = eScreenUITest3,
    .scr_create = scr_create,
};
LV_SCR_MGR_REG(s_scr_mgr_handle_test3);
