
RTE_SDK = /root/dpdk/
RTE_TARGET = x86_64-native-linuxapp-gcc

export RTE_SDK
export RTE_TARGET

include $(RTE_SDK)/mk/rte.vars.mk

APP = chimpanzee

SRCS-y := chimpanzee.c

include $(RTE_SDK)/mk/rte.extapp.mk
