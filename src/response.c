//
// Created by Quentin Wendegass on 2019-01-08.
//

#include "response.h"


httws_response * httws_alloc_response(){
    httws_response * response;
    if((response = malloc(sizeof(httws_response))) == NULL){
        return NULL;
    }
    response->header = NULL;
    response->body.body = NULL;
    response->body.size = 0;

    return response;
}

void httws_free_response(httws_response *response){
    /*
     * Free header strings
     */

    httws_header * current = response->header;

    while(current != NULL){
        free(current->header);
        free(current->value);
        httws_header * header = current;
        current = current->next;
        free(header);
    }

    // Set the head to a NULL pointer
    response->header = NULL;

    httws_free_body(&response->body);
    free(response);
}


/* Sets the pointer of a string to the first given character that occurs.
 * */
void iterate_to_char(char ** str, char c){
    // Iterate to the given char
    while(**str != c){
        (*str)++;
    }
}


/* Converts the first three characters in the string to a number and returns it.
 * */
short str_to_status(char * str){
    /* Copy the first three characters to a new string to terminate it with \0 */
    char status_code_str[4];
    memcpy(status_code_str, str , 3);
    status_code_str[3] = '\0';

    return (short) atoi(status_code_str);  // Convert the string to a number
}


void httws_lib_set_response_status(httws_response *response, char *status_line){
    char * current = status_line;

    iterate_to_char(&current, ' ');
    current++;

    response->status = str_to_status(current);
}

int httws_lib_add_response_header(httws_response *response, char *header_line, size_t size){

    char * header_end = header_line;
    size_t header_size, value_size;

    // Iterate to the end of the header
    iterate_to_char(&header_end, ':');

    // Calculate the header size without \0 terminator
    header_size = (size_t) (header_end - header_line);

    /* Allocate memory for the header and copy the string over */
    char * header;
    if((header = malloc(header_size + 1)) == NULL){
        return -1;
    };
    memcpy(header, header_line, header_size);
    header[header_size] = '\0';

    // Calculate the size in bytes for the value
    value_size = (size_t) ((header_line + size - 2) - (header_end + 2));

    char * value;
    if((value = malloc(value_size + 1)) == NULL){
        return -1;
    };

    // Copy the value from the header line
    memcpy(value, header_end + 2, value_size);
    value[value_size] = '\0';

    /* Create a new header and add it to the response*/
    httws_header * h = malloc(sizeof(httws_header));
    h->header = header;
    h->value = value;
    h->size = size;

    httws_add_header(&response->header, h);

    return 0;
}
