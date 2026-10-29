#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>

int tcgetattr(int fd, struct termios* termios_p) {
    return ioctl(fd, TCGETS, termios_p);
}

int tcsetattr(int fd, int optional_actions, const struct termios* termios_p) {
    switch (optional_actions) {
    case TCSANOW: return ioctl(fd, TCSETS, termios_p);
    case TCSADRAIN: return ioctl(fd, TCSETSW, termios_p);
    case TCSAFLUSH: return ioctl(fd, TCSETSF, termios_p);
    default: return -1;
    }
}

int tcsendbreak(int fd, int duration) {
    return ioctl(fd, TCSBRK, duration);
}

int tcdrain(int fd) {
    return ioctl(fd, TCSBRK, 1);
}

int tcflush(int fd, int queue_selector) {
    return ioctl(fd, TCFLSH, queue_selector);
}

int tcflow(int fd, int action) {
    return ioctl(fd, TCXONC, action);
}

speed_t cfgetispeed(const struct termios* termios_p) {
    return termios_p->c_cflag & CBAUD;
}

speed_t cfgetospeed(const struct termios* termios_p) {
    return termios_p->c_cflag & CBAUD;
}

int cfsetispeed(struct termios* termios_p, speed_t speed) {
    termios_p->c_cflag = (termios_p->c_cflag & ~CBAUD) | (speed & CBAUD);
    return 0;
}

int cfsetospeed(struct termios* termios_p, speed_t speed) {
    termios_p->c_cflag = (termios_p->c_cflag & ~CBAUD) | (speed & CBAUD);
    return 0;
}

void cfmakeraw(struct termios* termios_p) {
    termios_p->c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP |
        INLCR | IGNCR | ICRNL | IXON);
    termios_p->c_oflag &= ~OPOST;
    termios_p->c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    termios_p->c_cflag &= ~(CSIZE | PARENB);
    termios_p->c_cflag |= CS8;
    termios_p->c_cc[VMIN] = 1;
    termios_p->c_cc[VTIME] = 0;
}
