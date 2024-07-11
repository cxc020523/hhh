#ifndef _SERIAL_H_
#define _SERIAL_H_
#include <ros/ros.h>
#include <ros/time.h>

#include "std_msgs/String.h"//use data struct of std_msgs/String  
#include "std_msgs/Float32.h" 

#include<iostream>
#include<string>
#include<sstream>
#include <vector>

#define     HEAD_H              0xAA
#define     HEAD_R_H            0xAB
#define     HEAD_L              0x01
#define     HW_ADDR             0x30

#define     Board_Type          0x0101


#define     UART_GPIO_CONTROL   0x01000001
 
#endif
