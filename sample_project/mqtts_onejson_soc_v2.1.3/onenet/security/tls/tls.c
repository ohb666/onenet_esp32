/**
 * Copyright (c), 2012~2024 iot.10086.cn All Rights Reserved
 *
 * @file tls.c
 * @brief TCP/IP protocol process with TLS
 */

/*****************************************************************************/
/* Includes                                                                  */
/*****************************************************************************/
#include "tls.h"
#include "err_def.h"
#include "plat_osl.h"
#include "plat_tcp.h"
#include "plat_time.h"

#if defined(CONFIG_NETWORK_TLS) && CONFIG_NETWORK_TLS == 1
#include "wolfssl/ssl.h"
#endif

#include "common.h"
#include <stdio.h>
/*****************************************************************************/
/* Local Definitions ( Constant and Macro )                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Structures, Enum and Typedefs                                             */
/*****************************************************************************/
struct tls_t {
  handle_t handle;
#if defined(CONFIG_NETWORK_TLS) && CONFIG_NETWORK_TLS == 1
  WOLFSSL_CTX *wolf_ctx;
  WOLFSSL *wolf_ssl;
#endif
  uint32_t send_timeout;
  uint32_t recv_timeout;
};
/*****************************************************************************/
/* Local Function Prototype                                                  */
/*****************************************************************************/

/*****************************************************************************/
/* Local Variables                                                           */
/*****************************************************************************/

/*****************************************************************************/
/* Global Variables                                                          */
/*****************************************************************************/

/*****************************************************************************/
/* Function Implementation                                                   */
/*****************************************************************************/
#if defined(CONFIG_NETWORK_TLS) && CONFIG_NETWORK_TLS == 1
static int wolfssl_send(WOLFSSL *ssl, char *buf, int sz, void *ctx) {
  struct tls_t *net = (struct tls_t *)ctx;
  int32_t ret = 0;

  if (0 == (ret = plat_tcp_send(net->handle, buf, sz, net->send_timeout))) {
    ret = -2; // WOLFSSL_CBIO_ERR_WANT_WRITE
  }

  return ret;
}

static int wolfssl_recv(WOLFSSL *ssl, char *buf, int sz, void *ctx) {
  struct tls_t *net = (struct tls_t *)ctx;
  int32_t ret = 0;

  if (0 == (ret = plat_tcp_recv(net->handle, buf, sz, net->recv_timeout))) {
    return -2; // WOLFSSL_CBIO_ERR_WANT_READ
  }

  return ret;
}

