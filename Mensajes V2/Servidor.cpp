//============================================================================
// Name        : Servidor.cpp
// Author      : 
// Version     :
// Copyright   : 
// Description : Servidor que envia mensjes a n cantidad de clientes.
//============================================================================

#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>//Socket
#include <arpa/inet.h>
#include <unistd.h> //Sleep

//Creacion del socket y de la conexion.

int main(int argv, char** argc){

	int host_port= 1101;
	char* host_name="127.0.0.1";

	struct sockaddr_in my_addr;

		char buffer[1024];
        char* mensaje = "Mensaje de Prueba";
		int bytecount;
		int buffer_len=0;
		int hsock;
		int * p_int;
		int err;

		hsock = socket(AF_INET, SOCK_STREAM, 0);
		
	if(hsock == -1){
		printf("Error al iniciar el Socket %d\n",errno);
		goto FINISH;
	}
	
	p_int = (int*)malloc(sizeof(int));
	*p_int = 1;
		
	if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
		(setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
		printf("Error al definir la configuracion %d\n",errno);
		free(p_int);
		goto FINISH;
	}
	free(p_int);

	my_addr.sin_family = AF_INET ;
	my_addr.sin_port = htons(host_port);
	
	memset(&(my_addr.sin_zero), 0, 8);
	my_addr.sin_addr.s_addr = inet_addr(host_name);

	if( connect( hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1 ){
		if((err = errno) != EINPROGRESS){
			fprintf(stderr, "Error al tratar de conectar con el socket %d\n", errno);
			goto FINISH;
		}
	}
//========Fin del codigo de conexion===========================================

//Envio de Informacion al cliente

	buffer_len = 1024;

	memset(buffer, '\0', buffer_len);

    //==== Input Opcional============================================
	//printf("Ingresar un mensaje y presionar ENTER\n");
	//fgets(buffer, 1024, stdin);
	buffer[strlen(buffer)-1]='\0';
	
	
	if( (bytecount=send(hsock, mensaje, strlen(mensaje),0))== -1){
		fprintf(stderr, "Error enviando el mensaje %d\n", errno);
		goto FINISH;
	}
	printf("Sent bytes %d\n", bytecount);

	if((bytecount = recv(hsock, buffer, buffer_len, 0))== -1){
		fprintf(stderr, "Error al recibir datos %d\n", errno);
		goto FINISH;
	}
	printf("bytes Recibidos %d\nMensaje Recibido \"%s\"\n", bytecount, buffer);

	close(hsock);
	
FINISH:
;
}
