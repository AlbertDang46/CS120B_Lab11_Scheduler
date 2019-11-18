/*	Author: Albert Dang adang018
 *      Partner(s) Name: Min-Hua Wu mwu057
 *	Lab Section: 022
 *	Assignment: Lab #11  Exercise #1
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include "scheduler.h"
#include "timer.h"
#include "keypad.h"

#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

unsigned char led0_output = 0x00;
unsigned char led1_output = 0x00;
unsigned char pause = 0;

enum pauseButtonSM_States { pauseButton_start, pauseButton_wait, pauseButton_press, pauseButton_release };

int pauseButtonSMTick(int state) {
    unsigned char press = ~PINA & 0x01;

    switch(state) {
        case pauseButton_start:
            state = pauseButton_wait; break;
        case pauseButton_wait:
            state = press == 0x01 ? pauseButton_press : pauseButton_wait; break;
        case pauseButton_press:
            state = pauseButton_release; break;
        case pauseButton_release:
            state = press == 0x00 ? pauseButton_wait : pauseButton_press; break;
        default: state = pauseButton_start; break;       
    }
    switch(state) {
        case pauseButton_start: break;
        case pauseButton_wait: break;
        case pauseButton_press:
            pause = (pause == 0) ? 1 : 0;
            break;
        case pauseButton_release: break;
        default: break;
    }
    return state;
}

enum toggleLED0_States { toggleLED0_start, toggleLED0_wait, toggleLED0_blink };

int toggleLED0SMTick(int state) {
    switch(state) {
        case toggleLED0_start: state = toggleLED0_wait; break;
        case toggleLED0_wait: state = !pause ? toggleLED0_blink : toggleLED0_wait; break;
        case toggleLED0_blink: state = pause ? toggleLED0_wait : toggleLED0_blink; break;
        default: state = toggleLED0_start; break;
    }
    switch(state) {
        case toggleLED0_start: break;
        case toggleLED0_wait: break;
        case toggleLED0_blink:
            led0_output = (led0_output == 0x00) ? 0x01 : 0x00;
            break;
        default: break;
    }
    return state;
}

enum toggleLED1_States { toggleLED1_start, toggleLED1_wait, toggleLED1_blink };

int toggleLED1SMTick(int state) {
    switch(state) {
        case toggleLED1_start: state = toggleLED1_wait; break;
        case toggleLED1_wait: state = !pause ? toggleLED1_blink : toggleLED1_wait; break;
        case toggleLED1_blink: state = pause ? toggleLED1_wait : toggleLED1_blink; break;
        default: state = toggleLED1_start; break;
    }
    switch(state) {
        case toggleLED1_start: break;
        case toggleLED1_wait: break;
        case toggleLED1_blink:
            led1_output = (led1_output == 0x00) ? 0x01 : 0x00;
            break;
        default: break;
    }
    return state;
}

enum display_States { display_start, display_display };

int displaySMTick(int state) {
    unsigned char output = 0x00;
 
    switch(state) {
        case display_start: state = display_display; break;
        case display_display: state = display_display; break;
        default: state = display_start; break;
    }
    switch(state) {
        case display_start: break;
        case display_display:
            output = led0_output | led1_output << 1;
            break;
        default: break;
    }
    PORTB = output;
    return state;
}

enum keypad_States { keypad_start, keypad_press };

int keypadSMTick(int state) {
    unsigned char output = 0x1F;

    switch(state) {
        case keypad_start: state = keypad_press; break;
        case keypad_press: state = keypad_press; break;
        default: state = keypad_start; break;
    }
    switch(state) {
        case keypad_start: break;
        case keypad_press:
            switch(GetKeypadKey()) {
                case '\0': output = 0x1F; break;
                case '1': output = 0x01; break;
                case '2': output = 0x02; break;
                case '3': output = 0x03; break;
                case '4': output = 0x04; break;
                case '5': output = 0x05; break;
                case '6': output = 0x06; break;
                case '7': output = 0x07; break;
                case '8': output = 0x08; break;
                case '9': output = 0x09; break;
                case 'A': output = 0x0A; break;
                case 'B': output = 0x0B; break;
                case 'C': output = 0x0C; break;
                case 'D': output = 0x0D; break;
                case '*': output = 0x0E; break;
                case '0': output = 0x00; break;
                case '#': output = 0x0F; break;
                default: output = 0x1B; break;
            }
            break;
        default: break;
    }
    PORTD = output; 
    return state;  
}

int main(void) {
    DDRA = 0x00; PORTA = 0xFF;
    DDRB = 0xFF; PORTB = 0x00;
    DDRC = 0xF0; PORTC = 0x0F;
    DDRD = 0xFF; PORTD = 0x00;

    static task task1, task2, task3, task4, task5;
    task *tasks[] = { &task1, &task2, &task3, &task4, &task5 };
    const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

    task1.state = pauseButton_start;
    task1.period = 50;
    task1.elapsedTime = task1.period;
    task1.TickFct = &pauseButtonSMTick;

    task2.state = toggleLED0_start;
    task2.period = 500;
    task2.elapsedTime = task2.period;
    task2.TickFct = &toggleLED0SMTick;

    task3.state = toggleLED1_start;
    task3.period = 1000;
    task3.elapsedTime = task3.period;
    task3.TickFct = &toggleLED1SMTick;

    task4.state = display_start;
    task4.period = 10;
    task4.elapsedTime = task4.period;
    task4.TickFct = &displaySMTick;

    task5.state = keypad_start;
    task5.period = 1;
    task5.elapsedTime = task5.period;
    task5.TickFct = &keypadSMTick;

    unsigned long GCD = tasks[0]->period;
    unsigned short j;
    for(j = 1; j < numTasks; j++) {
        GCD = findGCD(GCD, tasks[j]->period);
    }

    TimerSet(GCD);
    TimerOn();

    unsigned short i;

    while (1) {
        for(i = 0; i < numTasks; i++) {
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
