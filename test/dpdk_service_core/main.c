/*
 * [root@D128 service_cores]# ll ../../../dpdk-stable-17.11.1/
 *
 * [root@D128 service_cores]# uname -a
 * Linux D128 3.10.0-693.el7.x86_64 #1 SMP Tue Aug 22 21:09:27 UTC 2017
 * 	x86_64 x86_64 x86_64 GNU/Linux
 *
 */


#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/queue.h>

#include <rte_memory.h>
#include <rte_launch.h>
#include <rte_eal.h>
#include <rte_debug.h>
#include <rte_cycles.h>

#include <rte_service.h>

#include <rte_service_component.h>

static int32_t service_func(void *args)
{
	RTE_SET_USED(args);
	rte_delay_us(2000);
	int i = 0;

	while (i++ < 5) {
		sleep(1);
		printf("test. %s @ core %d\n", (char*)args, rte_lcore_id());
	}

	return 0;
}

int
main(int argc, char **argv)
{
	int ret;
	uint32_t id;
	int core;

	struct rte_service_spec services[] =  {
		{"service_1", service_func, "1", 0, 0},
		{"service_1", service_func, "2", 0, 0}};

	ret = rte_eal_init(argc, argv);
	if (ret < 0)
		rte_panic("Cannot init EAL\n");

	/* service */
	ret = rte_service_component_register(&services[0], &id);
	rte_service_component_runstate_set(id, 1);
	rte_service_set_stats_enable(id, 1);
	ret = rte_service_runstate_set(id, 1);
	printf("registe service: %d\n", id);

	ret = rte_service_component_register(&services[1], &id);
	rte_service_component_runstate_set(id, 1);
	rte_service_set_stats_enable(id, 1);
	ret = rte_service_runstate_set(id, 1);
	printf("registe service: %d\n", id);

	/* core */
	core = 2;
	ret = rte_service_lcore_add(core);
	ret = rte_service_lcore_start(core);
	core = 3;
	ret = rte_service_lcore_add(core);
	ret = rte_service_lcore_start(core);
	
	/* map service to core. */
	rte_service_map_lcore_set(0, 2, 1);
	rte_service_map_lcore_set(1, 2, 1);
	rte_service_map_lcore_set(0, 3, 1);
	rte_service_map_lcore_set(1, 3, 1);

	while (1) {
		sleep(1);
		printf("main...\n");
	}

	return 0;
}
