//
// Created by Quentin Wendegass on 2019-01-07.
//

#include "body.h"


#define PARAMETER_EXTRA_BITS 1



/* Calculates the size of the parameter as string and sets it in the struct.
 * Use this function for the first parameter.
 * */
void set_parameter_size(httws_http_parameter * parameter){
    /* Calculate the size of the parameter */
    size_t size = 0;
    size += strlen(parameter->attribute);
    size += strlen(parameter->value);
    size += PARAMETER_EXTRA_BITS; // Defines the '=' in the string
    parameter->size = size;
}


void httws_body_add_parameter(httws_http_parameter **head, httws_http_parameter *parameter){
    // Set the current head parameter as next parameter
    parameter->next = *head;

    // Set the new parameter as new head
    *head = parameter;
}


char * httws_lib_parameter_to_str(httws_http_parameter *parameter){

    /* Allocate memory for the parameter string
     * Must be freed after using the string */
    char * param_str;
    if((param_str = malloc(parameter->size + 1)) == NULL){
        return NULL;
    }

    // Keep the head of the string
    char * param_str_start = param_str;

    /* Copy the parameter with the format 'attribute=value' to the string */
    strcpy(param_str, parameter->attribute);
    param_str += strlen(parameter->attribute);
    *param_str = '=';
    param_str++;
    strcpy(param_str, parameter->value);

    return param_str_start;
}


char * httws_body_urlencode_parameters(httws_http_parameter *head){
    httws_http_parameter * current = head;
    size_t size = 0;

    /* Calculate the size of the body */
    while (current != NULL) {
        set_parameter_size(current);
        size += current->size;
        size += 1; // For '&' between parameters
        current = current->next;
    }

    // Decrease the size by one to remove the last '&' that gets set above
    size--;


    char * body_str;

    /* Allocate memory for the body string
     * Must be freed after using it */
    if((body_str = malloc(size + 1)) == NULL){
        return NULL;
    }

    // Keep the start of the string
    char * body_str_start = body_str;

    current = head;

    /* Fill the body string. If only one parameter is in this list, the loop is skipped. */
    while (current->next != NULL) {
        char * param_str;
        if((param_str = httws_lib_parameter_to_str(head)) == NULL){
            free(body_str_start);
            return NULL;
        }
        strcpy(body_str, param_str);
        free(param_str); // Free the memory after the string is copied
        body_str += current->size;
        *body_str = '&';
        body_str++;

        current = current->next;
    }

    /* Add the last parameter extra, so '&' is not appended to the end of the body */
    char * param_str;
    if((param_str = httws_lib_parameter_to_str(current)) == NULL){
        free(body_str_start);
        return NULL;
    }
    strcpy(body_str, param_str);
    free(param_str);

    return body_str_start;
}


void httws_free_body(body_t *body){
    if(body->body != NULL){
        free(body->body);
        body->body = NULL;
        body->size = 0;
    }
}


