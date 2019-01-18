//
// Created by Quentin Wendegass on 2019-01-10.
//

#ifndef HTTPLIB_HTTPLIB_H
#define HTTPLIB_HTTPLIB_H


#include "httws.h"
#include "request.h"
#include "response.h"


#define READ_BUFFER_SIZE 4096

#define ERR_NO_DATA_TRANSFERRED 10
#define ERR_OPERATION_NOT_PERMITTED 11
#define ERR_WRONG_AMOUNT_TRANSFERRED 12


/* Send a http request to the connected server.
 * Returns a pointer to the response.
 * When an error occurs NULL is returned and the error is saved to errno.
 * */
httws_response * httws_http_send(httws_request *request);


#endif //HTTPLIB_HTTPLIB_H
