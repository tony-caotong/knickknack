/**
 * 	Chimpanzee under anthropoid.
 *		by Cao tong <tony.caotong@gmail.com>
 *		@ 2017-09-04
 *
 */

#include <stdio.h>
#include <stdbool.h>
#include <wordexp.h>
#include <string.h>
#include <errno.h>

#include <rte_eal.h>

#include "environment.h"
#include "configure.h"
#include "logger.h"

static bool Quit;

int dpdk_init(char* args)
{
	int r;
	wordexp_t p;

	if (wordexp(args, &p, 0) != 0) {
		err("wordexp: %d\n", strerror(errno));
		return -1;
	}

	r = rte_eal_init(p.we_wordc, p.we_wordv);
	if (r < 0) {
		err("rte_eal_init %d\n", rte_strerror(rte_errno));
		return r;
	}
	wordfree(&p);

	debug("dpdp_init success.\n");
	return 0;
}
	
int main(int argc, char** argv)
{
	struct configure_t c;
	struct environment_t e;
	wordexp_t 

	/* TODO: Should follow the philosophy of the New-Style Deamons. */
	fprintf(stderr, "name: %s\n", ENV_SELF_NAME);
	fprintf(stderr, "version: %s\n", ENV_VERSION);

	if (environment_init("ANTHROPOID_INS_ROOT", &e) < 0)
		return -1;

	/* TODO: Set thread name, use thread name with log. */

	if ((Logger = log_init(ENV_SELF_NAME, ENV_log_conf_file, 0, 0))
			== NULL) {
		err("LOGGER init failed.\n");
		return -1;
	}

	info("name: %s\n", ENV_SELF_NAME);
	info("version: %s\n", ENV_VERSION);
	info("root_path: %s\n", ENV_root_path);

	if (conf_reflect(ENV_conf_file, &c) < 0) {
		err("read_conf error.\n");
		goto stage1;
	}

	/* initilize dpdk enviroment. */
	if (dpdk_init() < 0) {
		err("dpdk_init failed.\n");
		goto stage1;
	}

	/* create mbuf pool. */

	/* configure port. */

	/* configure queue. */

	/* launch device. */

	/* strip prime thread. */

	/* strip assist thread. */

	/* setup signal. */

	info("hello, let's rock.\n");

	/* master logic */
	
	/* wait. */

	/* destroy resource. */

stage1:
	log_des(Logger);

	/* bye. */
	info("ByeBye.\n");
	return 0;
}

