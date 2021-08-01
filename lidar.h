/* 
 * File:   lidar.h
 * Author: 12591
 *
 * Created on July 22, 2021, 4:51 PM
 */

#ifndef LIDAR_H
#define	LIDAR_H

typedef union cmd_u {
    int command;
    char bytes[4];
} cmd_t;

void LIDAR_init();
void start_lidar();
void clear_result();
int* read_result();

#endif	/* LIDAR_H */

