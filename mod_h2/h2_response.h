/* Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __mod_h2__h2_response__
#define __mod_h2__h2_response__

/**
 * h2_response parses a HTTP/1.1 response into
 * - response status
 * - a list of header values
 * - a series of bytes that represent the response body alone, without
 *   any meta data, such as inserted by chunked transfer encoding.
 *
 * All data is allocated from the connection memory pool. Body data
 * is passed "through" into the given h2_bucket(s) and will not
 * cause allocations.
 */

typedef enum {
    H2_RESP_ST_STATUS_LINE, /* parsing http/1 status line */
    H2_RESP_ST_HEADERS,     /* parsing http/1 response headers */
    H2_RESP_ST_BODY,        /* transferring response body */
    H2_RESP_ST_DONE         /* complete response converted */
} h2_response_state_t;

struct h2_bucket;
struct h2_resp_head;
struct h2_response;

typedef void h2_response_state_change_cb(struct h2_response *resp,
                                         h2_response_state_t prevstate,
                                         void *cb_ctx);

typedef struct h2_response {
    int stream_id;
    conn_rec *c;
    h2_response_state_t state;
    
    h2_response_state_change_cb *state_cb;
    void *state_cb_ctx;
    
    int chunked;
    apr_size_t body_len;
    apr_size_t remain_len;
    struct h2_bucket *chunk_work;
    
    apr_size_t offset;
    struct h2_bucket *rawhead;
    
    const char *status;
    apr_array_header_t *hlines;
    
    struct h2_resp_head *head;
} h2_response;

h2_response *h2_response_create(int stream_id, conn_rec *c);
apr_status_t h2_response_destroy(h2_response *response);

void h2_response_set_state_change_cb(h2_response *resp,
                                     h2_response_state_change_cb *callback,
                                     void *cb_ctx);

apr_status_t h2_response_http_convert(struct h2_bucket *bucket,
                                      void *conv_ctrx,
                                      const char *data, apr_size_t len,
                                      apr_size_t *pconsumed);

struct h2_resp_head *h2_response_get_head(h2_response *resp);

#endif /* defined(__mod_h2__h2_response__) */