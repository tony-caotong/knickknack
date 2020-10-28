/**
	openssl async test by tony.
*/

#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

int create_socket(int port)
{
	struct sockaddr_in addr;
	int fd = 0;
	int flag = 1;

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0) {
		perror("socket: ");
		return -1;
	}
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &flag, sizeof(flag)) < 0) {
		perror("setsockopt:");
		return -1;
	}
	if (bind(fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
		perror("bind: ");
		return -1;
	}
	if (listen(fd, 0) < 0) {
		perror("listen: ");
		return -1;
	}
	return fd;
}

int config_context(SSL_CTX* ctx)
{
	if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) != 1) {
		ERR_print_errors_fp(stderr);
		return -1;
	}
	if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) != 1) {
		ERR_print_errors_fp(stderr);
		return -1;
	}
	return 0;
}

int main(int argc, char** argv)
{
//	ERR_load_crypto_strings();
//	OpenSSL_add_all_ciphers();

	const SSL_METHOD* m = TLS_server_method();
	SSL_CTX* ctx = SSL_CTX_new(m);

	if (config_context(ctx) < 0) {
		printf("config_context.\n");
		goto error;
	}

	int sock = create_socket(4443);
	if (sock < 0) {
		goto error;
	}
	while (1) {
		struct sockaddr_in addr;
		uint32_t len = sizeof(addr);

		printf("waitting...\n");
		int fd = accept(sock, (struct sockaddr*)&addr, &len);
		if (fd < 0) {
			perror("accept: ");
			goto error;
		}

		SSL* ssl = SSL_new(ctx);
		SSL_set_fd(ssl, fd);
		if (SSL_accept(ssl) <= 0) {
			ERR_print_errors_fp(stderr);
			printf("SSL_accept error.\n");
			goto error;
		}
		char buf[128] = {0};
		printf("accepted.\n");
		while (1) {
			int e;
			e = SSL_read(ssl, &buf, sizeof(buf));
			if (e <= 0) {
				ERR_print_errors_fp(stderr);
				printf("SSL_read error.\n");
				goto close;
			}
			buf[127] = 0;
			printf("read: %s\n", buf);
			if (buf[0] == 'a') {
				buf[0] = 'b';
			} else if (buf[0] == 'e') {
				break;
			} else {
				buf[0] = 'x';
			}
			buf[1] = 0;
			e = SSL_write(ssl, &buf, 2);
			if (e <= 0) {
				ERR_print_errors_fp(stderr);
				printf("SSL_write error.\n");
				goto close;
			}
			printf("write.\n");
		}
close:
		if (SSL_shutdown(ssl) < 0)
			printf("SSL_write error.\n");
		SSL_free(ssl);
		close(fd);
		printf("closed.\n");
	}
	close(sock);
	SSL_CTX_free(ctx);

	return 0;
error:
	printf("error.\n");
	SSL_CTX_free(ctx);
	return -1;
}
