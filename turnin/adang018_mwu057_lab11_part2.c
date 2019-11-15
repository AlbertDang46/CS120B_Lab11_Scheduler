/*	Author: Albert Dang adang018
 *      Partner(s) Name: Min-Hua Wu mwu057
 *	Lab Section: 022
 *	Assignment: Lab #11  Exercise #2
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <string.h>
#include "scheduler.h"
#include "timer.h"
#include "keypad.h"

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

const unsigned char numOpenChar = 16;
const char* message = "CS120B is Legend... wait for it DARy!";
char[numOpenChar*2 + strlen(message) + 1] new_message;
char[numOpenChar + 1] display_message;
unsigned char j;

enum lcd_States { lcd_start, lcd_init, lcd_display };

int lcdSMTick(int state) {
    switch(state) {
        case lcd_start: state = lcd_init; break;
        case lcd_init: state = lcd_display; break;
        case lcd_display: state = lcd_display: break;
        default: state = lcd_start; break;
    }
    switch(state) {
        case lcd_start: break;
        case lcd_init:
            j = 0;
            memset(new_message, ' ', numOpenChar);
            strcat(new_message, message);
            memset(new_message + numOpenChar + strlen(message), ' ', numOpenChar);
            new_message[numOpenChar*2 + strlen(message)] = '\0'; 
            break;
        case lcd_display;
            if(j <= numOpenChar + strlen(message) {
                memcpy(display_message, new_message + i, numOpenChar);
                new_message[numOpenChar] = '\0'; 
                j++;
            } else {
                j = 0;
            }       
            break;
        default: break;
    }
    
    LCD_DisplayString(1, display_message);  
}

int main(void) {
    DDRC = 0xFF; PORTC = 0x00;
    DDRD = 0xFF; PORTD = 0x00;

    static task task1;
    task *tasks[] = { &task1 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    task1.state = lcd_start;
    task1.period = 500;
    task1.elapsedTime = task1.period;
    task1.TickFct = &lcdSMTick;

    unsigned long GCD = tasks[0]->period;
    for(int i = 1; i < numTasks; i++) {
        GCD = findGCD(GCD, tasks[i]->period);
    }

    LCD_init();

    TimerSet(GCD);
    TimerOn();
    
    while (1) {
        for(unsigned short i = 0; i < numTasks; i++) {
            if(tasks[i]->elapsedTime == tasks[i]->period) {
                tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
                tasks[i]->elapsedTime = 0;
            }
            tasks[i]->elapsedTime += 10;
        }
        while(!TimerFlag);
        TimerFlag = 0;
    }
    return 0;
}
