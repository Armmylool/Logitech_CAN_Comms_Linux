#include <Port.h>

#define CAN_RATE_500K 500000
#define CAN_RATE_1M   1000000

int serial_port_init(const char* port_name, int baud_rate) {
    int fd = open(port_name, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        fprintf(stderr, "Error opening %s: %s\n", port_name, strerror(errno));
        return -1;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        fprintf(stderr, "Error from tcgetattr: %s\n", strerror(errno));
        close(fd);
        return -1;
    }
    
    // ตั้งค่า Baud Rate
    speed_t speed;
    switch(baud_rate) {
        case 9600:   speed = B9600;   break;
        case 19200:  speed = B19200;  break;
        case 57600:  speed = B57600;  break;
        case 115200: speed = B115200; break;
        default:
            fprintf(stderr, "Unsupported baud rate %d\n", baud_rate);
            close(fd);
            return -1;
    }
    cfsetospeed(&tty, speed);
    cfsetispeed(&tty, speed);

    // ตั้งค่า 8N1 (8 data bits, no parity, 1 stop bit) และอื่นๆ
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8; // 8-bit chars
    tty.c_cflag &= ~(PARENB | CSTOPB); // No parity, 1 stop bit
    tty.c_cflag &= ~CRTSCTS; // No hardware flow control
    tty.c_cflag |= CREAD | CLOCAL; // Turn on READ & ignore ctrl lines

    tty.c_lflag &= ~ICANON;
    tty.c_lflag &= ~ECHO; // Disable echo
    tty.c_lflag &= ~ECHOE; // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable new-line echo
    tty.c_lflag &= ~ISIG; // Disable interpretation of INTR, QUIT and SUSP

    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off s/w flow ctrl
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL);

    tty.c_oflag &= ~OPOST; // Prevent special interpretation of output bytes
    tty.c_oflag &= ~ONLCR; // Prevent conversion of newline to carriage return/line feed

    tty.c_cc[VTIME] = 0; // Wait for up to 1s (10 deciseconds)
    tty.c_cc[VMIN] = 0;   // Returning as soon as any data is received

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        fprintf(stderr, "Error from tcsetattr: %s\n", strerror(errno));
        close(fd);
        return -1;
    }

    return fd;
}

int serial_port_write(int fd, const char* data) {
    int len = strlen(data);
    int n = write(fd, data, len);
    if (n < 0) {
        fprintf(stderr, "Error writing to serial port: %s\n", strerror(errno));
    }
    return n;
}

int serial_port_read(int fd, char* buffer, size_t size) {
    int n = read(fd, buffer, size);
    if (n < 0) {
        fprintf(stderr, "Error reading from serial port: %s\n", strerror(errno));
    }
    return n;
}

void serial_port_close(int fd) {
    close(fd);
}

bool CAN_Begin(int fd, int bitrate) {
     switch (bitrate) {
        case CAN_RATE_500K :
            printf("CAN 500k bps beginned\n") ;
            sprintf(writeBuffer, "S6\r"); // Set bitrate to 500k
            break ;
        case CAN_RATE_1M :
            printf("CAN 1M bps beginned\n") ;
             sprintf(writeBuffer, "S8\r"); // Set bitrate to 1M
            break;
        default :
             printf("Error: Unsupported bitrate %d\n", bitrate);
            return false ;
    }
    if (!serial_port_write(fd, writeBuffer)){
	return false ;
    }
    sprintf(writeBuffer, "O\r");
    printf("\n%s\n", writeBuffer) ;
    if (!serial_port_write(fd, writeBuffer)){
        return false ;
    }
    printf("CAN channel opened successfully!\n");
    return true ;
}

bool CAN_Write(int fd, const char *addr, const char *data){
    sprintf(writeBuffer, "T%s8%s\r", addr, data) ;
    if (!serial_port_write(fd, writeBuffer)){
        printf("Error Write Fail") ;
        return false ;
    }
    else {
        //printf(writeBuffer) ;
        //printf("CAN writed successfully!\n") ;
        return true ;
    }
}

bool CAN_Write_integer(int fd, const char* addr, uint8_t* packet_buffer, const char* header) {
    char data_as_string[9];
    for (int i = 0; i < 4; i++) {
        sprintf(&data_as_string[i * 2], "%02X", packet_buffer[i]);
    }
     char completeMsg[strlen(header) + 9] ;
    strcpy(completeMsg, header); // คัดลอก header ("230D2003") มาใส่ก่อน
    strcat(completeMsg, data_as_string); // นำ data ("B9B0FFFF") มาต่อท้าย 
    return CAN_Write(fd, addr, completeMsg) ;
}
