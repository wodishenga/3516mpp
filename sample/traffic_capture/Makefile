# Hisilicon Hi35xx sample Makefile

include ../Makefile.param
#链接opencv库
CFLAGS += `pkg-config --cflags  --libs opencv`
CC = arm-himix200-linux-gcc

SMP_SRCS := $(wildcard *.c)
TARGET := $(SMP_SRCS:%.c=%)

TARGET_PATH := $(PWD)

# compile linux or HuaweiLite
include $(PWD)/../$(ARM_ARCH)_$(OSTYPE).mak
