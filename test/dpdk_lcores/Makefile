
RTE_SDK = /root/Src/thirdparty/dpdk-stable-17.11.1/
RTE_TARGET = x86_64-native-linuxapp-gcc

export RTE_SDK
export RTE_TARGET

include $(RTE_SDK)/mk/rte.vars.mk

APP = test

SRCS-y := main.c

include $(RTE_SDK)/mk/rte.extapp.mk
