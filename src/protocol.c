/*
** protocol.c
** Library regrouping protocol-based functions
** ------------------------------------------
** Made by Gilles Henrard
** Last modified : 16/12/2019
*/
#include "protocol.h"


/************************************************************************/
/*  I : socket from which receive data                                  */
/*      structure to which add the data (file, list, string buffer, ...)*/
/*      function to print errors (can be NULL)                          */
/*  P : Follow the established protocol on the receiver side:           */
/*          1- receive the header indicating how many bytes and type to */
/*              receive                                                 */
/*          2- receive the data and store it in the data structure      */
/*          3- send an acknowledge header with the actual bytes amount  */
/*              received                                                */
/*  O : -1 if error                                                     */
/*      0 otherwise                                                     */
/************************************************************************/
int prcv(int sockfd, void* structure, void (*doPrint)(char*, ...))
{
    unsigned char serialised[MAXDATASIZE] = {0};
    char buffer[MAXDATASIZE] = {0};
	head_t header = {0};
	meta_t* lis = NULL;
	int ret = 1, *fd = NULL;
	uint64_t received = 0, size = 0;

	//wait for the header containing the data info
    if (receiveData(sockfd, serialised, sizeof(head_t), NULL, 1) == -1)
    {
        if(doPrint)
            (*doPrint)("prcv: error while receiving the data header");
        return -1;
    }

    //deserialise it and set the header
    unpack(serialised, HEAD_F, &header.nbelem, &header.stype, &header.szelem);
    memset(&serialised, 0, sizeof(serialised));

    //unpack all the data sent by the sender and store it in the right structure
    size = header.nbelem * header.szelem;
    while(received < size && ret > 0)
    {
        //receive package from the sender
        if ((ret = receiveData(sockfd, buffer, (header.szelem < sizeof(buffer) ? header.szelem : sizeof(buffer)), NULL, 1)) == -1)
        {
            if(doPrint)
                (*doPrint)("prcv: error while receiving the data");
        }

        //unpack the data and store it
        switch(header.stype)
        {
            case SLIST: // receive a list
                lis = (meta_t*)structure;
                if(insertListSorted(lis, buffer) == -1)
                {
                    if(doPrint)
                        (*doPrint)("prcv: error while inserting data in the list");

                    ret = -1;
                }
                break;

            case SFILE: // receive a file
                fd = (int*)structure;
                if(write(*fd, buffer, ret) != ret)
                {
                    if(doPrint)
                        (*doPrint)("prcv: writing in the file: %s", strerror(errno));

                    ret = -1;
                }
                break;

            case SSTRING: // receive a string
                strcpy((char*)structure, (char*)buffer);
                break;

            default:
                break;
        }

        if(ret != -1)
        {
            //clear the memory buffers
            memset(&buffer, 0, sizeof(buffer));
            received += ret;
        }
    }

    //prepare the reply header to be sent
    header.nbelem = 1;
    header.szelem = (ret == -1 ? 0 : received);
    memset(serialised, 0, sizeof(serialised));
    pack(serialised, HEAD_F, header.nbelem, header.stype, header.szelem);
    size = sizeof(head_t);

    //send it to the sender
    if(sendData(sockfd, serialised, (int*)&size, NULL, 1) == -1)
    {
        if(doPrint)
            (*doPrint)("prcv: error while sending acknowledgement header");

        return -1;
    }

    //return data transmission status
    return ret;
}

/************************************************************************/
/*  I : socket to which send data                                       */
/*      structure from  which read the data (file, list, ...)           */
/*      header describing the data structure                            */
/*      function to print error messages (can be NULL)                  */
/*  P : Follow the established protocol on the sender side:             */
/*          1- send the header indicating how many bytes and type to    */
/*              receive                                                 */
/*          2- read the data structure and send it                      */
/*          3- receive an acknowledgement header with the actual bytes  */
/*              amount received                                         */
/*  O : -1 if error                                                     */
/*      0 otherwise                                                     */
/************************************************************************/
int psnd(int sockfd, void* structure, head_t* header, void (*doPrint)(char*, ...))
{
    unsigned char serialised[MAXDATASIZE] = {0};
    char* buffer = NULL;
    int ret=0, *fd=NULL;
    uint64_t sent = 0, size = 0;
    meta_t *lis = NULL;
    dyndata_t* tmp = NULL;

    //serialize the header and send it to the receiver
    pack(serialised, HEAD_F, header->nbelem, header->stype, header->szelem);
    ret = sizeof(head_t);
    if(sendData(sockfd, serialised, &ret, NULL, 1) == -1)
    {
        if(doPrint)
            (*doPrint)("psnd: error while sending the data header");

        return -1;
    }

    //send the actual data to the receiver
    switch(header->stype)
    {
        case SFILE: // send a file
            fd = (int*)structure;
            size = header->nbelem * header->szelem;
            while(sent < size && ret > 0)
            {
                if((ret = read(*fd, serialised, sizeof(serialised))) == -1)
                {
                    if(doPrint)
                        (*doPrint)("psnd: reading the file: %s", strerror(errno));
                }

                //send the data
                if(ret != -1)
                {
                    if((ret = sendData(sockfd, serialised, &ret, NULL, 1)) == -1)
                    {
                        if(doPrint)
                            (*doPrint)("psnd: error while sending the file");
                    }
                }

                //wipe the buffer and update the amount of bytes sent
                if(ret != -1)
                {
                    memset(serialised, 0, sizeof(serialised));
                    sent += ret;
                }
            }
            break;

        case SLIST: // send a list
            lis = (meta_t*)structure;
            tmp = lis->structure;
            do
            {
                buffer = getdata(tmp);
                if ((ret = sendData(sockfd, buffer, (int*)&lis->elementsize, NULL, 1)) == -1)
                {
                    if(doPrint)
                        (*doPrint)("psnd: error while sending %s", buffer);
                }

                tmp = getright(tmp);
            }while(tmp && ret > 0);

            if(ret != -1)
                size = lis->nbelements * lis->elementsize;
            break;

        case SSTRING: //send a string
            buffer = (char*)structure;
            if((ret = sendData(sockfd, buffer, (int*)&header->szelem, NULL, 1)) == -1)
            {
                if(doPrint)
                    (*doPrint)("psnd: error while sending %s", buffer);
            }

            //update the amount of bytes sent
            if(ret != -1)
                size = strlen(buffer);
            break;
    }

    //reset the header and receive the receiver's acknowlegement
    memset(serialised, 0, sizeof(serialised));
    receiveData(sockfd, serialised, sizeof(head_t), NULL, 1);
    unpack(serialised, HEAD_F, &header->nbelem, &header->stype, &header->szelem);

    //check if it matches the one sent by the receiver
    if(header->szelem != size)
    {
        if(doPrint)
            (*doPrint)("psnd: acknowlegement header does not match the data sent");

        return -1;
    }

    return ret;
}
