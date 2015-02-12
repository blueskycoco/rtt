#include "con_socket.h"
#include <rtdevice.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>

/*client use socket,server use netconn*/
#define BUF_SIZE 1024
rt_thread_t tid_w[4]={RT_NULL,RT_NULL,RT_NULL,RT_NULL},tid_r[4]={RT_NULL,RT_NULL,RT_NULL,RT_NULL};
extern struct rt_semaphore fifo_sem;
int ipv6_flag[4]={false,false,false,false};
int ipv4_flag[4]={false,false,false,false};
struct rt_mutex mutex[4];
extern bool ind[4];
bool is_right(char config,char flag)
{
	if((config&flag)==flag)
		return true;
	else
		return false;
}
void lock(int i)
{
    rt_err_t result;

    result = rt_mutex_take(&mutex[i], RT_WAITING_FOREVER);
    if (result != RT_EOK)
    {
        RT_ASSERT(0);
    }
}
void unlock(int i)
{
    rt_mutex_release(&mutex[i]);
}

void socket_ip6_w(void *paramter)
{
	int dev=(int)paramter;
	rt_size_t data_size;
	const void *last_data_ptr;
	struct timeval tv;
	fd_set myset;
	int status;
	tv.tv_sec = 0; 
    tv.tv_usec = 10000; 
	rt_kprintf("socket_ip6_w==> %d , %s mode, %s , %s . Thread Enter\r\n",dev,is_right(g_conf.config[dev],CONFIG_SERVER)?"Server":"Client",is_right(g_conf.config[dev],CONFIG_IPV6)?"IPV6":"IPV4",is_right(g_conf.config[dev],CONFIG_TCP)?"TCP":"UDP");
	while(1)
	{
		if(!g_ip6[dev].connected)
		{
			rt_thread_delay(10);
			continue;
		}
		cnn_out(dev,1);
		FD_ZERO(&myset);
		int sock;
		if(is_right(g_conf.config[dev],CONFIG_SERVER))
        	sock=g_ip6[dev].clientfd;
		else
			sock=g_ip6[dev].sockfd;
		FD_SET(sock, &myset);
		if(select(sock+1,NULL, &myset,  NULL, &tv) > 0) 
		{ 
			rt_data_queue_pop(&g_data_queue[dev*2], &last_data_ptr, &data_size, 0);
			if(data_size>0)
			{
				lock(dev);
				if(is_right(g_conf.config[dev],CONFIG_TCP))
				{
					status=send(sock, last_data_ptr, data_size, 0);				
				}
				else
				{
					if(is_right(g_conf.config[dev],CONFIG_SERVER))
						status=sendto(g_ip6[dev].sockfd, last_data_ptr, data_size, 0, (struct sockaddr *)&g_ip6[dev].client_addr6, sizeof(g_ip6[dev].client_addr6));
					else
						status=sendto(g_ip6[dev].sockfd, last_data_ptr, data_size, 0, (struct sockaddr *)&g_ip6[dev].server_addr6, sizeof(g_ip6[dev].server_addr6));
				}
				unlock(dev);
				if( status< 0)
				{
					rt_kprintf("Thread ip6_w%d send error\n",dev,errno);
					if(is_right(g_conf.config[dev],CONFIG_TCP))
					{
						rt_kprintf("Thread ip6_w%d send error %d\n",dev,errno);
						//g_ip6[dev].connected=false;
						cnn_out(dev,0);
					}
				}
			}
		}
	}
}
void socket_ip6_r(void *paramter)
{
	int dev=(int)paramter;
	struct sockaddr_in6 server_addr6;	
	int status;
	fd_set myset; 
  	struct timeval tv; 
  	socklen_t lon; 
	int valopt,ret; 
	struct sockaddr_in6 client_addr6;
	tv.tv_sec = 0; 
    tv.tv_usec = 10000; 
	rt_kprintf("socket_ip6_r==> %d , %s mode, %s , %s . Thread Enter\r\n",dev,is_right(g_conf.config[dev],CONFIG_SERVER)?"Server":"Client",is_right(g_conf.config[dev],CONFIG_IPV6)?"IPV6":"IPV4",is_right(g_conf.config[dev],CONFIG_TCP)?"TCP":"UDP");
	while(1)
	{
		if(g_ip6[dev].connected==false)
		{
			if(is_right(g_conf.config[dev],CONFIG_SERVER))
			{				
				rt_kprintf("socket_ip6_r %d to accept %d\n",dev,g_ip6[dev].sockfd);
				rt_uint32_t  sin_size = sizeof(struct sockaddr_in6);
				FD_ZERO(&myset);
		        FD_SET(g_ip4[dev].sockfd, &myset);
		        if(select(g_ip6[dev].sockfd+1, &myset, 0, 0, &tv) > 0 )
		        {
					g_ip6[dev].clientfd = accept(g_ip6[dev].sockfd, (struct sockaddr *)&client_addr6, &sin_size);
					if(g_ip6[dev].clientfd!=-1)
					{
						rt_kprintf("socket_ip6_r %d I got a connection from (IP:%s, PORT:%d\n) fd %d\n", dev,inet6_ntoa(client_addr6.sin6_addr), ntohs(client_addr6.sin6_port),g_ip6[dev].clientfd);
						g_ip6[dev].connected=true;
						cnn_out(dev,1);
						char a=1;
						setsockopt(g_ip6[dev].clientfd, SOL_SOCKET, SO_KEEPALIVE, &a, sizeof(char));										
					}
		        }
			}
			else
			{
				//rt_kprintf("to connect ipv6\n");
				status = connect(g_ip6[dev].sockfd, (struct sockaddr *)&g_ip6[dev].server_addr6, sizeof(g_ip6[dev].server_addr6));
				if(status==0)
				{
					g_ip6[dev].connected=true;
					cnn_out(dev,1);
					int imode=0;
					ioctlsocket(g_ip6[dev].sockfd, FIONBIO, &imode);
				}
				else if(errno==EINPROGRESS)
				{
					
			        FD_ZERO(&myset); 
			        FD_SET(g_ip6[dev].sockfd, &myset);
			        if(select(g_ip6[dev].sockfd+1, NULL, &myset, NULL, &tv) > 0) 
					{ 
					   if(FD_ISSET(g_ip6[dev].sockfd,&myset))
					   {
				           lon = sizeof(int); 
				           getsockopt(g_ip6[dev].sockfd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon); 
				           if(!valopt) 
						   { 
				              rt_kprintf("OK Connection is done\n"); 
							  g_ip6[dev].connected=true;
							  cnn_out(dev,1);
							  int imode=0;
							  ioctlsocket(g_ip6[dev].sockfd, FIONBIO, &imode);
							 continue;           
				           }
					   	}
			        }
				}
				lock(dev);
				closesocket(g_ip6[dev].sockfd);
				g_ip6[dev].sockfd= socket(PF_INET, SOCK_STREAM, 0);		
				unlock(dev);
				int imode = 1;
				setsockopt(g_ip6[dev].sockfd,SOL_SOCKET,SO_KEEPALIVE,&imode,sizeof(imode));
				ioctlsocket(g_ip6[dev].sockfd, FIONBIO, &imode);
				
			}			
		}
		if(g_ip6[dev].connected==false)
		{
			rt_thread_delay(10);			
			continue;
		}
		//if(need_reconfig(dev))
		//{
			
		//}
		cnn_out(dev,1);
		socklen_t clientlen = sizeof(g_ip6[dev].server_addr6);
		FD_ZERO(&myset); 
		int sock;
		if(is_right(g_conf.config[dev],CONFIG_SERVER))
        	sock=g_ip6[dev].clientfd;
		else
			sock=g_ip6[dev].sockfd;
		FD_SET(sock, &myset);
        if(select(sock+1, &myset, NULL, NULL, &tv) > 0) 
		{ 
			if(is_right(g_conf.config[dev],CONFIG_TCP))
			{
				status=recv(sock, g_ip6[dev].recv_data, BUF_SIZE, 0);	
				if(status>0)
				{
					if(ind[dev])
					rt_data_queue_push(&g_data_queue[dev*2+1], g_ip6[dev].recv_data, status, RT_WAITING_FOREVER);
				}
				else
				{
					rt_kprintf("Thread ip6_r_%d recv error,connection lost\n",dev);
					if(is_right(g_conf.config[dev],CONFIG_SERVER))
					{
						
							rt_kprintf("Thread ip6_r%d recv error %d\n",dev,errno);	
							lock(dev);
							closesocket(g_ip6[dev].clientfd);
							unlock(dev);
							g_ip6[dev].connected=false;
							cnn_out(dev,0);
					}
					else
					{
						
							rt_kprintf("Thread ip6_r%d recv error %d\n",dev,errno);	
							lock(dev);
							closesocket(g_ip6[dev].sockfd);
							unlock(dev);
							g_ip6[dev].sockfd= socket(PF_INET6, SOCK_STREAM, 0);						
							int imode = 1;
							setsockopt(g_ip6[dev].sockfd,SOL_SOCKET,SO_KEEPALIVE,&imode,sizeof(imode));
							ioctlsocket(g_ip6[dev].sockfd, FIONBIO, &imode);
							g_ip6[dev].connected=false;
							cnn_out(dev,0);
					}
				}

			}
			else
			{	
				status=recvfrom(g_ip6[dev].sockfd, g_ip6[dev].recv_data, BUF_SIZE, 0, (struct sockaddr *)&g_ip6[dev].server_addr6, &clientlen);
				//rt_kprintf("socket_ip6_r status %d\n",status);
				if(status>0)
				{
					if(ind[dev])
					rt_data_queue_push(&g_data_queue[dev*2+1], g_ip6[dev].recv_data, status, RT_WAITING_FOREVER);	
				}
				else
				{
					rt_kprintf("Thread ip6_r_%d Recvfrom error,connection lost\n",dev);
				}
			}	
        }
	}
	rt_kprintf("socket_ip6_r close\n");
}

