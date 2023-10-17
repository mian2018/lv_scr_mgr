# lv_scr_mgr
## lvgl 页面管理器
适配 lvgl 8.3
* 降低界面之间的耦合
* 使用较小的内存，界面切换后会自动释放内存
* 内存泄漏检测

## 使用方法
1. 在lv_scr_mgr_port.h 中创建一个枚举，用于界面ID
2. 为每个界面创建一个页面管理器句柄
3. 将界面句柄添加到 lv_scr_mgr_port.c 数组中
4. 在 lv_init() 后，对页面管理器进行初始化 lv_scr_mgr_init(NULL);
5. 使用 lv_scr_mgr_switch 设置初始根界面
6. 使用 lv_scr_mgr_push  lv_scr_mgr_pop 对界面进行操作
