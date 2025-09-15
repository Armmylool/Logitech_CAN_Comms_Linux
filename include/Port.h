#define _DEFAULT_SOURCE
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>

extern char writeBuffer[128] ;
extern int fd ;

/**
 * @brief เปิดและตั้งค่าพอร์ตอนุกรม
 * @param port_name ชื่อพอร์ต เช่น "/dev/ttyUSB0"
 * @param baud_rate ความเร็ว เช่น 9600, 115200
 * @return File descriptor ถ้าสำเร็จ, -1 ถ้าล้มเหลว
 */
int serial_port_init(const char* port_name, int baud_rate);

/**
 * @brief เขียนข้อมูลไปยังพอร์ตอนุกรม
 * @param fd File descriptor ของพอร์ต
 * @param data ข้อความที่ต้องการส่ง
 * @return จำนวน bytes ที่เขียนสำเร็จ, -1 ถ้าล้มเหลว
 */
int serial_port_write(int fd, const char* data);

/**
 * @brief อ่านข้อมูลจากพอร์ตอนุกรม
 * @param fd File descriptor ของพอร์ต
 * @param buffer ตัวแปรสำหรับเก็บข้อมูลที่อ่านได้
 * @param size ขนาดของ buffer
 * @return จำนวน bytes ที่อ่านได้, -1 ถ้าล้มเหลว
 */
int serial_port_read(int fd, char* buffer, size_t size);

/**
 * @brief ปิดพอร์ตอนุกรม
 * @param fd File descriptor ของพอร์ต
 */
void serial_port_close(int fd);

/** @brief  CAN Setting and Start
 *  @param bitrate  bitrate of CAN device or BUS
 * @warning Serial_Begin must be begin first
*/
bool CAN_Begin(int fd, int bitrate) ;

/** @brief CAN write Data 
 *  @param addr the Extended Address
 *  @param data 8 byte data
*/
bool CAN_Write(int fd, const char *addr, const char *data) ;

/** @brief Write integer Data to CAN (For change a msg for acceleration.) */
bool CAN_Write_integer(int fd, const char* addr, uint8_t* packet_buffer, const char* header) ;