bool socket_ip6(int dev)
{	
	/*create socket*/
	if(is_right(g_conf.config[dev],CONFIG_TCP))
		g_ip6[dev].sockfd= socket(PF_INET6, SOCK_STREAM, 0);
	else
		g_ip6[dev].sockfd= socket(PF_INET6, SOCK_DGRAM, 0);
	if(g_ip6[dev].sockfd == -1)
	{
		rt_kprintf("Socket error\n");
		return false;
	}
	int imode = 1;
    setsockopt(g_ip6[dev].sockfd,SOL_SOCKET,SO_KEEPALIVE,&imode,sizeof(imode));	   
	/*init sockaddr_in6 */
	if(is_right(g_conf.config[dev],CONFIG_SERVER))
	{//server mode
		g_ip6[dev].server_addr6.sin6_family = AF_INET6;
		memcpy(g_ip6[dev].server_addr6.sin6_addr.s6_addr, IP6_ADDR_ANY, 16);
		g_ip6[dev].server_addr6.sin6_port = htons(g_conf.local_port[dev]);
		if(bind(g_ip6[dev].sockfd, (struct sockaddr *)&g_ip6[dev].server_addr6, sizeof(struct sockaddr)) == -1)
		{
			rt_kprintf("Bind error\n");
			lock(dev);
			closesocket(g_ip6[dev].sockfd);
			unlock(dev);
			return false;
		}
		if(is_right(g_conf.config[dev],CONFIG_TCP))
		{
			rt_kprintf("socket_ip6 %d to listen %d\n",dev,g_ip6[dev].sockfd);
			if(listen(g_ip6[dev].sockfd, 1) == -1)
			{
				rt_kprintf("Listen error\n");
				lock(dev);
				closesocket(g_ip6[dev].sockfd);
				unlock(dev);
				return false;
			}
		}
		else
		{
			memset(&g_ip6[dev].client_addr6, 0, sizeof(g_ip6[dev].client_addr6));
			g_ip6[dev].client_addr6.sin6_family = AF_INET6;
			g_ip6[dev].client_addr6.sin6_port = htons(g_conf.remote_port[dev]);
			if(inet_pton(AF_INET6, (char *)g_conf.remote_ip6[dev], &g_ip6[dev].client_addr6.sin6_addr.s6_addr) != 1)
			{
				rt_kprintf("inet_pton() error\n");
				lock(dev);
				closesocket(g_ip6[dev].sockfd);
				unlock(dev);
				return false;
			}
		}
	}
	else
	{//client mode
		memset(&g_ip6[dev].server_addr6, 0, sizeof(g_ip6[dev].server_addr6));
		g_ip6[dev].server_addr6.sin6_family = AF_INET6;
		g_ip6[dev].server_addr6.sin6_port = htons(g_conf.remote_port[dev]);
		if(inet_pton(AF_INET6, g_conf.remote_ip6[dev], &g_ip6[dev].server_addr6.sin6_addr.s6_addr) != 1)
		{
			rt_kprintf("inet_pton() error\n");
			lock(dev);
			closesocket(g_ip6[dev].sockfd);
			unlock(dev);
			return false;
		}
		if(!is_right(g_conf.config[dev],CONFIG_TCP))
		{
			g_ip6[dev].client_addr6.sin6_family = AF_INET6;
			memcpy(g_ip6[dev].client_addr6.sin6_addr.s6_addr, IP6_ADDR_ANY, 16);
			g_ip6[dev].client_addr6.sin6_port = htons(g_conf.local_port[dev]);
			if(bind(g_ip6[dev].sockfd, (struct sockaddr *)&g_ip6[dev].client_addr6, sizeof(struct sockaddr)) == -1)
			{
				rt_kprintf("Bind error\n");
				lock(dev);
				closesocket(g_ip6[dev].sockfd);
				unlock(dev);
				return false;
			}
		}
		else
		{
			ioctlsocket(g_ip6[dev].sockfd, FIONBIO, &imode);
		}
	}
	/*mall receive buffer*/		
	g_ip6[dev].recv_data = rt_malloc(BUF_SIZE);
	if(g_ip6[dev].recv_data == RT_NULL)
	{
		rt_kprintf(" ip6 %d No memory\n",dev);
		lock(dev);
		closesocket(g_ip6[dev].sockfd);
		unlock(dev);
		return false;
	}
	if(is_right(g_conf.config[dev],CONFIG_TCP))
	{
		g_ip6[dev].connected=false;
		cnn_out(dev,0);
	}
	else
	{
		g_ip6[dev].connected=true;	
		cnn_out(dev,1);
	}
	return true;
}
void socket_ip4_w(void *paramter)
{
	int dev=(int)paramter;
	rt_size_t data_size;
	const void *last_data_ptr;
	struct timeval tv;
	fd_set myset;
	int status;
	tv.tv_sec = 0; 
    tv.tv_usec = 10000; 
	rt_kprintf("socket_ip4_w==> %d , %s mode, %s , %s . Thread Enter\r\n",dev,is_right(g_conf.config[dev],CONFIG_SERVER)?"Server":"Client",is_right(g_conf.config[dev],CONFIG_IPV6)?"IPV6":"IPV4",is_right(g_conf.config[dev],CONFIG_TCP)?"TCP":"UDP");
	while(1)
	{
		if(!g_ip4[dev].connected)
		{
			rt_thread_delay(10);
			continue;
		}
		cnn_out(dev,1);		
		FD_ZERO(&myset);
		int sock;
		if(is_right(g_conf.config[dev],CONFIG_SERVER))
        	sock=g_ip4[dev].clientfd;
		else
			sock=g_ip4[dev].sockfd;
		FD_SET(sock, &myset);
		if(select(sock+1,NULL, &myset,  NULL, &tv) > 0) 
		{ 
			rt_data_queue_pop(&g_data_queue[dev*2], &last_data_ptr, &data_size, RT_WAITING_FOREVER);		
			if(data_size>0)
			{
				lock(dev);
				if(is_right(g_conf.config[dev],CONFIG_TCP))
				{
					status=send(sock, last_data_ptr, data_size, 0);
				
				}
				else
				{
					if(is_right(g_conf.config[dev],CONFIG_SERVER))
						status=sendto(g_ip4[dev].sockfd, last_data_ptr, data_size, 0, (struct sockaddr *)&g_ip4[dev].client_addr, sizeof(g_ip4[dev].client_addr));
					else
						status=sendto(g_ip4[dev].sockfd, last_data_ptr, data_size, 0, (struct sockaddr *)&g_ip4[dev].server_addr, sizeof(g_ip4[dev].server_addr));
				}
				unlock(dev);
				if( status< 0)
				{
					rt_kprintf("Thread ip4_w%d send error\n",dev);
					if(is_right(g_conf.config[dev],CONFIG_TCP))
					{
						rt_kprintf("Thread ip4_w%d send error %d\n",dev,errno);	
						cnn_out(dev,0);
						g_ip4[dev].connected=false;
					}//EINPROGRESS
				} 
			}
		}
	}
	rt_kprintf("socket_ip_w %d close\n",dev);
}
void socket_ip4_r(void *paramter)
{
	int dev=(int)paramter;
	struct sockaddr_in server_addr;	
	int status;
	fd_set myset; 
  	struct timeval tv; 
  	socklen_t lon; 
	int valopt,ret; 
	struct sockaddr_in client_addr;
	tv.tv_sec = 0; 
    tv.tv_usec = 10000; 
	rt_kprintf("socket_ip4_r==> %d , %s mode, %s , %s . Thread Enter\r\n",dev,is_right(g_conf.config[dev],CONFIG_SERVER)?"Server":"Client",is_right(g_conf.config[dev],CONFIG_IPV6)?"IPV6":"IPV4",is_right(g_conf.config[dev],CONFIG_TCP)?"TCP":"UDP");
	while(1)
	{
		if(g_ip4[dev].connected==false)
		{
			if(is_right(g_conf.config[dev],CONFIG_SERVER))
			{				
				rt_uint32_t  sin_size = sizeof(struct sockaddr_in);
				FD_ZERO(&myset);
		        FD_SET(g_ip4[dev].sockfd, &myset);
		        if(select(g_ip4[dev].sockfd+1, &myset, 0, 0, &tv) > 0 )
		        {
					g_ip4[dev].clientfd = accept(g_ip4[dev].sockfd, (struct sockaddr *)&client_addr, &sin_size);
					if(g_ip4[dev].clientfd!=-1)
					{
						rt_kprintf("socket_ip4_r %d I got a connection from (IP:%s, PORT:%d\n) fd %d\n", dev,inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port),g_ip4[dev].clientfd);
						g_ip4[dev].connected=true;
						cnn_out(dev,1);
						char a=1;
						setsockopt(g_ip4[dev].clientfd, SOL_SOCKET, SO_KEEPALIVE, &a, sizeof(char));
					}
				}				
			}
			else
			{
				status = connect(g_ip4[dev].sockfd, (struct sockaddr *)&g_ip4[dev].server_addr, sizeof(g_ip4[dev].server_addr));
				if(status==0)
				{
					g_ip4[dev].connected=true;
					cnn_out(dev,1);
					int imode=0;
					ioctlsocket(g_ip4[dev].sockfd, FIONBIO, &imode);
				}
				else if(errno==EINPROGRESS)
				{
					
			        FD_ZERO(&myset); 
			        FD_SET(g_ip4[dev].sockfd, &myset);
			        if(select(g_ip4[dev].sockfd+1, NULL, &myset, NULL, &tv) > 0) 
					{ 
					   if(FD_ISSET(g_ip4[dev].sockfd,&myset))
					   {
				           lon = sizeof(int); 
				           getsockopt(g_ip4[dev].sockfd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon); 
				           if(!valopt) 
						   { 
				              rt_kprintf("OK Connection is done\n"); 
							  g_ip4[dev].connected=true;
							  cnn_out(dev,1);
							  int imode=0;
							  ioctlsocket(g_ip4[dev].sockfd, FIONBIO, &imode);
							 continue;           
				           }
					   	}
			        }
				}
				lock(dev);
				closesocket(g_ip4[dev].sockfd);
				unlock(dev);
				g_ip4[dev].sockfd= socket(PF_INET, SOCK_STREAM, 0);					
				int imode = 1;
				setsockopt(g_ip4[dev].sockfd,SOL_SOCKET,SO_KEEPALIVE,&imode,sizeof(imode));
				ioctlsocket(g_ip4[dev].sockfd, FIONBIO, &imode);				
			}			
		}
		if(g_ip4[dev].connected==false)
		{
			rt_thread_delay(10);			
			continue;
		}
		cnn_out(dev,1);
		socklen_t clientlen = sizeof(g_ip4[dev].server_addr);
		FD_ZERO(&myset); 
		int sock;
		if(is_right(g_conf.config[dev],CONFIG_SERVER))
        	sock=g_ip4[dev].clientfd;
		else
			sock=g_ip4[dev].sockfd;
		FD_SET(sock, &myset);
        if(select(sock+1, &myset, NULL, NULL, &tv) > 0) 
		{ 
			if(is_right(g_conf.config[dev],CONFIG_TCP))
			{
				status=recv(sock, g_ip4[dev].recv_data, BUF_SIZE, 0);					
				
				if(status>0)
				{
					if(ind[dev])
					rt_data_queue_push(&g_data_queue[dev*2+1], g_ip4[dev].recv_data, status, RT_WAITING_FOREVER);
				}
				else
				{
					rt_kprintf("Thread ip4_r_%d recv error,connection lost %d %d\n",dev,status,errno);
					if(is_right(g_conf.config[dev],CONFIG_SERVER))
					{
						rt_kprintf("Thread ip4_r%d recv error %d\n",dev,errno);	
						lock(dev);
						closesocket(g_ip4[dev].clientfd);
						unlock(dev);
						g_ip4[dev].connected=false;
						cnn_out(dev,0);
					}
					else
					{
						rt_kprintf("Thread ip4_r%d recv error %d\n",dev,errno);	
						lock(dev);
						closesocket(g_ip4[dev].sockfd);
						g_ip4[dev].sockfd= socket(PF_INET, SOCK_STREAM, 0);
						unlock(dev);
						int imode = 1;
						setsockopt(g_ip4[dev].sockfd,SOL_SOCKET,SO_KEEPALIVE,&imode,sizeof(imode));
						ioctlsocket(g_ip4[dev].sockfd, FIONBIO, &imode);
						g_ip4[dev].connected=false;
						cnn_out(dev,0);
					}
					
				}

			}
			else
			{
				status=recvfrom(g_ip4[dev].sockfd, g_ip4[dev].recv_data, BUF_SIZE, 0, (struct sockaddr *)&g_ip4[dev].server_addr, &clientlen);
				if(status>0)
				{		
					if(ind[dev])
					rt_data_queue_push(&g_data_queue[dev*2+1], g_ip4[dev].recv_data, status, RT_WAITING_FOREVER);
				}
				else
				{
					rt_kprintf("Thread ip4_r_%d Recvfrom error,connection lost\n",dev);
				}
			}
		}
		//else
			//rt_kprintf("%d select recv %d\n",dev,errno);
	}
	rt_kprintf("socket_ip4_r %d close\n",dev);
}
void test_select_connect()
{
	struct sockaddr_in server_addr;
	fd_set myset; 
  	struct timeval tv; 
  	socklen_t lon; 
	int res=-1, valopt,sockfd,ret; 
	int imode = 1;  
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(1234);
	server_addr.sin_addr.s_addr=inet_addr("192.168.1.6");
	rt_memset(&(server_addr.sin_zero),0, sizeof(server_addr.sin_zero));
	rt_thread_delay(300);
	//list_mem1();
	//list_tcps1();
	//list_thread();
	//EINPROGRESS
			
	do
	{	
		sockfd= socket(PF_INET, SOCK_STREAM, 0);
	    setsockopt(sockfd,SOL_SOCKET,SO_KEEPALIVE,&imode,sizeof(imode));
	    ioctlsocket(sockfd, FIONBIO, &imode);
	    ret=connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
		rt_kprintf("verify ret\n");
		if(ret==0)
			break;
		else if(errno==EINPROGRESS)
		{
			tv.tv_sec = 0; 
	        tv.tv_usec = 10000; 
	        FD_ZERO(&myset); 
	        FD_SET(sockfd, &myset);
	        if(select(sockfd+1, NULL, &myset, NULL, &tv) > 0) 
			{ 
			   if(FD_ISSET(sockfd,&myset))
			   {
		           lon = sizeof(int); 
		           getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon); 
		           if(!valopt) 
				   { 
		              rt_kprintf("OK Connection is done\n"); 
					  break;		           
		           }
				   else
		           {
		           	  //rt_kprintf("Error in connection() %d\n", valopt); 
					  closesocket(sockfd);
		           }
			   	   
			   }
			   closesocket(sockfd);
	        } 
	        else
			{  
			   closesocket(sockfd);
	        }			
		}
		else
		{
			 rt_kprintf("Error connecting %d\n", errno); 
			 closesocket(sockfd);
		}
	}
	while(1);
	rt_thread_delay(10);
	//list_mem1();
