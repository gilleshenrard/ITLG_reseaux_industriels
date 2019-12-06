/*
** protocol.c
** Library regrouping protocol-based functions
** ------------------------------------------
** Made by Gilles Henrard
** Last modified : 06/12/2019
*/
#include "protocol.h"

int prcv(int sockfd, void* structure)
{
    unsigned char serialised[MAXDATASIZE] = {0};
    char buffer[MAXDATASIZE] = {0};
	head_t header = {0};
	meta_t* lis = NULL;
	int bufsz = 0, *fd = NULL;
	uint64_t received = 0, size = 0;

	//wait for the header containing the data info
    if (receiveData(sockfd, serialised, sizeof(head_t), NULL, 1) == -1)
        return -1;

    unpack(serialised, HEAD_F, &header.nbelem, &header.stype, &header.szelem);
    memset(&serialised, 0, sizeof(serialised));

    //unpack all the data sent by the server and store it in a linked list
    size = header.nbelem * header.szelem;
    while(received < size)
    {
        //receive message from the server
        if ((bufsz = receiveData(sockfd, buffer, header.szelem, NULL, 1)) == -1)
            return -1;

        //unpack the data and store it
        switch(header.stype)
        {
            case SLIST:
                lis = (meta_t*)structure;
                insertListSorted(lis, buffer);
                break;

            case SFILE:
                fd = (int*)structure;
                bufsz = write(*fd, serialised, bufsz);
                break;

            case SSTRING:
                strcpy((char*)structure, (char*)buffer);
                break;

            default:
                break;
        }

        //clear the memory buffers
        memset(&buffer, 0, sizeof(serialised));
        received += bufsz;
    }

    //prepare the reply header to be sent
    header.nbelem = 1;
    header.szelem = received;
    memset(serialised, 0, sizeof(serialised));
    pack(serialised, HEAD_F, header.nbelem, header.stype, header.szelem);
    bufsz = sizeof(head_t);

    //send it to the server
    sendData(sockfd, serialised, &bufsz, NULL, 1);

    return 0;
}
