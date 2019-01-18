//
// Created by Quentin Wendegass on 2019-01-08.
//

#ifndef SOCKET_TEST_RESPONSE_H
#define SOCKET_TEST_RESPONSE_H

#include <stdbool.h>
#include "header.h"
#include "body.h"



typedef struct httws_response{
    body_t body;
    httws_header * header;
    short status;
} httws_response;


/* Allocates a new response struct and initializes it.
 * The memory must be freed with httws_free_response().
 * NULL is returned in case of an error.
 * */
httws_response * httws_alloc_response();


/* Frees a response that was allocated with httws_alloc_response().
 * */
void httws_free_response(httws_response *response);


/* Parses the status line string and adds the status code to the response.
 * */
void httws_lib_set_response_status(httws_response *response, char *status_line);


/* Creates a new header struct from the header line string and adds it the the response.
 * Returns -1 if an error occurs and 0 on success.
 * */
int httws_lib_add_response_header(httws_response *response, char *header_line, size_t size);


#endif //SOCKET_TEST_RESPONSE_H