//	list_tcps1();
	//list_thread();
	imode=0;
	ioctlsocket(sockfd, FIONBIO, &imode); 
	send(sockfd,"1234",sizeof("1234"),0);
	closesocket(sockfd);
	rt_thread_delay(10);
	//list_mem1();
	//list_tcps1();
	//list_thread();
}
void test_select_accept()
{
	struct sockaddr_in server_addr;
	fd_set myset; 
  	struct timeval tv; 
  	socklen_t lon; 
	int res=-1, valopt,sockfd,ret,clientfd; 
	struct sockaddr_in client_addr;
	int imode = 1;  
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(1234);
	server_addr.sin_addr.s_addr=INADDR_ANY;
	rt_memset(&(server_addr.sin_zero),0, sizeof(server_addr.sin_zero));
	rt_thread_delay(200);
	//list_mem1();
	//list_tcps1();
	//list_thread();
	//EINPROGRESS
	sockfd= socket(PF_INET, SOCK_STREAM, 0);
    setsockopt(sockfd,SOL_SOCKET,SO_KEEPALIVE,&imode,sizeof(imode));
    ioctlsocket(sockfd, FIONBIO, &imode);
	if(bind(sockfd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr)) == -1)
	{
		rt_kprintf("Bind error\n");
		closesocket(sockfd);
		return ;
	}
	if(listen(sockfd, 1) == -1)
	{
		rt_kprintf("Listen error\n");
		closesocket(sockfd);
		return ;
	}
	do
	{	
		tv.tv_sec = 0; 
	    tv.tv_usec = 1000; 
		FD_ZERO(&myset);
        FD_SET(sockfd, &myset);
        if(lwip_select(sockfd+1, &myset, 0, 0, &tv) == 0 )
        {
			rt_kprintf("Time out Server select\n");
			continue;
		}
	    rt_uint32_t  sin_size = sizeof(struct sockaddr_in);
		clientfd = accept(sockfd, (struct sockaddr *)&client_addr, &sin_size);
		rt_kprintf("clientfd %d\n",clientfd);
		break;
	}while(1);
	rt_thread_delay(10);
	//list_mem1();
	//list_tcps1();
	//list_thread();
	imode=0;
	ioctlsocket(clientfd, FIONBIO, &imode); 
	send(clientfd,"1234",sizeof("1234"),0);
	closesocket(clientfd);
	closesocket(sockfd);
	rt_thread_delay(10);
	//list_mem1();
	//list_tcps1();
	//list_thread();
}