handle_t tls_connect(const uint8_t *host, uint16_t port, const uint8_t *ca_cert,
                     uint16_t ca_cert_len, uint32_t timeout) {
  struct tls_t *net = NULL;
  handle_t tmr = 0;
  int connect_ret = ERR_OTHERS;
  int ssl_err = 0;

  /* 1. Initialize TLS context */
  SAFE_ALLOC(net, sizeof(struct tls_t));
  tmr = countdown_start(timeout);
  CHECK_EXPR_GOTO(0 == tmr, _ERROR, "Failed to start timeout counter");

  wolfSSL_Init();
  net->wolf_ctx = wolfSSL_CTX_new(wolfTLSv1_2_client_method());
  CHECK_EXPR_GOTO(!net->wolf_ctx, _ERROR, "Failed to create SSL context");

  /* 2. Load and verify CA certificate */
#if defined(USE_SDK_HTTPS)
  CHECK_EXPR_GOTO(
      wolfSSL_CTX_load_verify_buffer(net->wolf_ctx, ca_cert, ca_cert_len,
                                     WOLFSSL_FILETYPE_PEM) != SSL_SUCCESS,
      _ERROR, "Failed to load CA certificate");
  wolfSSL_CTX_set_verify(net->wolf_ctx, WOLFSSL_VERIFY_PEER, NULL);
#else
  wolfSSL_CTX_set_verify(net->wolf_ctx, WOLFSSL_VERIFY_NONE, NULL);
#endif

  /* 3. Set IO callbacks */
  wolfSSL_SetIOSend(net->wolf_ctx, wolfssl_send);
  wolfSSL_SetIORecv(net->wolf_ctx, wolfssl_recv);

  /* 4. Establish TCP connection */
  net->handle = plat_tcp_connect(host, port, countdown_left(tmr));
  CHECK_EXPR_GOTO(net->handle < 0, _ERROR,
                  "Failed to establish TCP connection");

  /* 5. Create SSL session */
  net->wolf_ssl = wolfSSL_new(net->wolf_ctx);
  CHECK_EXPR_GOTO(!net->wolf_ssl, _ERROR, "Failed to create SSL session");

  wolfSSL_set_fd(net->wolf_ssl, net->handle);
  wolfSSL_SetIOWriteCtx(net->wolf_ssl, net);
  wolfSSL_SetIOReadCtx(net->wolf_ssl, net);
  net->send_timeout = net->recv_timeout = countdown_left(tmr);

  /* 6. Perform SSL handshake */
  while ((connect_ret = wolfSSL_connect(net->wolf_ssl)) != SSL_SUCCESS) {
    ssl_err = wolfSSL_get_error(net->wolf_ssl, connect_ret);

    if (ssl_err == SSL_ERROR_WANT_READ) {
      if (countdown_is_expired(tmr)) {
        logd("TLS handshake timeout");
        break;
      }
      net->send_timeout = net->recv_timeout = countdown_left(tmr);
      continue;
    }
    logd("TLS handshake failed with error: %d", ssl_err);
    break;
  }

  CHECK_EXPR_GOTO(connect_ret != SSL_SUCCESS, _ERROR, "TLS handshake failed");

  countdown_stop(tmr);
  return (handle_t)net;

_ERROR:
  if (net) {
    if (net->handle >= 0)
      plat_tcp_disconnect(net->handle);
    if (net->wolf_ssl)
      wolfSSL_free(net->wolf_ssl);
    if (net->wolf_ctx)
      wolfSSL_CTX_free(net->wolf_ctx);
    countdown_stop(tmr);
    SAFE_FREE(net);
  }
  return ERR_FAIL;
}

int32_t tls_send(handle_t handle, void *buf, uint32_t len, uint32_t timeout) {
  struct tls_t *net = (struct tls_t *)handle;
  int32_t ret = 0;

  net->send_timeout = timeout;
  ret = wolfSSL_write(net->wolf_ssl, buf, len);

  if (wolfSSL_want_write(net->wolf_ssl)) {
    return 0;
  }

  return ret;
}

int32_t tls_recv(handle_t handle, void *buf, uint32_t len, uint32_t timeout) {
  struct tls_t *net = (struct tls_t *)handle;
  int32_t ret = 0;

  net->recv_timeout = timeout;
  ret = wolfSSL_read(net->wolf_ssl, buf, len);

  if (wolfSSL_want_read(net->wolf_ssl)) {
    return 0;
  }

  return ret;
}

int32_t tls_disconnect(handle_t handle) {
  struct tls_t *net = (struct tls_t *)handle;

  if (net) {
    plat_tcp_disconnect(net->handle);
    wolfSSL_free(net->wolf_ssl);
    wolfSSL_CTX_free(net->wolf_ctx);
    osl_free(net);
  }

  return 0;
}
#else
handle_t tls_connect(const uint8_t *host, uint16_t port, const uint8_t *ca_cert,
                     uint16_t ca_cert_len, uint32_t timeout) {
  return -1;
}

int32_t tls_send(handle_t handle, void *buf, uint32_t len, uint32_t timeout) {
  return -1;
}

int32_t tls_recv(handle_t handle, void *buf, uint32_t len, uint32_t timeout) {
  return -1;
}

int32_t tls_disconnect(handle_t handle) { return -1; }
#endif
