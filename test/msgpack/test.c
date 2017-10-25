#include <unistd.h>
#include <msgpack.h>

int main()
{
	/* packing */
	printf("packing !\n");
	msgpack_sbuffer sbuf;
	msgpack_packer pk;

	msgpack_sbuffer_init(&sbuf);
	msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);

	msgpack_pack_char(&pk, 'A');
	msgpack_pack_int(&pk, 100);
	msgpack_pack_int32(&pk, 500);

	char* s = "hello msgpack";
	msgpack_pack_str(&pk, strlen(s));
	msgpack_pack_str_body(&pk, s, strlen(s));

	
	/* unpack */
	printf("unpack !\n");
	/*
	msgpack_unpacker* unp = msgpack_unpacker_new(100);
	if (unp) {

	}
	msgpack_unpacker_free(unp);
	*/

	msgpack_unpacker unp;

	bool result = msgpack_unpacker_init(&unp, 100);
	printf("msgpack_unpacker_init result: %d !\n", result);

	if (result) {
		memcpy(msgpack_unpacker_buffer(&unp), sbuf.data, sbuf.size);
		msgpack_unpacker_buffer_consumed(&unp, sbuf.size);

		msgpack_unpacked und;
		msgpack_unpack_return ret;
		msgpack_unpacked_init(&und);
		
		int q = 0;
		while (!q) {
			msgpack_object obj;
			obj.type = MSGPACK_OBJECT_NIL;

			ret = msgpack_unpacker_next(&unp, &und);
			printf("msgpack_unpacker_next result: %d !\n", ret);
			switch (ret) {
			case MSGPACK_UNPACK_SUCCESS: {
				obj = und.data;
				break;
			}
			case MSGPACK_UNPACK_CONTINUE:
				q = 1;
				break;
			case MSGPACK_UNPACK_PARSE_ERROR:
				printf("next() error.\n");
				break;
			default:
				break;
			}

			switch (obj.type) {
			case MSGPACK_OBJECT_POSITIVE_INTEGER:
				printf("obj: <int> %ld\n", obj.via.u64);
				break;
			case MSGPACK_OBJECT_STR:
				// TODO: string length should be judgement.
				printf("obj: <string>[%d] %s\n",
					obj.via.str.size, obj.via.str.ptr);
				break;
			case MSGPACK_OBJECT_NIL:
			case MSGPACK_OBJECT_BOOLEAN:
			case MSGPACK_OBJECT_NEGATIVE_INTEGER:
			case MSGPACK_OBJECT_FLOAT32:
			case MSGPACK_OBJECT_FLOAT:
			case MSGPACK_OBJECT_ARRAY:
			case MSGPACK_OBJECT_MAP:
			case MSGPACK_OBJECT_BIN:
			case MSGPACK_OBJECT_EXT:
			default:
				printf("%d \n", obj.type);
				break;
			}
			sleep(1);
		}
		msgpack_unpacked_destroy(&und);
	}
	msgpack_unpacker_destroy(&unp);

	return 0;
}
