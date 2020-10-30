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
#include <openssl/engine.h>
#include <openssl/async.h>

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
	int is_async;
	void* ssl;
	char buf[16];
};

OSSL_ASYNC_FD async_addfd[16];
OSSL_ASYNC_FD async_delfd[16];

int main(int argc, char** argv)
{
	/* set Engine QAT*/
	ENGINE_load_builtin_engines();
	ENGINE* eg = ENGINE_by_id("qat");
	if (eg) {
		printf("found qat engine.\n");
		ENGINE_init(eg);
		ENGINE_set_default(eg, ENGINE_METHOD_ALL);
	} else {
		printf("not found qat engine.\n");
	}
	const SSL_METHOD* m = TLS_server_method();
	SSL_CTX* ctx = SSL_CTX_new(m);

	SSL_CTX_set_mode(ctx, SSL_MODE_ASYNC);

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
	priv->fd = sock;
	priv->is_async = 0;
	priv->ssl = NULL;
	ev.data.ptr = priv;
	ev.events = EPOLLIN;

	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &ev) < 0) {
		perror("epoll_ctl add: 1");
		goto error;
	}
	printf("EPOLL add 0: %d\n", sock);
/*
	ev.data.ptr = NULL;
	if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, sock, &ev) < 0) {
		perror("epoll_ctl add: 1");
		goto error;
	}
	printf("EPOLL add 0xx: %d\n", sock);
*/
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
				priv->fd = fd;
				priv->is_async = 0;
				priv->ssl = ssl;
				ev.data.ptr = priv;
				ev.events = EPOLLIN;
				if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &ev) < 0) {
					perror("epoll_ctl add: 2");
					goto error;
				}
				printf("EPOLL add 1: %d\n", fd);
				e = SSL_accept(ssl);
				if (e <= 0) {
					int ee = SSL_get_error(ssl, e);
					if (ee == SSL_ERROR_WANT_READ || ee == SSL_ERROR_WANT_WRITE) {
						printf("SSL_accept haha continue: %d.\n", ee);
						continue;
					}
					if (ee == SSL_ERROR_WANT_ASYNC) {
						printf("ASYNC!!! SSL_accept 1 haha async continue: %d.\n", ee);
						size_t addfd_num;
						size_t delfd_num;
						if (SSL_get_changed_async_fds(ssl, async_addfd, &addfd_num,
								async_delfd, &delfd_num) != 1) {
							printf("SSL_get_changed_async_fds error.\n");
							goto error;
						}
						if (addfd_num > 1 || delfd_num >1) {
							printf("SSL_get_changed_async_fds > 1 add:[%lu], del:[%lu].\n", addfd_num, delfd_num);
							goto error;
						}
						if (addfd_num == 1) {
							struct epoll_priv_data *priv = malloc(sizeof(struct epoll_priv_data));
							if (priv == NULL) {
								perror("malloc: ");
								goto error;
							}
							priv->is_async = 1;
							priv->fd =async_addfd[0];
							priv->ssl = ssl;
							ev.data.ptr = priv;
							ev.events = EPOLLIN;
							if (addfd_num == 1 && epoll_ctl(epoll_fd, EPOLL_CTL_ADD, async_addfd[0], &ev) < 0) {
								perror("epoll_ctl add: a1");
								goto error;
							}
							printf("EPOLL add async 2: %d\n", async_addfd[0]);
						}
						if (delfd_num == 1) {
							if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, async_delfd[0], &ev) < 0) {
								perror("epoll_ctl del: a1");
								goto error;
							}
							printf("EPOLL del async 3: %d\n", async_delfd[0]);
							free(ev.data.ptr);
						}
						continue;
					}
					printf("e:%d SSL_accept haha error: %s\n", e,
							ERR_error_string(ee, NULL));
					goto error;
				}
				printf("accepted a.\n");
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
					printf("EPOLL del 4: %d\n", fd);
					free(ev.data.ptr);
					close(fd);
					free(priv);
					printf("closed.\n");
				} else {
/* not sure howto here... 

*/
					if (priv->is_async && SSL_waiting_for_async(ssl) == 1){
//						printf("async wait.\n");
//	因为是协程,所以,如果不继续进行read write的话, 也没有机会驱动离开waiting async状态. 
//						continue;
					}
					if (SSL_is_init_finished(ssl) != 1) {
						e = SSL_accept(ssl);
						if (e <= 0) {
							int ee = SSL_get_error(ssl, e);
							if (ee == SSL_ERROR_WANT_READ || ee == SSL_ERROR_WANT_WRITE) {
								printf("SSL_accept continue: %d.\n", ee);
								continue;
							}
							if (ee == SSL_ERROR_WANT_ASYNC) {
								printf("ASYNC!!! SSL_accept 2 haha async continue: %d.\n", ee);
								size_t addfd_num;
								size_t delfd_num;
								if (SSL_get_changed_async_fds(ssl, async_addfd, &addfd_num,
										async_delfd, &delfd_num) != 1) {
									printf("SSL_get_changed_async_fds error.\n");
									goto error;
								}
								if (addfd_num > 1 || delfd_num >1) {
									printf("SSL_get_changed_async_fds > 1 add:[%lu], del:[%lu].\n", addfd_num, delfd_num);
									goto error;
								}
								if (addfd_num == 1) {
									struct epoll_priv_data *priv = malloc(sizeof(struct epoll_priv_data));
									if (priv == NULL) {
										perror("malloc: ");
										goto error;
									}
									priv->is_async = 1;
									priv->fd =async_addfd[0];
									priv->ssl = ssl;
									ev.data.ptr = priv;
									ev.events = EPOLLIN;
									if (addfd_num == 1 && epoll_ctl(epoll_fd, EPOLL_CTL_ADD, async_addfd[0], &ev) < 0) {
										perror("epoll_ctl add: a1");
										goto error;
									}
									printf("EPOLL add async 5: %d\n", async_addfd[0]);
								}
								if (delfd_num == 1) {
									if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, async_delfd[0], &ev) < 0) {
										perror("epoll_ctl del: a1");
										goto error;
									}
									printf("EPOLL del async 6: %d\n", async_delfd[0]);
									free(ev.data.ptr);
								}
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
							if (ee == SSL_ERROR_WANT_ASYNC) {
								printf("ASYNC!!! SSL_read haha async continue: %d.\n", ee);
								size_t addfd_num;
								size_t delfd_num;
								if (SSL_get_changed_async_fds(ssl, async_addfd, &addfd_num,
									async_delfd, &delfd_num) != 1) {
									printf("SSL_get_changed_async_fds error.\n");
									goto error;
								}
								if (addfd_num > 1 || delfd_num >1) {
									printf("SSL_get_changed_async_fds > 1 add:[%lu ], del:[%lu ].\n", addfd_num, delfd_num);
									goto error;
								}
								if (addfd_num == 1) {
									struct epoll_priv_data *priv = malloc(sizeof(struct epoll_priv_data));
									if (priv == NULL) {
										perror("malloc: ");
										goto error;
									}
									priv->is_async = 1;
									priv->fd =async_addfd[0];
									priv->ssl = ssl;
									ev.data.ptr = priv;
									ev.events = EPOLLIN;
									if (addfd_num == 1 && epoll_ctl(epoll_fd, EPOLL_CTL_ADD, async_addfd[0], &ev) < 0) {
										perror("epoll_ctl add: a1");
										goto error;
									}
									printf("EPOLL add async 7: %d\n", async_addfd[0]);
								}
								if (delfd_num == 1) {
									if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, &ev) < 0) {
										perror("epoll_ctl del: a1");
										goto error;
									}
									printf("EPOLL del async 8: %d\n", async_delfd[0]);
									free(ev.data.ptr);
								}
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
							if (ee == SSL_ERROR_WANT_ASYNC) {
								printf("ASYNC!!! SSL_write haha async continue: %d.\n", ee);
								size_t addfd_num;
								size_t delfd_num;
								if (SSL_get_changed_async_fds(ssl, async_addfd, &addfd_num,
									async_delfd, &delfd_num) != 1) {
									printf("SSL_get_changed_async_fds error.\n");
									goto error;
								}
								if (addfd_num > 1 || delfd_num >1) {
									printf("SSL_get_changed_async_fds > 1 add:[%lu ], del:[%lu ].\n", addfd_num, delfd_num);
									goto error;
								}
								if (addfd_num == 1) {
									struct epoll_priv_data *priv = malloc(sizeof(struct epoll_priv_data));
									if (priv == NULL) {
										perror("malloc: ");
										goto error;
									}
									priv->is_async = 1;
									priv->fd =async_addfd[0];
									priv->ssl = ssl;
									ev.data.ptr = priv;
									ev.events = EPOLLIN;
									if (addfd_num == 1 && epoll_ctl(epoll_fd, EPOLL_CTL_ADD, async_addfd[0], &ev) < 0) {
										perror("epoll_ctl add: a1");
										goto error;
									}
									printf("EPOLL add async 9: %d\n", async_addfd[0]);
								}
								if (delfd_num == 1) {
									if (epoll_ctl(epoll_fd, EPOLL_CTL_DEL, async_delfd[0], &ev) < 0) {
										perror("epoll_ctl del: a1");
										goto error;
									}
									printf("EPOLL del async 10: %d\n", async_delfd[0]);
									free(ev.data.ptr);
								}
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
	if (eg) {
		ENGINE_finish(eg);
		ENGINE_free(eg);
	}
	return 0;
error:
	printf("error.\n");
	SSL_CTX_free(ctx);
	if (eg) {
		ENGINE_finish(eg);
		ENGINE_free(eg);
	}
	return -1;
}
