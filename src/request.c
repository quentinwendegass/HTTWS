//
// Created by Quentin Wendegass on 2019-01-08.
//

#include "request.h"


#define REQUEST_LINE_EXTRA_BITS 12



void httws_lib_set_request_line_size(httws_request_line *request_line){

    /* Calculate string size */
    size_t size = 0;
    size += strlen(request_line->method);
    size += strlen(request_line->path);
    size += REQUEST_LINE_EXTRA_BITS;

    request_line->size = size;
}


char * httws_lib_request_line_to_str(httws_request_line *request_line){
    char * request_line_str;

    if((request_line_str = malloc((request_line->size) + 1)) == NULL){
        return NULL;
    }

    // Copy the formatted string to the allocated memory
    sprintf(request_line_str, "%s %s HTTP/1.1\r\n", request_line->method, request_line->path);

    return request_line_str;
}


void httws_set_body(httws_request *request, char *body){
    request->body.body = body;
    request->body.size = strlen(body);
};


httws_request * httws_alloc_request(char *host){
    httws_request * request;
    if((request = malloc(sizeof(httws_request))) == NULL){
        return NULL;
    }
    /* Initialize the request */
    request->header = NULL;
    request->body.body = NULL;
    request->body.size = 0;

    /* Add host and date headers to request */
    httws_add_host_header(&request->header, host);
    httws_add_date_header(&request->header);

    return request;
}

void httws_free_request(httws_request *request){
    free(request);
}