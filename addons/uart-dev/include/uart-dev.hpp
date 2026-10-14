#ifndef __UART_DEV_HPP__
#define __UART_DEV_HPP__

#include "godot_cpp/core/object.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/core/print_string.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/variant/packed_byte_array.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"
#include <cstddef>
#include <cstdint>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

namespace godot {
    class UartDev : public godot::Node {
        GDCLASS(UartDev, Node);
        int uart_handler = -1;
        godot::String dev_port = "";
    public:
        auto open_port() -> int {
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

        auto set_port(godot::String port) -> void {
            this->dev_port = port;
        }

        auto get_port() -> godot::String {
            return this->dev_port;
        }

        auto write_data(godot::PackedByteArray bytes) {
            if (!this->avaliable()) {
                print_error("Port is not avaliable");
                return;
            }

            const auto* raw = static_cast<const uint8_t*>(bytes.ptr());
            size_t len = bytes.size();
            write(this->uart_handler, raw, len);
        }

        auto avaliable() -> bool {
            return this->uart_handler != -1;
        }

        static void _bind_methods() {
            godot::ClassDB::bind_method(godot::D_METHOD("set_port", "port"), &UartDev::set_port, godot::Variant::STRING);
            godot::ClassDB::bind_method(godot::D_METHOD("get_port"), &UartDev::get_port);
            godot::ClassDB::bind_method(godot::D_METHOD("write_data", "bytes"), &UartDev::write_data, godot::Variant::PACKED_BYTE_ARRAY);
            godot::ClassDB::bind_method(godot::D_METHOD("avaliable"), &UartDev::avaliable);

            ADD_PROPERTY(godot::PropertyInfo(godot::Variant::STRING, "dev_port"), "set_port", "get_port");

            ADD_SIGNAL(MethodInfo("on_port_recv", PropertyInfo(Variant::INT, "byte")));
            
        }
    };
}

#endif

