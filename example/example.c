//
// Created by Quentin Wendegass on 2019-01-10.
//

#include <httws/httws.h>
#include <httws/websocket.h>
#include <httws/http.h>


void test_http(){
    httws_request * request = httws_alloc_request("postman-echo.com");

    httws_header content_type_header;
    content_type_header.header = "Content-Type";
    content_type_header.value = "application/x-www-form-urlencoded; charset=utf-8";

    httws_add_header(&request->header, &content_type_header);

    request->request_line.method = "POST";
    request->request_line.path = "/post";

    httws_http_parameter param1, param2;

    param1.attribute = "TestParameter";
    param1.value = "TestValue";
    param1.next = NULL;

    param2.attribute = "TestParameter2";
    param2.value = "TestValue2";

    httws_http_parameter * param_head = &param1;
    httws_body_add_parameter(&param_head, &param2);

    char * body = httws_body_urlencode_parameters(param_head);

    httws_set_body(request, body);


    if(httws_connect("postman-echo.com", "443", true) < 0){
        exit(-1);
    }


    httws_response * res = httws_http_send(request);

    printf("Status: %d\n", res->status);
    printf("\nHeader:\n");
    httws_print_headers(res->header);
    if(res->body.body != NULL){
        printf("\nBody:\n");
        printf("%s\n", res->body.body);
    }

    httws_free_body(&request->body);
    httws_free_response(res);
    httws_free_request(request);

    httws_disconnect();
}


/* Not working example!
 * Just to showcase how to use websocket. */
void test_websocket(){
    if(httws_connect("host", "443", true) < 0){
        exit(-1);
    }

    httws_ws_handshake();

    char d[] = "data to send";

    httws_ws_send(d, (unsigned short) strlen(d));

    char * res = httws_ws_resv();

    printf("%s\n", res);

    free(res);

    httws_disconnect();
}



int main() {
    test_http();
}