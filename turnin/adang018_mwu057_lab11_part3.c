/*	Author: Albert Dang adang018
 *      Partner(s) Name: Min-Hua Wu mwu057
 *	Lab Section: 022
 *	Assignment: Lab #11  Exercise #3
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

unsigned char curChar = ' ';

enum keypad_States { keypad_start, keypad_press };

int keypad_SMTick(int state) {
    switch(state) {
        case keypad_start: state = keypad_press; break;
        case keypad_press: state = keypad_press; break;
        default: state = keypad_start; break;
    }
    switch(state) {
        case keypad_start: break;
        case keypad_press:
            switch(GetKeypadKey()) {
                case '\0': break;
                case '1': curChar = '1'; break;
                case '2': curChar = '2'; break;
                case '3': curChar = '3'; break;
                case '4': curChar = '4'; break;
                case '5': curChar = '5'; break;
                case '6': curChar = '6'; break;
                case '7': curChar = '7'; break;
                case '8': curChar = '8'; break;
                case '9': curChar = '9'; break;
                case 'A': curChar = 'A'; break;
                case 'B': curChar = 'B'; break;
                case 'C': curChar = 'C'; break;
                case 'D': curChar = 'D'; break;
                case '*': curChar = '*'; break;
                case '0': curChar = '0'; break;
                case '#': curChar = '#'; break;
                default: curChar = ' '; break;
            }
            break;
        default: break;
    }
}

enum lcd_States { lcd_start, lcd_display };

int lcdSMTick(int state) {
    switch(state) {
        case lcd_start: state = lcd_display; break;
        case lcd_display: state = lcd_display: break;
        default: state = lcd_start; break;
    }
    switch(state) {
        case lcd_start: break;
        case lcd_display;
            LCD_Cursor(1);
            LCD_WriteData(curChar);
            break;
        default: break;
    }
}

int main(void) {
    DDRA = 0xFF; PORTA = 0x00;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xF0; PORTC = 0x0F;

    static task task1, task2;
    task *tasks[] = { &task1 &task2 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    task1.state = keypad_start;
    task1.period = 1;
    task1.elapsedTime = task1.period;
    task1.TickFct = &keypadSMTick;

    task2.state = lcd_start;
    task2.period = 1;
    task2.elapsedTime = task2.period;
    task2.TickFct = &lcdSMTick;

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
