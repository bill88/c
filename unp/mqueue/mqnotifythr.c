/*********************************************************************
* Copyright (C) 2014 CFETS Financial Data Co.,LTD                    *
* All right reserved.                                                *
* Name: mqnotifythr.c
* Author: Canux canuxcheng@gmail.com                                 *
* Version: V1.0                                                      *
* Time: Mon 10 Nov 2014 01:04:22 PM CST
* Description:                                                       *
*********************************************************************/
#include "all.h"

mqd_t mqd;
struct mq_attr attr;
struct sigevent sigev;

static void notify_thread(union sigval arg)
{
	ssize_t n;
	void *buff;

	printf("notify_thread started\n");
	buff = (void *)Malloc(attr.mq_msgsize);
	Mq_notify(mqd, &sigev);

	while ((n = mq_receive(mqd, buff, attr.mq_msgsize, NULL)) >= 0)
		printf("read %ld bytes\n", (long)n);
	if (errno != EAGAIN)
		err_sys("mq_receive error");

	free(buff);
	pthread_exit(NULL);
}

int main(int argc, char **argv)
{
	if (argc != 2)
		err_quit("Usage nft <name>");
	mqd = Mq_open(argv[1], O_RDONLY | O_NONBLOCK);
	Mq_getattr(mqd, &attr);

	sigev.sigev_notify = SIGEV_THREAD;
	sigev.sigev_value.sival_ptr = NULL;
	sigev.sigev_notify_function = notify_thread;
	sigev.sigev_notify_attributes = NULL;
	Mq_notify(mqd, &sigev);

	for ( ; ; )
		pause();
	exit(0);
}
