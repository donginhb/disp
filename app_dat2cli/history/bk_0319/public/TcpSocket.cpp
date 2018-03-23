#include "TcpSocket.h"
#include <sys/ioctl.h>

using namespace std;

//##ModelId=4314101E02A7
int TcpSocket::listen(int iPortID, bool bForkFlag)
{
	int optval = 1;
	int pid;
	struct sockaddr_in ServerAddr, NewAddr;
	int iTcpSocketId, iNewTcpSocketId, iNewLen = sizeof(struct sockaddr);

	memset((char *) &ServerAddr, 0, sizeof(ServerAddr));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//	ServerAddr.sin_addr.s_addr = inet_addr("192.168.191.12");
	ServerAddr.sin_port = htons(iPortID);

	while ((iTcpSocketId = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		//ENOSR---��Դ����
		//ENOMEM---�ڴ治��
		if ( errno == ENOSR || errno == ENOMEM)
		{
			sleep(1);
			continue;
		}
		printf("TcpSocket create error\n ");
		return -1;
	}
	m_iListenSocket = iTcpSocketId;
	/*
	 1������һ������ͬ���ص�ַ�Ͷ˿ڵ�TcpSocket1����TIME_WAIT״̬ʱ��������
	 ���ĳ����TcpSocket2Ҫռ�øõ�ַ�Ͷ˿ڣ���ĳ����Ҫ�õ���ѡ�
	 2��SO_REUSEADDR����ͬһport������ͬһ�������Ķ��ʵ��(�������)����
	 ÿ��ʵ���󶨵�IP��ַ�ǲ�����ͬ�ġ����ж����������IP Alias�����Ļ�����
	 �Բ������������
	 3��SO_REUSEADDR���������̰���ͬ�Ķ˿ڵ����TcpSocket�ϣ���ÿ��soc
	 ket�󶨵�ip��ַ��ͬ�����2�����ƣ������뿴UNPv1��
	 4��SO_REUSEADDR������ȫ��ͬ�ĵ�ַ�Ͷ˿ڵ��ظ��󶨡�����ֻ����UDP��
	 �ಥ��������TCP��
	 */
	if (setsockopt(iTcpSocketId, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof(optval)) != 0)
	{
		printf("setsockopt SO_REUSEADDR error\n");
		return -1;
	}

	//optval = 1;
	struct linger optlinger;
	optlinger.l_onoff = 1;
	optlinger.l_linger = 0;
	if (setsockopt(iTcpSocketId, SOL_SOCKET, SO_LINGER, (char *) &optlinger, sizeof(optlinger)) != 0)
	{
		printf("setsockopt SO_LINGER error\n");
		return -1;
	}
	for (;;)
	{
		if (::bind(iTcpSocketId, (struct sockaddr*) &ServerAddr, iNewLen) < 0)
		{
			printf("bind error:[%s]\n", strerror(errno));
			sleep(1);
			continue; //return -2;
		}
		break;
	}
	if (::listen(iTcpSocketId, 5) < 0)
	{
		printf("listen error:[%s]\n", strerror(errno));
		return -1;
	}
	printf("[Message]: Listen at Port: %d\n", iPortID);

	while (1)
	{
		for (;;)
		{
			if ((iNewTcpSocketId =
#ifdef DEF_HP
					accept(iTcpSocketId,(struct sockaddr*)&NewAddr,&iNewLen)) <0 )
#else
					accept(iTcpSocketId, (struct sockaddr*) &NewAddr, (socklen_t *) &iNewLen)) < 0)
#endif
			{
				printf("accept eror :%s\n", strerror(errno));
				sleep(1);
				continue;
			}
			else
			{
				m_iTcpSocketID = iNewTcpSocketId;
				break;
			}
		}
		if (bForkFlag == false)
		{
			close(iTcpSocketId);
			return iNewTcpSocketId;
		}
		else
		{
#ifndef DEF_LINUX
			sigset(SIGCHLD, SIG_IGN);
#else
			signal (SIGCHLD, SIG_IGN);
#endif
			if ((pid = fork()) == 0)
			{
				close(iTcpSocketId);
				return iNewTcpSocketId;
			}
			else
			{
				printf("fork process: pid=%d\n", pid);
				close(iNewTcpSocketId);
				continue;
			}
		}
	}
}

int TcpSocket::CloseListenSocket()
{
	::close(m_iListenSocket);
	return 1;
}

void TcpSocket::Close()
{
	::close(m_iTcpSocketID);
}

