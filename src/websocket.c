//
// Created by Quentin Wendegass on 2019-01-17.
//

#include "websocket.h"


#define FIN_MASK 0x80
#define OP_CODE_MASK 0x0F
#define MASK_MASK 0x80
#define PAYLOAD_LEN_MASK 0x7F


// The mask key which the payload is masked on send
char mask_key[4] = "abcd";


/* Union to access single bytes of the length */
union payload_len{
    char len_c[2];
    unsigned short len_s;
};


int httws_ws_handshake(){
    /*
     * Create a new request with the needed headers
     */
    httws_request * handshake_request;
    if((handshake_request = httws_alloc_request(connection)) == NULL){
        return -1;
    }

    httws_header upgrade, connection, ws_key, ws_ver;

    upgrade.header = "Upgrade";
    upgrade.value = "websocket";

    connection.header = "Connection";
    connection.value = "Upgrade";

    ws_key.header = "Sec-WebSocket-Key";
    ws_key.value = "dGhlIHNhbXBsZSBub25jZQ==";

    ws_ver.header = "Sec-WebSocket-Version";
    ws_ver.value = "13";

    handshake_request->request_line.path = "/";
    handshake_request->request_line.method = "GET";

    httws_add_header(&handshake_request->header, &upgrade);
    httws_add_header(&handshake_request->header, &connection);
    httws_add_header(&handshake_request->header, &ws_key);
    httws_add_header(&handshake_request->header, &ws_ver);

    /* Send the request to the remote server */
    httws_response * handshake_response;
    if((handshake_response = httws_http_send(handshake_request)) == NULL){
        return -1;
    }

    /* Check if the server upgrades the protocol to ws */
    if(handshake_response->status != 101){
        errno = ERR_PROTOCOL_NOT_SUPPORTED;
        return -1;
    }

    // TODO: Validate Sec-WebSocket-Accept header

    httws_free_request(handshake_request);
    httws_free_response(handshake_response);

    return 0;
}


int httws_ws_send(char *data, unsigned short size){

    /*
     * 1 bit fin
     * 3 bit rsv
     * 4 bit opcode: 0x1 -> text frame, 0x2 -> binary frame, 0x8 -> connection close
     *
     * 1 bit mask
     * 7 bit payload len: 126 -> 16 bit payload extend
     * 16 bit payload len extend: x
     *
     * 32 bit masking key: only if mask is 1
     *
     * x bit payload data
     */

    unsigned char mask_index = 2;
    union payload_len data_len;
    data_len.len_s = size;

    // The header for the websocket protocol
    char frame_header[8];

    frame_header[0] = (char) 0x81; // Always send a full frame with text data

    /* Set the payload length in the header */
    if(data_len.len_s < 126){
        frame_header[1] = (char)(MASK_MASK | (char) data_len.len_s);
    }else{
        frame_header[1] = (char)(MASK_MASK | 126);
        /* Bytes must be switched */
        frame_header[2] = data_len.len_c[1];
        frame_header[3] = data_len.len_c[0];
        mask_index = 4;
    }

    /* Add the mask key to the header */
    for(int i = 0; i < 4; i++){
        frame_header[mask_index + i] = mask_key[i];
    }


    /* Mask the payload
     * j = i MOD 4
     * masked[i] = data[i] XOR mask[j]
     */
    char * masked_payload;
    if((masked_payload = malloc(data_len.len_s)) == NULL){
        return -1;
    }
    int i;
    for(i = 0; i < data_len.len_s; i++){
        masked_payload[i] = data[i] ^ mask_key[i % 4];
    }

    /* Send the ws header to the server */
    int written = BIO_write(sbio, frame_header, mask_index + 4);
    if(written < 1){
        errno = ERR_NO_DATA_TRANSFERRED;
        if(written == -2){
            errno = ERR_OPERATION_NOT_PERMITTED;
        }
        free(masked_payload);
        return -1;
    }else if(written != (mask_index + 4)){
        errno = ERR_WRONG_AMOUNT_TRANSFERRED;
        free(masked_payload);
        return -1;
    }

    /* Send the payload to the server */
    written = BIO_write(sbio, masked_payload, data_len.len_s);
    if(written < 1){
        errno = ERR_NO_DATA_TRANSFERRED;
        if(written == -2){
            errno = ERR_OPERATION_NOT_PERMITTED;
        }
        free(masked_payload);
        return -1;
    }else if(written != data_len.len_s){
        errno = ERR_WRONG_AMOUNT_TRANSFERRED;
        free(masked_payload);
        return -1;
    }

    free(masked_payload);

    return 0;
}


char * httws_ws_resv(){

    // TODO: Check for closing frame and send a closing frame back

    unsigned char header[2];
    unsigned char masking_key[4];

    /* Read the first 16 bit of the header */
    int read = BIO_read(sbio, header, 2);
    if(read < 1){
        errno = ERR_NO_DATA_TRANSFERRED;
        if(read == -2){
            errno = ERR_OPERATION_NOT_PERMITTED;
        }
        return NULL;
    }

    /*
     * Currently no continuation frames, only text data and unmasked payloads are possible.
     */
    int fin = header[0] & FIN_MASK;
    if(!fin){
        errno = ERR_NOT_SUPPORTED;
        return NULL;
    }

    int opcode = header[0] & OP_CODE_MASK;
    if(opcode != 1){
        errno = ERR_NOT_SUPPORTED;
        return NULL;
    }

    int masked = header[1] & MASK_MASK;
    if(masked){
        errno = ERR_NOT_SUPPORTED;
        return NULL;
    }


    /* Get the payload length */
    union payload_len len;
    len.len_s = (unsigned short) (header[1] & PAYLOAD_LEN_MASK);

    /* Check if extended payload length is needed.
     * Only 16 bit extended payload is supported. */
    if(len.len_s == 126){
        /* Read the extended payload length byte by byte */
        read = BIO_read(sbio, &len.len_c[1], 1);
        if(read < 1){
            errno = ERR_NO_DATA_TRANSFERRED;
            if(read == -2){
                errno = ERR_OPERATION_NOT_PERMITTED;
            }
            return NULL;
        }
        read = BIO_read(sbio, &len.len_c[0], 1);
        if(read < 1){
            errno = ERR_NO_DATA_TRANSFERRED;
            if(read == -2){
                errno = ERR_OPERATION_NOT_PERMITTED;
            }
            return NULL;
        }
    }else if(len.len_s == 127){
        errno = ERR_NOT_SUPPORTED;
        return NULL;
    }

    // TODO: Support masking on recv frames

    /*if(masked){
        read = BIO_read(sbio, masking_key, 4);
        if(read < 1){
            errno = ERR_NO_DATA_TRANSFERRED;
            if(read == -2){
                errno = ERR_OPERATION_NOT_PERMITTED;
            }
            return NULL;
        }
    }*/

    char * payload;
    if ((payload = malloc((size_t) len.len_s + 1)) == NULL){
        return NULL;
    }
    payload[len.len_s] = '\0';

    /* Get the payload */
    BIO_read(sbio, payload, len.len_s);
    if(read < 1){
        errno = ERR_NO_DATA_TRANSFERRED;
        if(read == -2){
            errno = ERR_OPERATION_NOT_PERMITTED;
        }
        return NULL;
    }

    return payload;
}