bool socket_ip4(int dev)
{	
	/*create socket*/
	if(is_right(g_conf.config[dev],CONFIG_TCP))
		g_ip4[dev].sockfd= socket(PF_INET, SOCK_STREAM, 0);
	else
		g_ip4[dev].sockfd= socket(PF_INET, SOCK_DGRAM, 0);

	if(g_ip4[dev].sockfd == -1)
	{
		rt_kprintf("Socket error\n");
		return false;
	}
	int imode = 1;
    setsockopt(g_ip4[dev].sockfd,SOL_SOCKET,SO_KEEPALIVE,&imode,sizeof(imode));	   
	/*init sockaddr_in */
	if(is_right(g_conf.config[dev],CONFIG_SERVER))
	{//server mode
		g_ip4[dev].server_addr.sin_family = AF_INET;
		g_ip4[dev].server_addr.sin_addr.s_addr = INADDR_ANY;
		g_ip4[dev].server_addr.sin_port = htons(g_conf.local_port[dev]);
		rt_memset(&(g_ip4[dev].server_addr.sin_zero),8, sizeof(g_ip4[dev].server_addr.sin_zero));
		if(bind(g_ip4[dev].sockfd, (struct sockaddr *)&g_ip4[dev].server_addr, sizeof(struct sockaddr)) == -1)
		{
			rt_kprintf("Bind error\n");
			lock(dev);
			closesocket(g_ip4[dev].sockfd);
			unlock(dev);
			return false;
		}
		if(is_right(g_conf.config[dev],CONFIG_TCP))
		{
			rt_kprintf("to listen %d\n",g_ip4[dev].sockfd);
			if(listen(g_ip4[dev].sockfd, 1) == -1)
			{
				rt_kprintf("Listen error\n");
				lock(dev);
				closesocket(g_ip4[dev].sockfd);
				unlock(dev);
				return false;
			}
		}
		else
		{
			memset(&g_ip4[dev].client_addr, 0, sizeof(g_ip4[dev].client_addr));
			g_ip4[dev].client_addr.sin_family = AF_INET;
			g_ip4[dev].client_addr.sin_port = htons(g_conf.remote_port[dev]);
			rt_memset(&(g_ip4[dev].client_addr.sin_zero),0, sizeof(g_ip4[dev].client_addr.sin_zero));
			g_ip4[dev].client_addr.sin_addr.s_addr=inet_addr(g_conf.remote_ip[dev]);
		}
	}
	else
	{//client mode
		memset(&g_ip4[dev].server_addr, 0, sizeof(g_ip4[dev].server_addr));
		g_ip4[dev].server_addr.sin_family = AF_INET;
		g_ip4[dev].server_addr.sin_port = htons(g_conf.remote_port[dev]);
		rt_memset(&(g_ip4[dev].server_addr.sin_zero),0, sizeof(g_ip4[dev].server_addr.sin_zero));
		g_ip4[dev].server_addr.sin_addr.s_addr=inet_addr(g_conf.remote_ip[dev]);
		if(!is_right(g_conf.config[dev],CONFIG_TCP))
		{
			g_ip4[dev].client_addr.sin_family = AF_INET;
			g_ip4[dev].client_addr.sin_addr.s_addr = INADDR_ANY;
			g_ip4[dev].client_addr.sin_port = htons(g_conf.local_port[dev]);
			rt_memset(&(g_ip4[dev].client_addr.sin_zero),0, sizeof(g_ip4[dev].client_addr.sin_zero));
			if(bind(g_ip4[dev].sockfd, (struct sockaddr *)&g_ip4[dev].client_addr, sizeof(struct sockaddr)) == -1)
			{
				rt_kprintf("Bind error\n");
				lock(dev);
				closesocket(g_ip4[dev].sockfd);
				unlock(dev);
				return false;
			}
		}
		else
		 ioctlsocket(g_ip4[dev].sockfd, FIONBIO, &imode);
	}
	/*mall receive buffer*/
	g_ip4[dev].recv_data = rt_malloc(BUF_SIZE);
	if(g_ip4[dev].recv_data == RT_NULL)
	{
		rt_kprintf(" socket_ip4 %d No memory\n",dev);
		lock(dev);
		closesocket(g_ip4[dev].sockfd);
		unlock(dev);
		return false;
	}
	if(is_right(g_conf.config[dev],CONFIG_TCP))
	{
		g_ip4[dev].connected=false;
		cnn_out(dev,0);
	}
	else
	{
		g_ip4[dev].connected=true;	
		cnn_out(dev,1);
	}
	return true;
}

