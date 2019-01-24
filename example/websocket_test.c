//
// Created by Quentin Wendegass on 2019-01-24.
//

#include <httws/httws.h>
#include <httws/websocket.h>


int main() {
    if(httws_connect("echo.websocket.org", "443", true) < 0){
        exit(-1);
    }

    httws_ws_handshake();

    char d[] = "My data";

    httws_ws_send(d, (unsigned short) strlen(d));

    char * res = httws_ws_resv();

    printf("Received: %s\n", res);

    free(res);

    httws_disconnect();
}