//##ModelId=4314101E02AA
int TcpSocket::connect(char *sRemoteHostIP, int iRemotePortID)
{
	struct sockaddr_in ServerAddr;
	struct linger mylinger;
	int iTcpSocketId, iResult, optval = 1;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr(sRemoteHostIP);
	ServerAddr.sin_port = htons(iRemotePortID);

	if ((iTcpSocketId = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("TcpSocket error\n");
		return -1;
	}
	printf("connect [%s:%d]\n", sRemoteHostIP, iRemotePortID);
	while (1)
	{
		mylinger.l_onoff = 1;
		mylinger.l_linger = 0;

		setsockopt(iTcpSocketId, SOL_SOCKET, SO_LINGER, (char *) &mylinger, sizeof(struct linger));

#ifndef DEF_LINUX
		sigset(SIGPIPE, SIG_IGN);
#else
		signal(SIGPIPE,SIG_IGN);
#endif

		optval = 1;
		setsockopt(iTcpSocketId, SOL_SOCKET, SO_KEEPALIVE, (char *) &optval, sizeof(int));

		iResult = /*std*/::connect(iTcpSocketId, (struct sockaddr*) &ServerAddr, sizeof(ServerAddr));
		if (iResult != 0)
		{
			printf("connect error:%s", strerror(errno));
			shutdown(iTcpSocketId, 2);
			close(iTcpSocketId);
			if ( errno == ECONNREFUSED)
			{
				sleep(5);
				continue;
			}
			printf("Connect error\n");
			return -1;
		}
		else
			break;
	}
	m_iTcpSocketID = iTcpSocketId;
	return iTcpSocketId;
}

int TcpSocket::connect(char *sRemoteHostIP, int iRemotePortID, int time_out_time)
{
	struct sockaddr_in ServerAddr;
	struct linger mylinger;
	int iTcpSocketId, optval = 1;
	unsigned long ul = 1;
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = inet_addr(sRemoteHostIP);
	ServerAddr.sin_port = htons(iRemotePortID);

	if ((iTcpSocketId = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("TcpSocket error\n");
		return -1;
	}
	printf("connect [%s:%d]\n", sRemoteHostIP, iRemotePortID);
	while (1)
	{
		mylinger.l_onoff = 0;
		mylinger.l_linger = 0;

		setsockopt(iTcpSocketId, SOL_SOCKET, SO_LINGER, (char *) &mylinger, sizeof(struct linger));

#ifndef DEF_LINUX
		sigset(SIGPIPE, SIG_IGN);
#else
		signal(SIGPIPE,SIG_IGN);
#endif

		optval = 1;
		setsockopt(iTcpSocketId, SOL_SOCKET, SO_KEEPALIVE, (char *) &optval, sizeof(int));

		int ul = 1;
		ioctl(iTcpSocketId, FIONBIO, &ul); //����Ϊ������ģʽ

		if (::connect(iTcpSocketId, (struct sockaddr*) &ServerAddr, sizeof(ServerAddr)) < 0)
		{
			if (errno == EINPROGRESS)
			{
				timeval tm = { time_out_time, 0 };
				fd_set setW;
				FD_ZERO(&setW);
				FD_SET(iTcpSocketId, &setW);
				int iret = select(iTcpSocketId + 1, NULL, &setW, NULL, &tm);
				if (iret == -1 || iret == 0)
				{
					if (iret == -1)
						printf("Socket read error!\n"); //return -1;//����
					if (iret == 0)
						printf("Socket read time out!\n"); //return -2;//��ʱ
					shutdown(iTcpSocketId, 2);
					close(iTcpSocketId);
					return iret;
				}
				if (FD_ISSET(iTcpSocketId, &setW))
				{
					int error = -1;
#ifdef DEF_LINUX
					socklen_t len = sizeof(error);
#else
					int len = sizeof(error);
#endif

					getsockopt(iTcpSocketId, SOL_SOCKET, SO_ERROR, &error, &len);
					if (error == 0)
					{ //�������ӳɹ�
					  //printf("");
						break;
					}
					printf("error=%d\n", error);
				}
				printf("iret=%d , manul set iret = -1\n", iret);
				shutdown(iTcpSocketId, 2);
				close(iTcpSocketId);
				iret = -1;
				return iret;

			} //if (iResult != 0)
			else
			{
				shutdown(iTcpSocketId, 2);
				close(iTcpSocketId);
				return -1;
			}
		}
		else
		{
			//printf("connect ok. irestul==0\n");
		}

		break;
	}
	m_iTcpSocketID = iTcpSocketId;
	ul = 0;
	ioctl(iTcpSocketId, FIONBIO, &ul); //����Ϊ����ģʽ 

	return iTcpSocketId;
}

//���÷�������ʽ
int TcpSocket::setBlock()
{
	int optval = 1;
	fcntl(m_iTcpSocketID, F_GETFL, &optval);
//#ifdef DEF_AIX
//	fcntl(m_iTcpSocketID , F_SETFL , optval|O_NDELAY);
//#else
	fcntl(m_iTcpSocketID, F_SETFL, optval | O_NONBLOCK);
//#endif
	return 0;
}

//##ModelId=4314101E02AD
TcpSocket::TcpSocket()
{
	m_iTcpSocketID = -1;
}

//##ModelId=4314101E02AE
TcpSocket::~TcpSocket()
{
	m_iTcpSocketID = -1;
}

/*���ͳ���Ϊlen	byte��һ������
 ���أ�>=0 -- ���͵��ֽ���
 -1  -- ʧ��, ���г�ʱʱ��timeout_val��λ����
 ��������������ww_error()���� */
int TcpSocket::Sendn(char *send_buf, int len, int timeout_val)
{
	time_t start_tm, curr_tm;
	int n, nbytes;
//  char sLen[14];

	time(&start_tm);
	nbytes = len;
//  printf("write TcpSocket len:[%d]\n", len);

	while (len > 0)
	{
		if ((n = ::write(m_iTcpSocketID, send_buf, len)) <= 0)
		{
			if ( errno != EWOULDBLOCK)
			{
				printf("write TcpSocket errno:[%d], err=%s\n", errno, strerror(errno));
				return -1;
			}
//		  printf("1.write %d,err=%s\n", n, strerror(errno));
		}
		else
		{
			len -= n;
			send_buf += n;
		}

		if (timeout_val > 0)
		{
			time(&curr_tm);
			if (difftime(curr_tm, start_tm) > timeout_val)
			{
				printf("write TcpSocket timeout:[%d],nbytes-len:[%d]\n", len, nbytes - len);
				return -1;
			}
		}
//	  printf("2.write %d\n", n);
	}
//  printf("write TcpSocket len:[%d]\n", len);
	return (nbytes - len);
}

//##ModelId=432E7B0E019F
int TcpSocket::read(unsigned char *sBuffer, int iLen)
{
	return ::read(m_iTcpSocketID, sBuffer, iLen);
}

int TcpSocket::read(unsigned char *sBuffer, int iLen, unsigned long timeout_val/*unit:ms*/)
{
	fd_set set;
	FD_ZERO(&set);
	FD_SET(m_iTcpSocketID, &set);
	struct timeval timeout;

	timeout.tv_sec = timeout_val / 1000;
	timeout.tv_usec = timeout_val * 1000 - timeout.tv_sec * 1000000;
	//struct timeval timeout = {0,timeout_val * 1000};
	int retval = select(m_iTcpSocketID + 1, &set, NULL, NULL, &timeout);
	if (retval == -1)
	{
		printf("Socket read error!\n");
		return -1; //����
	}
	else if (retval == 0)
	{
		printf("Socket read time out!\n");
		return -2; //��ʱ
	}
	if (FD_ISSET(m_iTcpSocketID, &set))
		return ::read(m_iTcpSocketID, sBuffer, iLen);
	return 0;
}

//##ModelId=432E7B5303C5
int TcpSocket::write(unsigned char *sBuffer, int iLen)
{
	return ::write(m_iTcpSocketID, sBuffer, iLen);
}

int TcpSocket::send(unsigned char *sBuffer, int iLen)
{
	return ::send(m_iTcpSocketID, sBuffer, iLen, 0);
}

int TcpSocket::listen(int iPortID, struct sockaddr_in& NewAddr, bool bForkFlag)
{
	int optval = 1;
	int pid;
	struct sockaddr_in ServerAddr;
	int iTcpSocketId, iNewTcpSocketId, iNewLen = sizeof(struct sockaddr);

	memset((char *) &ServerAddr, 0, sizeof(ServerAddr));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_addr.s_addr = htonl(INADDR_ANY);
//	ServerAddr.sin_addr.s_addr = inet_addr("192.168.191.12");
	ServerAddr.sin_port = htons(iPortID);

	while ((iTcpSocketId = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		//ENOSR---��Դ����
		//ENOMEM---�ڴ治��
		if ( errno == ENOSR || errno == ENOMEM)
		{
			sleep(1);
			continue;
		}
		printf("TcpSocket create error\n ");
		return -1;
	}
	m_iListenSocket = iTcpSocketId;
	/*
	 1������һ������ͬ���ص�ַ�Ͷ˿ڵ�TcpSocket1����TIME_WAIT״̬ʱ��������
	 ���ĳ����TcpSocket2Ҫռ�øõ�ַ�Ͷ˿ڣ���ĳ����Ҫ�õ���ѡ�
	 2��SO_REUSEADDR����ͬһport������ͬһ�������Ķ��ʵ��(�������)����
	 ÿ��ʵ���󶨵�IP��ַ�ǲ�����ͬ�ġ����ж����������IP Alias�����Ļ�����
	 �Բ������������
	 3��SO_REUSEADDR���������̰���ͬ�Ķ˿ڵ����TcpSocket�ϣ���ÿ��soc
	 ket�󶨵�ip��ַ��ͬ�����2�����ƣ������뿴UNPv1��
	 4��SO_REUSEADDR������ȫ��ͬ�ĵ�ַ�Ͷ˿ڵ��ظ��󶨡�����ֻ����UDP��
	 �ಥ��������TCP��
	 */
	if (setsockopt(iTcpSocketId, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof(optval)) != 0)
	{
		printf("setsockopt SO_REUSEADDR error\n");
		return -1;
	}
	for (;;)
	{
		if (::bind(iTcpSocketId, (struct sockaddr*) &ServerAddr, iNewLen) < 0)
		{
			printf("bind error:[%s]\n", strerror(errno));
			sleep(1);
			continue; //return -2;
		}
		break;
	}
	if (::listen(iTcpSocketId, 5) < 0)
	{
		printf("listen error:[%s]\n", strerror(errno));
		return -1;
	}
	printf("[Message]: Listen at Port: %d\n", iPortID);

	while (1)
	{
		for (;;)
		{
			if ((iNewTcpSocketId =
#ifdef DEF_HP
					accept(iTcpSocketId,(struct sockaddr*)&NewAddr,&iNewLen)) <0 )
#else
					accept(iTcpSocketId, (struct sockaddr*) &NewAddr, (socklen_t *) &iNewLen)) < 0)
#endif
			{
				printf("accept eror :%s\n", strerror(errno));
				sleep(1);
				continue;
			}
			else
			{
				m_iTcpSocketID = iNewTcpSocketId;
				break;
			}
		}
		if (bForkFlag == false)
		{
			close(iTcpSocketId);
			return iNewTcpSocketId;
		}
		else
		{
#ifndef DEF_LINUX
			sigset(SIGCHLD, SIG_IGN);
#else
			signal (SIGCHLD, SIG_IGN);
#endif
			if ((pid = fork()) == 0)
			{
				close(iTcpSocketId);
				return iNewTcpSocketId;
			}
			else
			{
				printf("fork process: pid=%d\n", pid);
				close(iNewTcpSocketId);
				continue;
			}
		}
	}
}

char * TcpSocket::getClientAddr(char *sClientAddr, int iLen)
{
	sClientAddr[0] = 0;
	struct sockaddr_in ClientAddr;
	int Addr_Len = sizeof(struct sockaddr_in);
#ifdef DEF_HP
	if (getpeername(m_iTcpSocketID,(void *)&ClientAddr,&Addr_Len)==0)
#else
	if (getpeername(m_iTcpSocketID, (struct sockaddr *) &ClientAddr, (socklen_t *) &Addr_Len) == 0)
#endif
		snprintf(sClientAddr, iLen, "%s:%d", inet_ntoa(ClientAddr.sin_addr), ntohs(ClientAddr.sin_port));
	else
		snprintf(sClientAddr, iLen, "%s", "0.0.0.0");
	return sClientAddr;
}

int TcpSocket::setSocketBufferLen(bool bType, int &iLen)
{
	socklen_t optlen = sizeof(iLen);
	if (bType)
		return setsockopt(m_iTcpSocketID, SOL_SOCKET, SO_SNDBUF, &iLen, optlen);
	else
		return setsockopt(m_iTcpSocketID, SOL_SOCKET, SO_RCVBUF, &iLen, optlen);
}

int TcpSocket::getSocketBufferLen(bool bType, int &iLen)
{
#ifdef DEF_LINUX
	socklen_t optlen = sizeof(iLen);
#else
	int optlen = sizeof(iLen);
#endif
	if (bType)
		return getsockopt(m_iTcpSocketID, SOL_SOCKET, SO_SNDBUF, &iLen, &optlen);
	else
		return getsockopt(m_iTcpSocketID, SOL_SOCKET, SO_RCVBUF, &iLen, &optlen);
}

