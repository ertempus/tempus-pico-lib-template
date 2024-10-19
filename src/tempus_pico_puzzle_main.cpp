/**
 * Tempus Clue System Puzzle
 *
 * Copyright (c) 2023 Tempus ER LTD. All rights reserved.
 *
 */

#include <stdio.h>
#include <string.h>

#include "pico/stdlib.h"

#include "FreeRTOS.h"
#include "task.h"

#include "lwip/ip4_addr.h"

#ifdef RASPBERRYPI_PICO_W
#include "pico/cyw43_arch.h"
#endif

#include "tempus/tempus_lib.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

// Define this RoomScreens details
const char *room_slug = "template-room";                    // max length ROOM_STRING_SIZE
const char *component_name = "TemplateRoomPuzzle";          // max length NAME_STRING_SIZE
// optional define a PuzzleData store
const char *store_name = "template-room:puzzle";            // max length STORE_NAME_STRING_SIZE

// Standard Task priority
#define MAIN_TASK_STACK_SIZE (1024 * 2)
static const char *MAIN_TASK_NAME = "MainThread";
#define MAIN_TASK_PRIORITY ( tskIDLE_PRIORITY + 1UL )
static TaskHandle_t _main_task_handle;

enum puzzle_data_names {
    PUZZLE_DATA_,

    PUZZLE_DATA_STORE_SIZE, // keep as last entry for size
};
puzzle_data_t puzzle_data_store[PUZZLE_DATA_STORE_SIZE];
#define PUZZLE_DATA_STORE_INIT_DELAY (1000 * 5)

game_state_t _game_state = GAME_STATE_NULL;

typedef enum main_task_flag {
    MAIN_TASK_FLAG_,
} main_task_flag_t;

/*
 * MQTT Lib Hooks
 */

void mqtt_game_state_handler(game_state_t game_state) {
    printf("Main: Got game state: %s\n", game_state_strings[game_state]);
    _game_state = game_state;

    switch (_game_state) {
    case GAME_STATE_NULL:
        break;
    case GAME_STATE_READY:
        break;
    case GAME_STATE_START_BRIEF:
        break;
    case GAME_STATE_START:
        break;
    case GAME_STATE_FAILED:
        break;
    case GAME_STATE_ESCAPE:
        break;
    default:
        break;
    }
}

/*
 * ====================
 * End of MQTT Lib Hooks
 */

/*
 * Store
 */

void store_cb(const char *store_name, puzzle_data_t *puzzle_data) {
    printf("Store: store_cb: store_name: %s, key: %s\n", store_name, puzzle_data->key);
    printf("Store: store_cb: value: %s\n", puzzle_data->j.dump().c_str());
    for (int i = 0; i < PUZZLE_DATA_STORE_SIZE; ++i) {
        if (0 == strcmp(puzzle_data_store[i].key, puzzle_data->key)) {
            if (PUZZLE_DATA_TYPE_JSON == puzzle_data_store[i].type) {
                puzzle_data_store[i].j = puzzle_data->j;
            } else if (PUZZLE_DATA_TYPE_STRING == puzzle_data_store[i].type) {
                strcpy(puzzle_data_store[i].string, puzzle_data->j.template get<std::string>().c_str());
            } else if (PUZZLE_DATA_TYPE_INT == puzzle_data_store[i].type) {
                puzzle_data_store[i].value = puzzle_data->j;
            }

            puzzle_data_store[i].published = true;
            printf("Store: store_cb: pd: %d updated from mqtt\n", i);
        }
    }
}

void store_init() {
    printf("Store: Init\n");

    // strcpy(puzzle_data_store[{{ PUZZLE_DATA_  }}].key, "{{ }}");
    // puzzle_data_store[{{ PUZZLE_DATA_  }}].type = PUZZLE_DATA_TYPE_JSON;
    // puzzle_data_store[{{ PUZZLE_DATA_  }}].j = json::array();
    // puzzle_data_store[{{ PUZZLE_DATA_  }}].j_default = json::array();
}

