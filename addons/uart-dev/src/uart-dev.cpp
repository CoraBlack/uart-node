#include "uart-dev.hpp"
#include <array>
#include <bits/types/struct_timeval.h>
#include <cerrno>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <sys/select.h>
#include <unistd.h>

namespace godot {
    auto UartDev::_notification(int32_t p_what) -> void {
        if (p_what == NOTIFICATION_INTERNAL_PROCESS && this->avaliable()) {
            fd_set readset;
            struct timeval timeout;
            timeout.tv_sec = 1;

            FD_ZERO(&readset);
            FD_SET(this->uart_handler, &readset);

            errno = 0;
            int ret = select(this->uart_handler + 1,
                &readset,
                NULL,
                NULL, 
                &timeout
            );

            if (ret == -1) {
                if (errno == EINTR) return;
                godot::print_error(strerror(errno));
                return;
            } else if (ret == 0) {
                return;
            }

            if (FD_ISSET(this->uart_handler, &readset)) {
                std::array<uint8_t, 128> byte_buf;
                int n = read(this->uart_handler, &byte_buf, byte_buf.size());
                if (n > 0) {
                    for (const uint8_t b : byte_buf) {
                        emit_signal("on_port_recv", b);
                    }
                }
            }
        }
    }

    auto UartDev::open_port() -> int {
        int fd = open(dev_port.ascii().ptr(), O_RDWR|O_NOCTTY| O_NDELAY);
        if (fd != -1) {
            struct termios opt;
            cfsetispeed(&opt, B19200);
            cfsetospeed(&opt, B19200);

            opt.c_cflag |= (CLOCAL | CREAD);
            opt.c_cflag &= ~CRTSCTS;
            opt.c_cflag &= ~CSTOPB;
            opt.c_cflag &= ~PARENB;
            opt.c_cflag &= ~CSIZE;
            opt.c_cflag |= CS8;

            opt.c_cc[VMIN]  = 0;
            opt.c_cc[VTIME] = 10;
            
            if (tcsetattr(fd, TCSANOW, &opt) != 0) {
                close(fd);
                return -1;
            }
            
            int flags = fcntl(fd, F_GETFL, 0);
            fcntl(fd, F_SETFL, flags & ~O_NONBLOCK);
            this->uart_handler = fd;
        } else {
            godot::print_line("Port " + dev_port + " can't open");
        }
        return fd;
    }

    auto UartDev::set_port(godot::String port) -> void {
        this->dev_port = port;
        this->open_port();
    }

    auto UartDev::get_port() -> godot::String {
        return this->dev_port;
    }
}
