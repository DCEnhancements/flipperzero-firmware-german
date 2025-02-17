#include "../quenon_dht_mon.h"

//Текущий вид
static View* view;
//Список
static VariableItemList* variable_item_list;

/* ================== Подтверждение удаления ================== */
/**
 * @brief Функция обработки нажатия кнопки "Назад"
 * 
 * @param context Указатель на данные приложения
 * @return ID вида в который нужно переключиться
 */
static uint32_t delete_exitCallback(void* context) {
    PluginData* app = context;
    UNUSED(app);
    //Возвращаем ID вида, в который нужно вернуться
    return SENSOR_ACTIONS_VIEW;
}
/**
 * @brief Обработчик нажатий на кнопку в виджете
 * 
 * @param result Какая из кнопок была нажата
 * @param type Тип нажатия
 * @param context Указатель на данные плагина
 */
static void widgetCallback(GuiButtonType result, InputType type, void* context) {
    PluginData* app = context;
    //Коротко нажата левая кнопка (Cancel)
    if(result == GuiButtonTypeLeft && type == InputTypeShort) {
        view_dispatcher_switch_to_view(app->view_dispatcher, SENSOR_ACTIONS_VIEW);
    }
    //Коротко нажата правая кнопка (Delete)
    if(result == GuiButtonTypeRight && type == InputTypeShort) {
        //Удаление датчика
        DHTMon_sensor_delete(app->currentSensorEdit);
        //Выход из меню
        view_dispatcher_switch_to_view(app->view_dispatcher, VIEW_NONE);
    }
}
/**
 * @brief Создание виджета удаления датчика
 * 
 * @param app Указатель на данные плагина
 */
static void sensorDeleteWidget(PluginData* app) {
    //Очистка виджета
    widget_reset(app->widget);
    //Добавление кнопок
    widget_add_button_element(app->widget, GuiButtonTypeLeft, "Cancel", widgetCallback, app);
    widget_add_button_element(app->widget, GuiButtonTypeRight, "Delete", widgetCallback, app);

    char delete_str[32];
    snprintf(delete_str, sizeof(delete_str), "\e#Delete %s?\e#", app->currentSensorEdit->name);
    widget_add_text_box_element(
        app->widget, 0, 0, 128, 23, AlignCenter, AlignCenter, delete_str, false);
    snprintf(
        delete_str,
        sizeof(delete_str),
        "\e#Type:\e# %s",
        app->currentSensorEdit->type ? "DHT22" : "DHT11");
    widget_add_text_box_element(
        app->widget, 0, 0, 128, 47, AlignLeft, AlignCenter, delete_str, false);
    snprintf(
        delete_str,
        sizeof(delete_str),
        "\e#GPIO:\e# %d",
        DHTMon_GPIO_to_int(app->currentSensorEdit->GPIO));
    widget_add_text_box_element(
        app->widget, 0, 0, 128, 72, AlignLeft, AlignCenter, delete_str, false);
    view_set_previous_callback(widget_get_view(app->widget), delete_exitCallback);
    view_dispatcher_switch_to_view(app->view_dispatcher, WIDGET_VIEW);
}

/* ================== Меню действий ================== */
/**
 * @brief Функция обработки нажатия средней кнопки
 * 
 * @param context Указатель на данные приложения
 * @param index На каком элементе списка была нажата кнопка
 */
static void enterCallback(void* context, uint32_t index) {
    PluginData* app = context;
    if(index == 0) {
        sensorEdit_scene(app);
    }
    if(index == 1) {
        sensorDeleteWidget(app);
    }
}

/**
 * @brief Функция обработки нажатия кнопки "Назад"
 * 
 * @param context Указатель на данные приложения
 * @return ID вида в который нужно переключиться
 */
static uint32_t actions_exitCallback(void* context) {
    PluginData* app = context;
    UNUSED(app);
    //Возвращаем ID вида, в который нужно вернуться
    return MAIN_MENU_VIEW;
}

/**
 * @brief Создание списка действий с указанным датчиком
 * 
 * @param app Указатель на данные плагина
 */
void sensorActions_sceneCreate(PluginData* app) {
    variable_item_list = variable_item_list_alloc();
    //Сброс всех элементов меню
    variable_item_list_reset(variable_item_list);
    //Добавление элементов в список
    variable_item_list_add(variable_item_list, "Edit", 0, NULL, NULL);
    variable_item_list_add(variable_item_list, "Delete", 0, NULL, NULL);

    //Добавление колбека на нажатие средней кнопки
    variable_item_list_set_enter_callback(variable_item_list, enterCallback, app);

    //Создание вида из списка
    view = variable_item_list_get_view(variable_item_list);
    //Добавление колбека на нажатие кнопки "Назад"
    view_set_previous_callback(view, actions_exitCallback);
    //Добавление вида в диспетчер
    view_dispatcher_add_view(app->view_dispatcher, SENSOR_ACTIONS_VIEW, view);
}
void sensorActions_scene(PluginData* app) {
    //Переключение на наш вид
    view_dispatcher_switch_to_view(app->view_dispatcher, SENSOR_ACTIONS_VIEW);
}

void sensorActions_screneRemove(void) {
    variable_item_list_free(variable_item_list);
}
