//
// Created by Quentin Wendegass on 2019-01-07.
//

#include "header.h"


#define HEADER_EXTRA_BITS 4
#define TIME_HEADER_LENGTH 30



void httws_lib_set_header_size(httws_header *header){

    /* Calculate the size of the header line after httws_lib_header_to_str() */
    size_t size = 0;
    size += strlen(header->header);
    size += strlen(header->value);

    // Add the other chars to the size that are needed for the header line
    size += HEADER_EXTRA_BITS;

    header->size = size;
}


void httws_add_header(httws_header **head, httws_header *header){
    httws_lib_set_header_size(header);

    // Set the current head header node as next node
    header->next = *head;

    // Set the new header as new head
    *head = header;
}


void httws_add_content_length_header(httws_header **head, int length){
    static httws_header content_length_header;
    static char value_dummy[5];

    // Convert the body size to a string
    sprintf(value_dummy, "%d", length);

    content_length_header.header = "Content-Length";
    content_length_header.value = value_dummy;

    httws_add_header(head, &content_length_header);
}


void httws_add_date_header(httws_header **head){
    static httws_header date_header;
    static char header_str[] = "Date";
    static char value_str[TIME_HEADER_LENGTH];

    /* Use the right syntax for the date header:
     * <day-name>, <day> <month> <year> <hour>:<minute>:<second> GMT
     * */
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    t->tm_hour -= t->tm_gmtoff / 3600; // Set to GMT time

    strftime(value_str, TIME_HEADER_LENGTH, "%a, %d %b %Y %H:%M:%S GMT", t);

    /* Set the header */
    date_header.header = header_str;
    date_header.value = value_str;

    // Add the header to the list
    httws_add_header(head, &date_header);
}

void httws_add_host_header(httws_header **head, char *host){
    static httws_header host_header;
    static char header_str[] = "Host";

    host_header.header = header_str;
    host_header.value = host;

    httws_add_header(head, &host_header);
}


char * httws_lib_header_to_str(httws_header *header){
    /* Allocate memory for the header
     * Must be freed manually */
    char * header_line;
    if((header_line = malloc(header->size + 1)) == NULL){ // size + 1 because of the string terminator
        return NULL;
    }

    // Copy the formatted string the the newly allocated memory
    sprintf(header_line, "%s: %s\r\n", header->header, header->value);

    return header_line;
}


void httws_print_headers(httws_header *head) {

    httws_header * current = head;

    while(current != NULL){
        printf("%s: %s\n", current->header, current->value);
        current = current->next;
    }
}

/*
void test_header(){
    header_t content_type_header;
    header_t * head = &content_type_header;

    content_type_header.header = "Content-Type";
    content_type_header.value = "application/x-www-form-urlencoded";
    content_type_header.next = NULL;
    httws_lib_set_header_size(&content_type_header);

    httws_add_content_length_header(&head, 11);
    httws_add_date_header(&head);

    httws_print_headers(head);

    char * header_str = httws_lib_header_to_str(head);

    printf("\nTo String: %s\n", header_str);

    free(header_str);
}*/
