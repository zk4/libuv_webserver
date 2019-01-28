#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <uv.h>
#include "http_parser/http_parser.h"

#define RESPONSE                   \
    "HTTP/1.1 200 OK\r\n"          \
    "Content-Type: text/plain\r\n" \
    "Content-Length: 12\r\n"       \
    "\r\n"                         \
    "hello world\n"

static uv_buf_t resbuf;
 
static http_parser_settings settings;

typedef struct
{
    uv_tcp_t tcp;
    http_parser parser;
    uv_write_t write_req;
} client_t;

void on_close(uv_handle_t *handle)
{
    free(handle);
}

static void on_alloc(uv_handle_t *handle, size_t suggested_size, uv_buf_t *buf)
{
    buf->base = malloc(suggested_size);
    buf->len = suggested_size;
}

static void on_read(uv_stream_t *handle, ssize_t nread, const uv_buf_t *buf)
{
    client_t *client = handle->data;
    size_t parsed;

    // if (nread >= 0)
    // {
    //     /* parse http */

    //     parsed = http_parser_execute(&client->parser,
    //                                  &settings,
    //                                  buf.base,
    //                                  nread);

    //     if (parsed < nread)
    //     {
    //         printf("parse error");
    //         uv_close((uv_handle_t *)handle, on_close);
    //     }
    // }
    // else
    // {

    //     if (nread == UV_EOF)
    //     {
    //         /* do nothing */
    //         fprintf(stderr, "Read error %s\n", uv_err_name(nread));
    //     }

    //     fprintf(stderr, "read: %s\n", uv_strerror(err));

    //     uv_close((uv_handle_t *)handle, on_close);
    // }

    // free(buf.base);
    ///
    if (nread < 0)
    {
        if (nread != UV_EOF)
            fprintf(stderr, "Read error %s\n", uv_err_name(nread));
        uv_close((uv_handle_t *)client, NULL);
        free(buf->base);
        free(client);
        return;
    }

    char *data = (char *)malloc(sizeof(char) * (nread + 1));
    data[nread] = '\0';
    strncpy(data, buf->base, nread);

    fprintf(stderr, "%s", data);
    free(data);
    free(buf->base);
}

void on_connected(uv_stream_t *server, int status)
{
    
    assert(status == 0);

    client_t *client = malloc(sizeof(client_t));
   
    int r = uv_tcp_init(server->loop, &client->tcp);
    if (r)
    {
        fprintf(stderr, "accept: %s\n", uv_strerror(r));
        return;
    }

    r = uv_accept( server, (uv_stream_t *)&client->tcp);
    if (r)
    {
        fprintf(stderr, "accept: %s\n", uv_strerror(r));
        return;
    }
  fprintf(stdout, "111111111111");
    client->tcp.data = client;
    client->parser.data = client;

    http_parser_init(&client->parser, HTTP_REQUEST);

    uv_read_start((uv_stream_t *)&client->tcp, on_alloc, on_read);
}

static void write_cb(uv_write_t *req, int status)
{
    uv_close((uv_handle_t *)req->data, on_close);
}

int on_headers_complete(http_parser *parser)
{
    client_t *client = parser->data;

    client->write_req.data = client;
    uv_write(&client->write_req, (uv_stream_t *)&client->tcp, &resbuf, 1, write_cb);

    return 1;
}

uv_loop_t *loop;

int main()
{
    fprintf(stderr, "tiny sever v0.0.1");

    resbuf.base = RESPONSE;
    resbuf.len = sizeof(RESPONSE);

    settings.on_headers_complete = on_headers_complete;
    struct sockaddr_in addr;

    loop = uv_default_loop();

    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    uv_ip4_addr("0.0.0.0", 8000, &addr);

    uv_tcp_bind(&server, (const struct sockaddr *)&addr, 0);
    int r = uv_listen((uv_stream_t *)&server, 128, on_connected);
    if (r)
    {
        fprintf(stderr, "Listen error %s\n", uv_strerror(r));
        return 1;
    }

    return uv_run(loop, UV_RUN_DEFAULT);
}
