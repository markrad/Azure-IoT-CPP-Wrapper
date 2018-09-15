cmake_minimum_required(VERSION 3.0.0)
cmake_policy(SET CMP0048 OLD)

project(AzureIoTCPPWrapper)

option(build_sample "Build sample executables" ON)

set(build_sample FALSE CACHE BOOL "Build the sample code" FORCE)
set(use_amqp FALSE CACHE BOOL "set use_amqp to ON if amqp is to be used, set to OFF to not use amqp" FORCE)
set(skip_samples ON CACHE BOOL "set skip_samples to ON to skip building samples (default is OFF)[if possible, they are always build]" FORCE)

include(CTest)
enable_testing()

add_subdirectory(azure-iot-sdk-c)

set(headers 
    "CIoTHubDevice.h"
    "CIoTHubMessage.h"
    "CMapUtil.h"
)
set(source 
    "CIoTHubDevice.cpp"
    "CIoTHubMessage.cpp"
    "CMapUtil.cpp"
)

add_library(AzureIoTDevice ${source} ${headers})

target_include_directories(AzureIoTDevice PRIVATE 
	"./azure-iot-sdk-c/deps" 
	"./azure-iot-sdk-c/iothub_client/inc" 
	"./azure-iot-sdk-c/c-utility/inc"
)

target_link_libraries(AzureIoTDevice 
    parson
    iothub_client
)

if (build_sample)
	add_executable(AzureIotDeviceSample "main.cpp" ${headers})
	target_link_libraries(AzureIotDeviceSample AzureIoTDevice)
	target_include_directories(AzureIotDeviceSample PRIVATE 
		"./azure-iot-sdk-c/deps" 
		"./azure-iot-sdk-c/iothub_client/inc" 
		"./azure-iot-sdk-c/c-utility/inc"
	)
endif(build_sample)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
