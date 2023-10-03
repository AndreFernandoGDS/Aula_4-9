#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"


QueueHandle_t interruptQueue;

#define PIN 4
int counter;

static void IRAM_ATTR gpio4_isr_handler(void *params){ //IRAM_ATTR é uma diretiva de atributo que é usada para indicar que a função deve ser colocada na memória IRAM, que é mais rápida e apropriada para funções de interrupção.
    int pinNumber = (int)params;
    xQueueSendFromISR(interruptQueue, &pinNumber, NULL);
}

void signalTriggered(void *params){
    int pinNumber;

    while(1){
        if (xQueueReceive(interruptQueue, &pinNumber, portMAX_DELAY)){
            counter++;
            printf("Contador: %d \n", counter);
        }
    }
}

void app_main()
{
   esp_rom_gpio_pad_select_gpio(PIN);
   gpio_set_direction(PIN, GPIO_MODE_INPUT);
   gpio_pulldown_dis(PIN);
   gpio_pullup_dis(PIN);
   gpio_set_intr_type(PIN, GPIO_INTR_POSEDGE);


   interruptQueue = xQueueCreate(10, sizeof(int));
   xTaskCreate(signalTriggered, "signalTriggered", 2048, NULL, 1, NULL);

   gpio_install_isr_service(0);     // Instalar o serviço de interrupção GPIO e configurar a função de tratamento
   gpio_isr_handler_add(PIN, gpio4_isr_handler, (void *)PIN);
}
