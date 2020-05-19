cmake_minimum_required(VERSION 3.4.1)

add_library(
        # Sets the name of the library.
        serial-port-lib
        # Sets the library as a shared library.
        SHARED
        # Provides a relative path to your source file(s).
        SerialPort.c)

target_link_libraries( # Specifies the target library.
        serial-port-lib)