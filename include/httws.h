//
// Created by Quentin Wendegass on 2019-01-17.
//

#ifndef HTTPLIB_HTTWS_H
#define HTTPLIB_HTTWS_H

#include <stdbool.h>
#include "openssl/bio.h"
#include "openssl/ssl.h"
#include "openssl/err.h"
#include "websocket.h"


// SSL and Buffer BIO.
BIO * sbio, * bbio;

// Host and port as string 'host:port'
char * connection;


/* Initialize a connection to the server.
 * On success 0 is returned, on an error -1.
 * */
int httws_connect(char *host, char *port, bool is_ssl);


/* Must be called after httws_connect() to free memory.
 * */
void httws_disconnect();


#endif //HTTPLIB_HTTWS_H
