#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdlib.h>

void main(void) {
    printk("--- Pokretanje Zephyr IoT Demo Aplikacije ---\n");

    while (1) {
        // Simulacija očitavanja senzora temperature (nasumično između 20 i 35 stepeni)
        int temp = 20 + (rand() % 16);
        
        printk("Ocitavanje senzora: Temperatura je %d C. ", temp);

        // Aktuacija na osnovu uslova
        if (temp >= 30) {
            printk(" -> UPOZORENJE! AKTUACIJA: Ukljucujem ventilator (LED ON)!\n");
        } else {
            printk(" -> AKTUACIJA: Gasim ventilator (LED OFF).\n");
        }

        // Pauza od 2 sekunde (slično vTaskDelay iz tvojih slajdova)
        k_msleep(2000);
    }
}