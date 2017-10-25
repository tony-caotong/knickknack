#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <arpa/inet.h>

#include <nids.h>
#include <htp/htp.h>

struct user_data {
    FILE *request_body_file;
    FILE *response_body_file;
};

struct user_data udata;

struct ht_sniff_cfg {
    htp_cfg_t * hcfg;
    char      * iface;
    char      * filter;
};

struct ht_sniff_cfg * cfg;

int HTPCallbackRequestLine(htp_tx_t *tx)
{
    printf("request line = %*.*s\n", (int)bstr_len(tx->request_line), (int)bstr_len(tx->request_line), bstr_ptr(tx->request_line));
    return HTP_OK;
}

int HTPCallbackRequestHeaders(htp_tx_t *tx)
{
    printf("request headers\n");

    int i;
    htp_header_t *h;
    int table_size = htp_table_size(tx->request_headers);

    for (i = 0; i < table_size; i++) {
	h = (htp_header_t *)htp_table_get_index(tx->request_headers, i, NULL);
        printf("request header name = %*.*s\n", (int)bstr_len(h->name), (int)bstr_len(h->name), bstr_ptr(h->name));
        printf("request header value = %*.*s\n", (int)bstr_len(h->value), (int)bstr_len(h->value), bstr_ptr(h->value));
    }

    return HTP_OK;
}

int HTPCallbackRequestBodyData(htp_tx_data_t *tx_data)
{
    int ret;
    printf("request body length = %zu\n", tx_data->len);

    struct user_data *ud = (struct user_data *) htp_tx_get_user_data(tx_data->tx);
    if (ud == NULL) {
        /* Set the user data for handling body chunks on this transaction */
        ud = &udata;
        htp_tx_set_user_data(tx_data->tx, ud);
    }

    if (ud->request_body_file == NULL) {
        ud->request_body_file = fopen("/tmp/request_body_file.txt", "w");
        if (ud->request_body_file == NULL) {
            printf("create request_body_file.txt error\n");
            return HTP_ERROR;
        }
    }

    if (tx_data->len == 0) {
        if (ud->request_body_file == NULL) {
            printf("close request_body_file.txt error\n");
            return HTP_ERROR;
        }
        fclose(ud->request_body_file);
        ud->request_body_file = NULL;
        return HTP_OK;
    }

    // to do deal return result
    ret = fwrite(tx_data->data, tx_data->len, 1, ud->request_body_file);
    printf("fwrite %d\n", ret);

    return HTP_OK;
}



int HTPCallbackResponseLine(htp_tx_t *tx)
{
    printf("response line = %*.*s\n", (int)bstr_len(tx->response_line), (int)bstr_len(tx->response_line), bstr_ptr(tx->response_line));
    return HTP_OK;
}

int HTPCallbackResponseHeaders(htp_tx_t *tx)
{
    printf("response headers\n");

    int i;
    htp_header_t *h;
    int table_size = htp_table_size(tx->response_headers);

    for (i = 0; i < table_size; i++) {
	h = (htp_header_t *)htp_table_get_index(tx->response_headers, i, NULL);
        printf("response header name = %*.*s\n", (int)bstr_len(h->name), (int)bstr_len(h->name), bstr_ptr(h->name));
        printf("response header value = %*.*s\n", (int)bstr_len(h->value), (int)bstr_len(h->value), bstr_ptr(h->value));
    }

    return HTP_OK;
}

int HTPCallbackResponseBodyData(htp_tx_data_t *tx_data)
{
    int ret;
    printf("response body length = %zu\n", tx_data->len);

    struct user_data *ud = (struct user_data *) htp_tx_get_user_data(tx_data->tx);
    if (ud == NULL) {
        /* Set the user data for handling body chunks on this transaction */
        ud = &udata;
        htp_tx_set_user_data(tx_data->tx, ud);
    }

    if (ud->response_body_file == NULL) {
        ud->response_body_file = fopen("/tmp/response_body_file.txt", "w");
        if (ud->response_body_file == NULL) {
            printf("create response_body_file.txt error\n");
            return HTP_ERROR;
        }
    }

    if (tx_data->len == 0) {
        if (ud->response_body_file == NULL) {
            printf("close response_body_file.txt error\n");
            return HTP_ERROR;
        }
        fclose(ud->response_body_file);
        ud->response_body_file = NULL;
        return HTP_OK;
    }

    // to do deal return result
    ret = fwrite(tx_data->data, tx_data->len, 1, ud->response_body_file);

    return HTP_OK;
}




