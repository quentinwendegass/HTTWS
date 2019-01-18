//
// Created by Quentin Wendegass on 2019-01-17.
//

#include "httws.h"


SSL_CTX * context;
SSL * ssl;

/* Converts the host and port to a single string in the format 'host:port' for OpenSSL.
 * */
int set_connection_str(char * host, char * port){
    size_t host_size = strlen(host);
    size_t port_size = strlen(port);

    /* Allocate memory for host, port, ':' and '\0' */
    if((connection = malloc(host_size + port_size + 2)) == NULL){
        return -1;
    }
    strcpy(connection, host);
    connection[host_size] = ':';
    strcpy(connection + host_size + 1, port);
    connection[host_size + port_size + 1] = '\0';
    return 0;
}


int httws_connect(char *host, char *port, bool is_ssl){
    if((set_connection_str(host, port)) < 0){
        fprintf(stderr, "Error: Error: Can't allocate memory for connection!\n");
        return -1;
    };
    /* Initializing OpenSSL */
    SSL_load_error_strings();
    ERR_load_BIO_strings();
    SSL_library_init();

    /* Setup the ssl bio */
    if(is_ssl == true){
        // Create a new SSL context fpr SSL v2 or v3
        context = SSL_CTX_new(SSLv23_client_method());

        /* Load certificates in default location */
        if(SSL_CTX_set_default_verify_paths(context) == 0){
            fprintf(stderr, "Error: Error: Can't load default verify paths!\n");
            printf("Reason: %s\n", ERR_reason_error_string(ERR_get_error()));
            return -1;
        }

        sbio = BIO_new_ssl_connect(context);
        BIO_get_ssl(sbio, &ssl);

        if(!ssl){
            fprintf(stderr, "Error: Can't locate SSL pointer\n");
            return -1;
        }

        //Don't want any retries
        SSL_set_mode(ssl, SSL_MODE_AUTO_RETRY);

        /* TODO: I probably want to do some other configuration here to in the future */

        BIO_set_conn_hostname(sbio, connection);
    }
        /* Setup the non ssl bio */
    else{
        sbio = BIO_new_connect(connection);
    }

    /* Create a buffer bio and push it in front of the (ssl) bio */
    bbio = BIO_new(BIO_f_buffer());
    BIO_set_read_buffer_size(bbio, READ_BUFFER_SIZE);
    BIO_push(bbio, sbio);

    /* Try to connect with the server */
    if(BIO_do_connect(sbio) <= 0){
        fprintf(stderr, "Error: Error: Can't load default verify paths!\n");
        ERR_print_errors_fp(stderr);
        return -1;
    }

    /* Verify the ssl certificate */
    if(is_ssl == true){
        if(SSL_get_verify_result(ssl) != X509_V_OK)
        {
            fprintf(stderr, "No valid certificate!\n");
            return -1;
        }
    }

    return 0;
}


void httws_disconnect(){
    if(bbio)
        BIO_free_all(bbio);
    if(context != NULL)
        SSL_CTX_free(context);
    if(connection != NULL){
        free(connection);
    }
}
