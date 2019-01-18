//
// Created by Quentin Wendegass on 2019-01-10.
//


#include "http.h"


httws_response * httws_http_send(httws_request *request){

    // Adds the content type header for the body
    httws_add_content_length_header(&request->header, (int) request->body.size);

    /* Send the request line */
    httws_lib_set_request_line_size(&request->request_line);
    char * req_line_str;
    if((req_line_str = httws_lib_request_line_to_str(&request->request_line)) == NULL){
        return NULL;
    }
    int len = BIO_puts(sbio, req_line_str);

    free(req_line_str);

    /* Check if the request line was transferred successfully*/
    if(len < 1){
        errno = ERR_NO_DATA_TRANSFERRED;
        if(len == -2){
            errno = ERR_OPERATION_NOT_PERMITTED;
        }
        return NULL;
    }

    /* Send the header lines */
    httws_header * current = request->header;
    while(current != NULL){
        char * header_str = httws_lib_header_to_str(current);
        len = BIO_puts(sbio, header_str);
        free(header_str);

        if(len < 1){
            errno = ERR_NO_DATA_TRANSFERRED;
            if(len == -2){
                errno = ERR_OPERATION_NOT_PERMITTED;
            }
            return NULL;
        }
        current = current->next;
    }
    len = BIO_puts(sbio, "\r\n");
    if(len < 1){
        errno = ERR_NO_DATA_TRANSFERRED;
        if(len == -2){
            errno = ERR_OPERATION_NOT_PERMITTED;
        }
        return NULL;
    }


    /* Send the body if one is present */
    if(request->body.body != NULL){
        int bsize = BIO_write(sbio, request->body.body, (int) request->body.size);
        if(bsize < 1){
            errno = ERR_NO_DATA_TRANSFERRED;
            if(bsize == -2){
                errno = ERR_OPERATION_NOT_PERMITTED;
            }
            return NULL;
        }else if(bsize != request->body.size){
            errno = ERR_WRONG_AMOUNT_TRANSFERRED;
            return NULL;
        }
    }

    httws_response * response;
    if((response = httws_alloc_response()) == NULL){
        return NULL;
    }

    static char recv_buffer[4096]; // 4kB max header and status line length

    /* Get status line */
    len = BIO_gets(bbio, recv_buffer, 4096);
    if(len < 1){
        errno = ERR_NO_DATA_TRANSFERRED;
        if(len == -2){
            errno = ERR_OPERATION_NOT_PERMITTED;
        }
        return NULL;
    }
    httws_lib_set_response_status(response, recv_buffer);

    int content_size = 0; // Stores the content size for the body

    /* Get all header lines and store them in the response */
    while ((len = BIO_gets(bbio, recv_buffer, 4096)) > 2){
        if(len < 1){
            errno = ERR_NO_DATA_TRANSFERRED;
            if(len == -2){
                errno = ERR_OPERATION_NOT_PERMITTED;
            }
            return NULL;
        }
        httws_lib_add_response_header(response, recv_buffer, (size_t) len);

        /* Look for the Content-Size header and save it as int*/
        if(strcmp(response->header->header, "Content-Length") == 0){
            content_size = atoi(response->header->value);
        }
    }

    if(content_size > 0){
        char * body;
        if((body = malloc((size_t) content_size + 1)) == NULL){
            errno = ENOMEM;
            return NULL;
        }

        /* Get the body */
        len = BIO_read(bbio, body, content_size);
        if(len < 1){
            errno = ERR_NO_DATA_TRANSFERRED;
            if(len == -2){
                errno = ERR_OPERATION_NOT_PERMITTED;
            }
            return NULL;
        }else if(len != content_size){
            errno = ERR_WRONG_AMOUNT_TRANSFERRED;
            return NULL;
        }
        body[content_size] = '\0';

        response->body.size = (size_t) content_size;
        response->body.body = body;
    }

    return response;
}
