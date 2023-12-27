/**
 * Pico w5x00 driver test
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
static const char *MAIN_TASK_NAME = "TempusThread";
#define MAIN_TASK_PRIORITY ( tskIDLE_PRIORITY + 1UL )

void led_init() {
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
}

void led_toggle() {
    gpio_put(PICO_DEFAULT_LED_PIN, ! gpio_get(PICO_DEFAULT_LED_PIN));
}

void main_task(void *params)
{
    printf("Main task: start\n");
    tempus_setup(room_slug, component_name);
    while (! tempus_is_mqtt_connected()) {
        vTaskDelay(500);
    }

    led_init();

    printf("Main task: Entering loop\n");
    while (true) {
        led_toggle();
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void vLaunch()
{
    TaskHandle_t task;

    xTaskCreate(main_task, MAIN_TASK_NAME, MAIN_TASK_STACK_SIZE, NULL, MAIN_TASK_PRIORITY, &task);

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
