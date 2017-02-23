cmake_minimum_required(VERSION 3.0)

project(usb-pwm)

if(UNIX)
	include_directories("/usr/include/libusb-1.0")
endif()

add_executable(${PROJECT_NAME}
	src/main.cpp
	src/USBDevice.cpp src/USBDevice.h
	src/PWMDevice.cpp src/PWMDevice.h
)
target_link_libraries(${PROJECT_NAME} usb-1.0)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 11)
