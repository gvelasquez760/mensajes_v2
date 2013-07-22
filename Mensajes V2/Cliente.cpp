//============================================================================
// Name        : Cliente.cpp
// Author      : 
// Version     :
// Copyright   : 
// Description : Cliente que recibe mensajes de Servidor.cpp y genera un nuevo 
//               hilo para cada uno. (g++ -o cliente Cliente.cpp -lpthread) 
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
#include <pthread.h> //Manejo de Hilos

//Creacion del socket y de la conexion.

void* SocketHandler(void*);

int main(int argv, char** argc){

	int host_port= 1101;

	struct sockaddr_in my_addr;

		int hsock;
		int * p_int ;
		int err;

		socklen_t addr_size = 0;
		int* csock;
		sockaddr_in sadr;
		pthread_t thread_id=0;
		hsock = socket(AF_INET, SOCK_STREAM, 0);
		
	if(hsock == -1){
		printf("Error iniciando socket %d\n", errno);
		goto FINISH;
	}
	
	p_int = (int*)malloc(sizeof(int));
	*p_int = 1;
		
	if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
		(setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
		printf("Error definiendo configuracion %d\n", errno);
		free(p_int);
		goto FINISH;
	}
	free(p_int);

	my_addr.sin_family = AF_INET ;
	my_addr.sin_port = htons(host_port);
	
	memset(&(my_addr.sin_zero), 0, 8);
	my_addr.sin_addr.s_addr = INADDR_ANY ;
	
	if( bind( hsock, (sockaddr*)&my_addr, sizeof(my_addr)) == -1 ){
		fprintf(stderr,"Error con el socket o puerto 1101 ocupado %d\n",errno);
		goto FINISH;
	}
	if(listen( hsock, 10) == -1 ){
		fprintf(stderr, "Error listening %d\n",errno);
		goto FINISH;
	}

//========Fin del codigo de conexion===========================================	

//Recepcion de el mensaje y retorno un "ECHO" al servidor

	addr_size = sizeof(sockaddr_in);
	
	while(true){
		printf("Listo para recibir mensajes\n");
		csock = (int*)malloc(sizeof(int));
		if((*csock = accept( hsock, (sockaddr*)&sadr, &addr_size))!= -1){
			printf("---------------------\nConexion recibida de %s\n",inet_ntoa(sadr.sin_addr));
			pthread_create(&thread_id,0,&SocketHandler, (void*)csock );
			pthread_detach(thread_id);
		}
		else{
			fprintf(stderr, "Error al conectar %d\n", errno);
		}
	}
	
FINISH:
;
}

//Recepcion de Mensajes por parte del cliente

void* SocketHandler(void* lp){
    int *csock = (int*)lp;

	char buffer[1024];
	int buffer_len = 1024;
	int bytecount;

	memset(buffer, 0, buffer_len);
	if((bytecount = recv(*csock, buffer, buffer_len, 0))== -1){
		fprintf(stderr, "Error al recibir el mensaje %d\n", errno);
		goto FINISH;
	}
	printf("bytes recibidos %d\nMensaje Recibido \"%s\"\n", bytecount, buffer);
	strcat(buffer, " ECHO");

	if((bytecount = send(*csock, buffer, strlen(buffer), 0))== -1){
		fprintf(stderr, "Error enviando el mensaje %d\n", errno);
		goto FINISH;
	}
	
	printf("bytes enviados %d\n", bytecount);


FINISH:
	free(csock);
    return 0;
}