/*init socket 1,2,3,4*/
void socket_init(int i)
{
	rt_uint8_t *thread_string;	
	thread_string=(rt_uint8_t *)rt_malloc(20*sizeof(rt_uint8_t));
	rt_memset(thread_string,'\0',20);
	rt_kprintf("%s Socket==> %d , %s mode, %s , %s . Thread Enter\r\n","Create",i,is_right(g_conf.config[i],CONFIG_SERVER)?"Server":"Client",is_right(g_conf.config[i],CONFIG_IPV6)?"IPV6":"IPV4",is_right(g_conf.config[i],CONFIG_TCP)?"TCP":"UDP");
	if(tid_w[i]==RT_NULL && tid_r[i]==RT_NULL)
	{
		rt_sprintf(thread_string,"socket_%d_mu",i);
		rt_mutex_init(&mutex[i], thread_string, RT_IPC_FLAG_FIFO);
		if(is_right(g_conf.config[i],CONFIG_IPV6))
		{//ipv6
			g_ip6[i].connected=false;
			if(socket_ip6(i))
			{
				ipv6_flag[i]=true;
				rt_sprintf(thread_string,"socket_%d_6_w",i);
				tid_w[i] = rt_thread_create(thread_string,socket_ip6_w, (void *)i,2048, 20, 10);
				rt_sprintf(thread_string,"socket_%d_6_r",i);
				tid_r[i] = rt_thread_create(thread_string,socket_ip6_r, (void *)i,2048, 15, 10);
			}
		}
		else
		{// ipv4	
			g_ip4[i].connected=false;
			if(socket_ip4(i))
			{
				ipv4_flag[i]=true;	
				rt_sprintf(thread_string,"socket_%d_4_w",i);
				tid_w[i] = rt_thread_create(thread_string,socket_ip4_w, (void *)i,2048, 20, 10);
				rt_sprintf(thread_string,"socket_%d_4_r",i);
				tid_r[i] = rt_thread_create(thread_string,socket_ip4_r, (void *)i,2048, 15, 10);
			}
		}
		if (tid_w[i] != RT_NULL)
			rt_thread_startup(tid_w[i]);
		if (tid_r[i] != RT_NULL)
			rt_thread_startup(tid_r[i]);		
	}
	rt_free(thread_string);
}
#ifdef RT_USING_FINSH
#include <finsh.h>
/* 输出udpclient函数到finsh shell中 */
FINSH_FUNCTION_EXPORT(socket_ctl, ctl socket);
#endif
