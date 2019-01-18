//
// Created by Quentin Wendegass on 2019-01-07.
//

#ifndef SOCKET_TEST_HEADER_H
#define SOCKET_TEST_HEADER_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <time.h>



typedef struct httws_header{
    char * header;
    char * value;
    size_t size;
    struct httws_header * next; // Node to the next header (linked list)
} httws_header;



/* Calculates the size of one header as string and sets it in the struct.
 * Use this function for the first parameter.
 * */
void httws_lib_set_header_size(httws_header *header);


/* Adds a new header to the linked list.
 * The size of the header is calculated in this function,
 * therefore it don't have to be set with httws_lib_set_header_size().
 * Although the first header you create doesn't call this function so the size must be set manually.
 * */
void httws_add_header(httws_header **head, httws_header *header);


/* Adds the Date header to the header list with the current date and time.
 * */
void httws_add_date_header(httws_header **head);


/* Adds the host header to the header list.
 * */
void httws_add_host_header(httws_header **head, char *host);


/* Adds the Content-Length header to the header list.
 * */
void httws_add_content_length_header(httws_header **head, int length);


/* Creates a string from a single header.
 * The returned string must be freed manually.
 * If an error occurs NULL is returned and the error can be seen with errno.
 * */
char * httws_lib_header_to_str(httws_header *header);


/* Prints all header in the list.
 * */
void httws_print_headers(httws_header *head);


#endif //SOCKET_TEST_HEADER_H