void store_check_set_defaults(TickType_t xLastWakeTime) {
    static bool store_subscribed = false;
    static TickType_t store_subscribed_at = 0;
    static bool store_all_published = false;

    if (! tempus_is_mqtt_connected()) {
        return;
    }

    if (! store_subscribed) {
        store_subscribed = true;
        store_subscribed_at = xLastWakeTime;
        printf("Store: Subscribing\n");
        tempus_store_subscribe(store_name, store_cb);
        printf("Store: Subscribed\n");

        return;
    }

    if (store_all_published) {
        return;
    }

    if (xTaskGetTickCount() - store_subscribed_at < PUZZLE_DATA_STORE_INIT_DELAY) {
        return;
    }

    printf("Store: Publishing defaults\n");
    for (int i = 0; i < PUZZLE_DATA_STORE_SIZE; ++i) {
        if (puzzle_data_store[i].published) {
            continue;
        }

        tempus_store_set(store_name, &puzzle_data_store[i]);
        puzzle_data_store[i].published = true;
    }

    store_all_published = true;
    printf("Store: All defaults published\n");
}

/*
 * ====================
 * End of Store
 */

/*
 * Puzzle Custom Subscribes
 */

void puzzle_custom_check() {
    static bool puzzle_custom_subscribed = false;

    if (puzzle_custom_subscribed) {
        return;
    }

    if (! tempus_is_mqtt_connected()) {
        return;
    }

    // sprintf({{ }}, "%s%s", puzzle_custom_topic_prefix, "{{  }}");
    // tempus_puzzle_custom_subscribe({{  }}}, {{ }});
    puzzle_custom_subscribed = true;
}

/*
 * ====================
 * End of Puzzle Custom Subscribes
 */


void print_task_list(TickType_t xLastWakeTime) {
    static TickType_t xLastStatsPrint;

    if (xLastWakeTime - xLastStatsPrint > pdMS_TO_TICKS(1000 * 60)) {
        xLastStatsPrint = xLastWakeTime;
        uint16_t stats_buffer_length = 59 * uxTaskGetNumberOfTasks();
        char stats_buffer[stats_buffer_length];
        vTaskListTasks(stats_buffer, stats_buffer_length);
        printf("Task Stats\n");
        printf("Name            State\tPri\tSHWM\tNo.\tCore Affinity\n");
        printf(stats_buffer);
        printf("Free Heap Size: %d\n", xPortGetFreeHeapSize());
    }
}

void led_init() {
    printf("Main: LED Init\n");

    #ifndef RASPBERRYPI_PICO_W
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    #endif
}

void led_toggle() {
    #ifdef RASPBERRYPI_PICO_W
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, ! cyw43_arch_gpio_get(CYW43_WL_GPIO_LED_PIN));
    #else
    gpio_put(PICO_DEFAULT_LED_PIN, ! gpio_get(PICO_DEFAULT_LED_PIN));
    #endif
}

void main_task(void *params) {
    printf("Main task: start\n");
    TickType_t xLastWakeTime;
    TickType_t xLastWatchdog;
    TickType_t xLastStatsPrint;

    // store_init();

    tempus_setup(room_slug, component_name);
    // vTaskDelay(pdMS_TO_TICKS(500));

    led_init();

    printf("Main task: Entering loop\n");
    uint32_t ulNotifiedValue;
    while (true) {
        xTaskNotifyWait(
            0,
            ULONG_MAX,
            &ulNotifiedValue,
            (TickType_t) pdMS_TO_TICKS(100));
        xLastWakeTime = xTaskGetTickCount();
        if (xLastWakeTime - xLastWatchdog > pdMS_TO_TICKS(1000)) {
            xLastWatchdog = xLastWakeTime;
            #ifndef NDEBUG
            printf("Watchdog: Main\n");
            #endif
        }

        // store_check_set_defaults(xLastWakeTime);
        // puzzle_custom_check();

        #ifndef NDEBUG
        // print_task_list(xLastWakeTime);
        #endif

        led_toggle();
    }
}

void vLaunch() {
    xTaskCreate(main_task, MAIN_TASK_NAME, MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &_main_task_handle);

    // more tasks, they can set vTaskCoreAffinitySet if needed

    // Start the tasks and timer running.
    vTaskStartScheduler();
}

int main() {
    stdio_init_all();
    sleep_ms(2000);
    printf("GO\n");

    vLaunch();

    return 0;
}
