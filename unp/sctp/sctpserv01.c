/*************************************************************************
* Copyright C 2014 CFETS Financial Data Co.,LTD
* All right reserved.
* File Name: sctpserv01.c
* Version: 
* Writen by canux canuxcheng@gmail.com
* Created Time: 2014年09月23日 星期二 13时36分10秒
* Description: 
************************************************************************/
#include "all.h"

int main(int argc, char **argv)
{
	int sock_fd, msg_flags;
	char readbuf[BUFFSIZE];
    struct sockaddr_in servaddr, cliaddr;
	struct sctp_sndrcvinfo sri;
	struct sctp_event_subscribe evnts;
    struct sctp_initmsg initm;
	int stream_increment = 1;
	socklen_t len;
	size_t rd_sz;

	if (argc == 2)
		stream_increment = atoi(argv[1]);

	sock_fd = Socket(AF_INET, SOCK_SEQPACKET, IPPROTO_SCTP);

	bzero(&initm, sizeof(initm));
	initm.sinit_num_ostreams = SERV_MORE_STRMS_SCTP;
	Setsockopt(sock_fd, IPPROTO_SCTP, SCTP_INITMSG, &initm, sizeof(initm));

	bzero(&evnts, sizeof(evnts));
    evnts.sctp_data_io_event = 1;
	Setsockopt(sock_fd, IPPROTO_SCTP, SCTP_EVENTS, &evnts, sizeof(evnts));
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(sock_fd, (SA *)&servaddr, sizeof(servaddr));

	Listen(sock_fd, LISTENQ);

	for ( ; ; )
	{
		len = sizeof(cliaddr);
		rd_sz = Sctp_recvmsg(sock_fd, readbuf, sizeof(readbuf), (SA *)&cliaddr, &len, &sri, &msg_flags);

		if (stream_increment)
		{
			sri.sinfo_stream++;
			if (sri.sinfo_stream >= sctp_get_no_strms(sock_fd, (SA *)&cliaddr, len))
				sri.sinfo_stream = 0;
		}
		Sctp_sendmsg(sock_fd, readbuf, rd_sz, (SA *)&cliaddr, len, sri.sinfo_ppid, sri.sinfo_flags | SCTP_EOF, sri.sinfo_stream, 0, 0);
	}
}
