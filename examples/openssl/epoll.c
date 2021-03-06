/**
	openssl async test by tony.
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
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

	fd = socket(AF_INET, SOCK_STREAM|SOCK_NONBLOCK, 0);
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


int set_nonblocking(int fd)
{
	int flags;
	do {
		flags = fcntl(fd, F_GETFL);
	} while (flags < 0 && (errno == EAGAIN || errno == EINTR));
	if (flags < 0)
		flags = 0;
	flags |= O_NONBLOCK;
	return fcntl(fd, F_SETFL, flags);
}

struct epoll_priv_data {
	int fd;
	void* ssl;
	char buf[16];
};

int main(int argc, char** argv)
{
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

	int epoll_fd = epoll_create1(0);
	if (epoll_fd < 0) {
		perror("epoll_create1(): ");
		goto error;
	}

	struct epoll_event ev;
	struct epoll_event events[16];

	struct epoll_priv_data *priv = malloc(sizeof(struct epoll_priv_data));
	if (priv == NULL) {
		perror("malloc: ");
		goto error;
	}
	priv->fd =sock;
	priv->ssl = NULL;
	ev.data.ptr = priv;
	ev.events = EPOLLIN;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &ev) < 0) {
		perror("epoll_ctl add: 1");
		goto error;
	}

	while (1) {
		struct sockaddr_in addr;
		uint32_t len = sizeof(addr);

		printf("waitting...\n");

		int nfds, n;

		nfds = epoll_wait(epoll_fd, events, 16, -1);
		if (nfds == -1) {
			perror("epoll_wait:");
			goto error;
		}

		for (n = 0; n < nfds; n++) {
			int e;
			struct epoll_priv_data* priv = events[n].data.ptr;
			if (priv->fd == sock) {
				int fd = accept(sock, (struct sockaddr*)&addr, &len);
				if (fd < 0) {
					perror("accept: ");
					goto error;
				}

				if (set_nonblocking(fd) < 0) {
					perror("set block.\n");
					goto error;
				}

				SSL* ssl = SSL_new(ctx);
				if (SSL_set_fd(ssl, fd) != 1) {
					printf("SSL_set_fd error: %s\n",
						ERR_error_string(SSL_get_error(ssl, e), NULL));
					goto error;
				}
				struct epoll_priv_data *priv = malloc(sizeof(struct epoll_priv_data));
				if (priv == NULL) {
					perror("malloc: ");
					goto error;
				}
				priv->fd =fd;
				priv->ssl = ssl;
				ev.data.ptr = priv;
//				ev.events = EPOLLIN | EPOLLOUT;
				ev.events = EPOLLIN;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
					perror("epoll_ctl add: 2");
					goto error;
				}
				e = SSL_accept(ssl);
				if (e <= 0) {
					int ee = SSL_get_error(ssl, e);
					if (ee == SSL_ERROR_WANT_READ || ee == SSL_ERROR_WANT_WRITE) {
						printf("SSL_accept haha continue: %d.\n", ee);
						continue;
					}
					printf("e:%d SSL_accept haha error: %s\n", e,
							ERR_error_string(ee, NULL));
					goto error;
				}
				printf("accepted.\n");
			} else {
				char* buf = priv->buf;
				SSL* ssl = priv->ssl;
				int fd = priv->fd;

				if ((events[n].events & (EPOLLIN | EPOLLOUT)) == 0) {
					printf("epoll wait events unkonwn: %d.\n", events[n].events);
close:
					if (SSL_shutdown(ssl) < 0)
						printf("SSL_write error.\n");
					SSL_free(ssl);
					if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ev) < 0) {
						perror("epoll_ctl del: 1");
						goto error;
					}
					close(fd);
					free(priv);
					printf("closed.\n");
				} else {
					if (SSL_is_init_finished(ssl) != 1) {
						e = SSL_accept(ssl);
						if (e <= 0) {
							int ee = SSL_get_error(ssl, e);
							if (ee == SSL_ERROR_WANT_READ || ee == SSL_ERROR_WANT_WRITE) {
								printf("SSL_accept continue: %d.\n", ee);
								continue;
							}
							printf("e:%d SSL_accept error: %s\n", e,
								ERR_error_string(ee, NULL));
							goto error;
						}
						printf("accepted.\n");
					}
					if ((events[n].events & EPOLLIN) != 0) {
						e = SSL_read(ssl, buf, 16);
						if (e <= 0) {
							int ee = SSL_get_error(ssl, e);
							if (ee == SSL_ERROR_WANT_READ || ee == SSL_ERROR_WANT_WRITE) {
								printf("SSL_read continue: %d.\n", ee);
								continue;
							}
							ERR_print_errors_fp(stderr);
							printf("SSL_read error.\n");
							goto close;
						}
						buf[15] = 0;
						printf("read[%d]: %s\n", e, buf);
						if (buf[0] == 'a') {
							buf[0] = 'b';
						} else if (buf[0] == 'e') {
							goto close;
						} else {
							buf[0] = 'x';
						}
						buf[1] = 0;

						e = SSL_write(ssl, buf, 2);
						if (e <= 0) {
							int ee = SSL_get_error(ssl, e);
							if (ee == SSL_ERROR_WANT_READ || ee == SSL_ERROR_WANT_WRITE) {
								printf("SSL_write continue: %d.\n", ee);
								continue;
							}
							ERR_print_errors_fp(stderr);
							printf("SSL_write error.\n");
							goto close;
						}
						buf[0] = 0;
						printf("write[%d].\n", e);
					}
				}
			}
		}
	}
	close(epoll_fd);
	close(sock);
	SSL_CTX_free(ctx);

	return 0;
error:
	printf("error.\n");
	SSL_CTX_free(ctx);
	return -1;
}