void ht_sniff_cb(struct tcp_stream * a_tcp, void ** pkt)
{
    int ret;
    char srcaddr[32];
    char dstaddr[32];
    struct timeval tv;
    struct half_stream * hlf;

    htp_connp_t *connp;

    gettimeofday(&tv, NULL);

    switch (a_tcp->nids_state) {
        case NIDS_JUST_EST:
	        printf("new http\n");
	        a_tcp->client.collect++;
	        a_tcp->server.collect++;

            strncpy(srcaddr, (const char *)inet_ntoa(*((struct in_addr *)&a_tcp->addr.saddr)), 31);
            strncpy(dstaddr, (const char *)inet_ntoa(*((struct in_addr *)&a_tcp->addr.daddr)), 31);

            connp = htp_connp_create(cfg->hcfg);
            htp_connp_open(connp, srcaddr, a_tcp->addr.source, dstaddr, a_tcp->addr.dest, &tv);
            a_tcp->user = connp;
            break;
        case NIDS_TIMED_OUT:
        case NIDS_CLOSE:
        case NIDS_EXITING:
        case NIDS_RESET:
            if ((connp = (htp_connp_t *)a_tcp->user)) {
	            printf("free http\n");
                htp_connp_close(connp, &tv);
                htp_connp_destroy_all(connp);
            }
            break;
        case NIDS_DATA:
            if (!(connp = (htp_connp_t *)a_tcp->user)) {
                break;
            }

	        if (a_tcp->client.count_new) {
                printf("server send data\n");

		        hlf = &a_tcp->client;
                ret = htp_connp_res_data(connp, &tv, hlf->data, hlf->count_new);

                switch (ret) {
                case HTP_STREAM_CLOSED:
                    printf("HTP_STREAM_CLOSED\n");
                    break;
                case HTP_STREAM_ERROR:
                    printf("HTP_STREAM_ERROR\n");
                    break;
                case HTP_STREAM_TUNNEL:
                    printf("HTP_STREAM_TUNNEL\n");
                    break;
                case HTP_STREAM_DATA_OTHER:
                    printf("HTP_STREAM_DATA_OTHER\n");
                    break;
                case HTP_STREAM_STOP:
                    printf("HTP_STREAM_STOP\n");
                    break;
                case HTP_STREAM_DATA:
                    printf("HTP_STREAM_DATA\n");
                    break;
                default:
                    printf("response other ret = %d\n", ret);
                    break;
                }

	        } else {
                printf("client send data\n");

		        hlf = &a_tcp->server;
                ret = htp_connp_req_data(connp, &tv, hlf->data, hlf->count_new);

                switch (ret) {
                case HTP_STREAM_CLOSED:
                    printf("HTP_STREAM_CLOSED\n");
                    break;
                case HTP_STREAM_ERROR:
                    printf("HTP_STREAM_ERROR\n");
                    break;
                case HTP_STREAM_TUNNEL:
                    printf("HTP_STREAM_TUNNEL\n");
                    break;
                case HTP_STREAM_DATA_OTHER:
                    printf("HTP_STREAM_DATA_OTHER\n");
                    break;
                case HTP_STREAM_STOP:
                    printf("HTP_STREAM_STOP\n");
                    break;
                case HTP_STREAM_DATA:
                    printf("HTP_STREAM_DATA\n");
                    break;
                default:
                    printf("request other ret = %d\n", ret);
                    break;
                }
	        }
            break;
        default:
            break;
    }
}

int parse_args(struct ht_sniff_cfg * cfg, int argc, char ** argv)
{
    extern char * optarg;
    extern int    optind,
                  opterr,
                  optopt;
    int           c;

    static char * help =
        "Options: \n"
        " -h:         Derr...\n"
        " -i <iface>: Interface to sniff\n"
        " -l <fmt>  : Log format\n"
        " -f <bpf>:   BPF Filter\n";

    cfg->iface  = "eth1";
    cfg->filter = "tcp port 80";

    while ((c = getopt(argc, argv, "hi:f:l:")) != -1) {
        switch (c) {
            case 'i':
                cfg->iface = optarg;
                break;
            case 'f':
                cfg->filter = optarg;
                break;
            case 'h':
            default:
                printf("Usage: %s [opts\n%s",
                       argv[0], help);
                exit(1);
        }
    }

    return 0;
}

void ht_sniff_init(struct ht_sniff_cfg * cfg)
{
    cfg->hcfg = htp_config_create();

    htp_config_set_tx_auto_destroy(cfg->hcfg, 1);

    htp_config_set_server_personality(cfg->hcfg, HTP_SERVER_IDS);

    // register request hook
    htp_config_register_request_line(cfg->hcfg, HTPCallbackRequestLine);
    htp_config_register_request_headers(cfg->hcfg, HTPCallbackRequestHeaders);
    htp_config_register_request_body_data(cfg->hcfg, HTPCallbackRequestBodyData);

    // register response hook
    htp_config_register_response_line(cfg->hcfg, HTPCallbackResponseLine);
    htp_config_register_response_headers(cfg->hcfg, HTPCallbackResponseHeaders);
    htp_config_register_response_body_data(cfg->hcfg, HTPCallbackResponseBodyData);
}

int main(int argc, char **argv)
{
    struct nids_chksum_ctl ctl;

    if (!(cfg = (struct ht_sniff_cfg *)calloc(sizeof(struct ht_sniff_cfg), 1))) {
        perror(strerror(errno));
    }

    if (!(cfg->hcfg = htp_config_create())) {
        perror(strerror(errno));
    }

    parse_args(cfg, argc, argv);

    ht_sniff_init(cfg);

    // turn off checksum offloading
    ctl.netaddr = 0;
    ctl.mask    = 0;
    ctl.action  = NIDS_DONT_CHKSUM;
    nids_register_chksum_ctl(&ctl, 1);

    //nids_params.pcap_filter     = cfg->filter;
    nids_params.device          = cfg->iface;

    if (!nids_init()) {
        printf("%s\n", nids_errbuf);
        exit(1);
    }

    nids_register_tcp(ht_sniff_cb);
    nids_run();

    return 0;
}
