/*
** protocol.c
** Library regrouping protocol-based functions
** ------------------------------------------
** Made by Gilles Henrard
** Last modified : 15/12/2019
*/
#include "protocol.h"


/************************************************************************/
/*  I : socket from which receive data                                  */
/*      structure to which add the data (file, list, string buffer, ...)*/
/*  P : Follow the established protocol on the receiver side:           */
/*          1- receive the header indicating how many bytes and type to */
/*              receive                                                 */
/*          2- receive the data and store it in the data structure      */
/*          3- send an acknowledge header with the actual bytes amount  */
/*              received                                                */
/*  O : -1 if error                                                     */
/*      0 otherwise                                                     */
/************************************************************************/
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

    //unpack all the data sent by the sender and store it in a linked list
    size = header.nbelem * header.szelem;
    while(received < size)
    {
        //receive message from the sender
        if ((bufsz = receiveData(sockfd, buffer, header.szelem, NULL, 1)) == -1)
            return -1;

        //unpack the data and store it
        switch(header.stype)
        {
            case SLIST: // receive a list
                lis = (meta_t*)structure;
                insertListSorted(lis, buffer);
                break;

            case SFILE: // receive a file
                fd = (int*)structure;
                bufsz = write(*fd, serialised, bufsz);
                break;

            case SSTRING: // receive a string
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

    //send it to the sender
    sendData(sockfd, serialised, &bufsz, NULL, 1);

    return 0;
}

/************************************************************************/
/*  I : socket to which send data                                       */
/*      structure from  which read the data (file, list, ...)           */
/*      header describing the data structure                            */
/*      action to perform to send list elements (can be NULL)           */
/*  P : Follow the established protocol on the sender side:             */
/*          1- send the header indicating how many bytes and type to    */
/*              receive                                                 */
/*          2- read the data structure and send it                      */
/*          3- receive an acknowledgement header with the actual bytes  */
/*              amount received                                         */
/*  O : -1 if error                                                     */
/*      0 otherwise                                                     */
/************************************************************************/
int psnd(int sockfd, void* structure, head_t* header, int (*doSendList)(void*,void*))
{
    unsigned char serialised[MAXDATASIZE] = {0};
    char* buffer = NULL;
    int bufsz=0, ret=0, *fd=NULL;
    uint64_t sent = 0, size = 0;
    meta_t *lis = NULL;

    //serialize the header and send it to the receiver
    pack(serialised, HEAD_F, header->nbelem, header->stype, header->szelem);
    bufsz = sizeof(head_t);
    if(sendData(sockfd, serialised, &bufsz, NULL, 1) == -1)
        return -1;

    switch(header->stype)
    {
        case SFILE:
            //send the whole requested data
            fd = (int*)structure;
            size = header->nbelem * header->szelem;
            while(sent < size)
            {
                ret = read(*fd, serialised, sizeof(serialised));
                if (ret == -1)
                {
                    close(*fd);
                    return -1;
                }

                //send the data
                if(sendData(sockfd, serialised, &ret, NULL, 1) == -1)
                {
                    close(*fd);
                    return -1;
                }

                //wipe the buffer
                memset(serialised, 0, sizeof(serialised));
                sent += ret;
            }
            break;

        case SLIST:
            lis = (meta_t*)structure;
            //send the whole list to the client
            if(foreachList(lis, &sockfd, doSendList) == -1)
                return -1;

            size = lis->nbelements * lis->elementsize;
            break;

        case SSTRING:
            buffer = (char*)structure;
            if(sendData(sockfd, buffer, (int*)&header->szelem, NULL, 1) == -1)
                return -1;

            size = strlen(buffer);
            break;
    }

    //reset the header and receive the receiver's reply
    memset(serialised, 0, sizeof(serialised));
    receiveData(sockfd, serialised, sizeof(head_t), NULL, 1);
    unpack(serialised, HEAD_F, &header->nbelem, &header->stype, &header->szelem);

    //check if it matches the one sent by the receiver
    if(header->szelem != size)
        return -1;

    return 0;
}
