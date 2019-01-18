//
// Created by Quentin Wendegass on 2019-01-08.
//

#ifndef SOCKET_TEST_BODY_H
#define SOCKET_TEST_BODY_H

#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


/* Parameter with attribute and value for an url-encoded body.
 * The next parameter is the pointer to next (linked list).
 * When next is NULL this is the last parameter.
 * */
typedef struct httws_http_parameter {
    char * attribute;
    char * value;
    size_t size; // The size of the string after httws_lib_parameter_to_str()
    struct httws_http_parameter * next;
} httws_http_parameter;


typedef struct body {
    char * body;
    size_t size;
} body_t;


/* Adds a new parameter to the linked list.
 * The size of the parameter is calculated in this function, therefore it don't have to be set manually.
 * */
void httws_body_add_parameter(httws_http_parameter **head, httws_http_parameter *parameter);


/* Creates a url-encoded string from a single parameter.
 * The returned string must be freed manually.
 * If an error occurs NULL is returned and the error can be seen with errno.
 * */
char * httws_lib_parameter_to_str(httws_http_parameter *parameter);


/* Creates a new url-encoded body for all parameters in the linked list.
 * The body must either be freed manually.
 * If an error occurs NULL is returned and the error can be seen with errno.
 * On success the function returns the urlencoded body.
 * */
char * httws_body_urlencode_parameters(httws_http_parameter *head);


/* Frees the body string and sets all values to zero.
 * */
void httws_free_body(body_t *body);


#endif //SOCKET_TEST_BODY_H
