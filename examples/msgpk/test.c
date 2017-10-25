
#include <stdio.h>
#include <msgpack.h>

int main(int argc, char** argv)
{
	msgpack_sbuffer sbuf;
	msgpack_sbuffer_init(&sbuf);

	msgpack_packer pk;
	msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

	msgpack_pack_array(&pk, 3);
	msgpack_pack_int(&pk, 1);
	msgpack_pack_true(&pk);
	msgpack_pack_str(&pk, 7);
	msgpack_pack_str_body(&pk, "example", 7);

	msgpack_zone mempool;
	msgpack_zone_init(&mempool, 2048);

	msgpack_object deserialized;
	msgpack_unpack(sbuf.data, sbuf.size, NULL, &mempool, &deserialized);

	msgpack_object_print(stdout, deserialized);
	puts("");

	msgpack_zone_destroy(&mempool);
	msgpack_sbuffer_destroy(&sbuf);

	return 0;
}
