/* 
 * File:   servo.h
 * Author: 12591
 *
 * Created on July 25, 2021, 3:19 PM
 */

#ifndef SERVO_H
#define	SERVO_H

void servo_init();
void change_pwm(int new_pwm);
int read_pwm();
int is_unfinished();

#endif	/* SERVO_H */

