#include "base/lidig_logger.h"
#include "network/lidig_uart.h"

lidig_uart::lidig_uart() {
    LogTrace();
}

lidig_uart::~lidig_uart() {
    LogTrace();
}

int lidig_uart::open(const std::string& dev_name, uint32_t baud_rate) {
    LogInfo() << dev_name << " at " << baud_rate << " bauds rate (8,1,N)";
    int ret = lidig_pipe::open(dev_name);
    if (ret < 0) {
        LogError() << dev_name << " failed!";
        return ret;
    }

    set_parity(8, 1, 'n');
    set_speed(baud_rate);
    async_read_start();
    return ret;
}

static int speed_arr[] = {B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300};
static uint32_t name_arr[] = {115200, 57600, 38400, 19200, 9600, 4800, 2400, 1200, 300};

bool lidig_uart::set_speed(uint32_t speed)
{
    uint32_t i;
    int status;
    struct termios Opt;
    tcgetattr(get_fd(), &Opt);
    for (i = 0;  i < sizeof(speed_arr)/sizeof(int); i++) {
        if (speed == name_arr[i]) {
            tcflush(get_fd(), TCIOFLUSH);
            cfsetispeed(&Opt, speed_arr[i]);
            cfsetospeed(&Opt, speed_arr[i]);
            status = tcsetattr(get_fd(), TCSANOW, &Opt);
            if (status != 0)
                return false;

            tcflush(get_fd(), TCIOFLUSH);
            return true;
        }
    }

    return false;
}

bool lidig_uart::set_parity(uint32_t databits, uint8_t stopbits, uint8_t parity)
{
    struct termios options;
    if (tcgetattr( get_fd(),&options) != 0)
        return false;

    options.c_cflag &= ~CSIZE;
    switch (databits) {
        case 7:
            options.c_cflag |= CS7;
            break;
        case 8:
            options.c_cflag |= CS8;
            break;
        default:
            return false;
    }

    switch (parity) {
        case 'n':
        case 'N':
            options.c_cflag &= ~PARENB;
            options.c_iflag &= ~INPCK;
            break;
        case 'o':
        case 'O':
            options.c_cflag |= (PARODD | PARENB);
            options.c_iflag |= INPCK;
            break;
        case 'e':
        case 'E':
            options.c_cflag |= PARENB;
            options.c_cflag &= ~PARODD;
            options.c_iflag |= INPCK;
            break;
        case 'S':
        case 's':
            options.c_cflag &= ~PARENB;
            options.c_cflag &= ~CSTOPB;
            break;
        default:
            return false;
    }

    switch (stopbits) {
        case 1:
            options.c_cflag &= ~CSTOPB;
            break;
        case 2:
            options.c_cflag |= CSTOPB;
           break;
        default:
            return false;
    }

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG | ECHONL);
    options.c_oflag &= ~OPOST;
    options.c_iflag &= ~(ICRNL | IXON);
    options.c_cc[VTIME] = 0;
    options.c_cc[VMIN] = 1;
    options.c_iflag &=~(INLCR|IGNCR|ICRNL);

    tcflush(get_fd(), TCIFLUSH);

    if (tcsetattr(get_fd(), TCSANOW, &options) != 0)
        return false;

    return true;
}