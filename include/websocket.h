//
// Created by Quentin Wendegass on 2019-01-17.
//

//
// Currently only a text payload, 16 bit payload length extend and single frames are supported.
//


#ifndef HTTPLIB_WEBSOCKET_H
#define HTTPLIB_WEBSOCKET_H


#include "http.h"


#define ERR_PROTOCOL_NOT_SUPPORTED 20
#define ERR_NOT_SUPPORTED 21


/* Makes the http handshake for the websocket upgrade.
 * Returns 0 on success and -1 if an error occurs. The error is saved in errno.
 * */
int httws_ws_handshake();


/* Send a message to the ws server.
 * Returns 0 on success and -1 if an error occurs. The error is saved in errno.
 * */
int httws_ws_send(char *data, unsigned short size);


/* Receives a message to the ws server.
 * Returns 0 on success and -1 if an error occurs. The error is saved in errno.
 * */
char * httws_ws_resv();


#endif //HTTPLIB_WEBSOCKET_H
