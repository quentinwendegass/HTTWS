//
// Created by Quentin Wendegass on 2019-01-08.
//

#ifndef SOCKET_TEST_REQUEST_H
#define SOCKET_TEST_REQUEST_H

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

#include "header.h"
#include "body.h"


/* This struct can be filled through the request struct */
typedef struct httws_request_line{
    char * method;
    char * path;
    size_t size; // Size of the request line string. Must not be set manually.
} httws_request_line;

typedef struct httws_request{
    httws_request_line request_line;
    body_t body;
    httws_header * header;
} httws_request;



/* Allocates a new request struct and initializes it.
 * The memory must be freed with httws_free_request().
 * NULL is returned in case of an error.
 * */
httws_request * httws_alloc_request(char *host);


/* Frees a request that was allocated with httws_alloc_request().
 * */
void httws_free_request(httws_request *request);


/* Add body string to the request.
 * */
void httws_set_body(httws_request *request, char *body);


/* Sets the size attribute in the request line struct,
 * to the size it would have as a string.
 * In most cases it mustn't be called manually, because it is called by request_to_str().
 * */
void httws_lib_set_request_line_size(httws_request_line *request_line);


/* Returns the request line as a string.
 * If no memory could be allocated it returns a NULL pointer.
 * The request line string must be freed manually.
 * */
char * httws_lib_request_line_to_str(httws_request_line *request_line);




#endif //SOCKET_TEST_REQUEST_H
