#include "actions.h"
#include "screens.h"
#include "ui.h"
 
 int16_t currentScreen = -1;
 lv_obj_t *getScreenObj(enum ScreensEnum screenId) {
    return ((lv_obj_t **)&objects)[screenId - 1];
}
 
 void on_screen_unloaded(lv_event_t *e) {
    if (lv_event_get_code(e) == LV_EVENT_SCREEN_UNLOADED) {
        enum ScreensEnum screenId =
            (enum ScreensEnum)(lv_uintptr_t)lv_event_get_user_data(e);
        delete_screen_by_id(screenId);
    }
}
 
 void deleteScreenOnUnload(enum ScreensEnum screenId) {
    lv_obj_add_event_cb(
        getScreenObj(screenId),
        on_screen_unloaded,
        LV_EVENT_SCREEN_UNLOADED,
        (void*)(lv_uintptr_t)(screenId)
    );    
}
 void changeToScreen(enum ScreensEnum screenId) {
    if (!getScreenObj(screenId)) {
        create_screen_by_id(screenId);
        if (!getScreenObj(screenId)) {
            return;
        }
        deleteScreenOnUnload(screenId);
    }
 
    loadScreen(screenId);
}
void action_change_to_config(lv_event_t *e) {
    // TODO: Implement action return_to_home here
    
    changeToScreen(SCREEN_ID_CONFIGURATION);
}
/*void action_go_to_data(lv_event_t *e) {
    // TODO: Implement action go_to_data here
    changeToScreen(SCREEN_ID_DATA);
}*/