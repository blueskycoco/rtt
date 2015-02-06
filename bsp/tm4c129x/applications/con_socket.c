#include "con_socket.h"
#include <rtdevice.h>
#include <lwip/netdb.h>
#include <lwip/sockets.h>
/*client use socket,server use netconn*/
#define BUF_SIZE 1024
rt_thread_t tid_w[4]={RT_NULL,RT_NULL,RT_NULL,RT_NULL},tid_r[4]={RT_NULL,RT_NULL,RT_NULL,RT_NULL};
extern struct rt_semaphore fifo_sem;
bool is_right(char config,char flag)
{
	if((config&flag)==flag)
		return true;
	else
		return false;
}
void socket_ip6_w(void *paramter)
{
	int dev=(int)paramter;
	rt_size_t data_size;
	const void *last_data_ptr;
	int status;
	rt_kprintf("socket_ip6_w==> %d , %s mode, %s , %s . Thread Enter\r\n",dev,is_right(g_conf.config[dev],CONFIG_SERVER)?"Server":"Client",is_right(g_conf.config[dev],CONFIG_IPV6)?"IPV6":"IPV4",is_right(g_conf.config[dev],CONFIG_TCP)?"TCP":"UDP");
	while(1)
	{
		if(g_ip6[dev].connected==false)
		{
			rt_thread_delay(10);
			continue;
		}
		cnn_out(dev,1);
		rt_data_queue_pop(&g_data_queue[dev*2], &last_data_ptr, &data_size, RT_WAITING_FOREVER);
		if(is_right(g_conf.config[dev],CONFIG_TCP))
		{
			if(is_right(g_conf.config[dev],CONFIG_SERVER))
			{
				status=send(g_ip6[dev].clientfd, last_data_ptr, data_size, 0);
			}	
			else
			{
				status=send(g_ip6[dev].sockfd, last_data_ptr, data_size, 0);
			}
		}
		else
		{
			if(is_right(g_conf.config[dev],CONFIG_SERVER))
				status=sendto(g_ip6[dev].sockfd, last_data_ptr, data_size, 0, (struct sockaddr *)&g_ip6[dev].client_addr6, sizeof(g_ip6[dev].client_addr6));
			else
				status=sendto(g_ip6[dev].sockfd, last_data_ptr, data_size, 0, (struct sockaddr *)&g_ip6[dev].server_addr6, sizeof(g_ip6[dev].server_addr6));
		}
		//rt_kprintf("socket_ip6_w status %d\n",status);
		if( status<= 0)
		{
			rt_kprintf("Thread ip6_w%d send error\n",dev);
			if(is_right(g_conf.config[dev],CONFIG_TCP))
			{
				if(is_right(g_conf.config[dev],CONFIG_SERVER))
				{	
					//server&tcp mode need closesocket clientfd
						closesocket(g_ip6[dev].clientfd);
				}
				else
				{	
					//server&tcp mode need closesocket clientfd
						closesocket(g_ip6[dev].sockfd);
						g_ip6[dev].sockfd= socket(PF_INET6, SOCK_STREAM, 0);
				}
				g_ip6[dev].connected=false;
				cnn_out(dev,0);
			}
		}
	}
}
void socket_ip6_r(void *paramter)
{
	int dev=(int)paramter;
	struct sockaddr_in6 server_addr6;	
	int status;
	struct sockaddr_in6 client_addr6;
	rt_kprintf("socket_ip6_r==> %d , %s mode, %s , %s . Thread Enter\r\n",dev,is_right(g_conf.config[dev],CONFIG_SERVER)?"Server":"Client",is_right(g_conf.config[dev],CONFIG_IPV6)?"IPV6":"IPV4",is_right(g_conf.config[dev],CONFIG_TCP)?"TCP":"UDP");
	while(1)
	{
		if(g_ip6[dev].connected==false)
		{
			if(is_right(g_conf.config[dev],CONFIG_SERVER))
			{				
				rt_kprintf("socket_ip6_r %d to accept %d\n",dev,g_ip6[dev].sockfd);
				rt_uint32_t  sin_size = sizeof(struct sockaddr_in6);
				g_ip6[dev].clientfd = accept(g_ip6[dev].sockfd, (struct sockaddr *)&client_addr6, &sin_size);
				rt_kprintf("socket_ip6_r %d I got a connection from (IP:%s, PORT:%d\n) fd %d\n", dev,inet6_ntoa(client_addr6.sin6_addr), ntohs(client_addr6.sin6_port),g_ip6[dev].clientfd);
				g_ip6[dev].connected=true;
				cnn_out(dev,1);
				char a=1;
				setsockopt(g_ip6[dev].clientfd, SOL_SOCKET, SO_KEEPALIVE, &a, sizeof(char));
			}
			else
			{
				status = connect(g_ip6[dev].sockfd, (struct sockaddr *)&g_ip6[dev].server_addr6, sizeof(g_ip6[dev].server_addr6));
				if(status < 0)
				{
					closesocket(g_ip6[dev].sockfd);
					g_ip6[dev].sockfd= socket(PF_INET6, SOCK_STREAM, 0);				
					//rt_kprintf("%d socket_ip6_r connect ...\n",dev);					
				}
				else
				{
					g_ip6[dev].connected=true;
					cnn_out(dev,1);
				}
			}			
		}
		if(g_ip6[dev].connected==false)
		{
			rt_thread_delay(10);			
			continue;
		}
		cnn_out(dev,1);
		socklen_t clientlen = sizeof(g_ip6[dev].server_addr6);
		if(is_right(g_conf.config[dev],CONFIG_TCP))
		{

			if(is_right(g_conf.config[dev],CONFIG_SERVER))
			{
				status=recv(g_ip6[dev].clientfd, g_ip6[dev].recv_data, BUF_SIZE, 0);					
			}
			else
			{
				status=recv(g_ip6[dev].sockfd, g_ip6[dev].recv_data, BUF_SIZE, 0);
			}
			if(status>0)
			{
				rt_data_queue_push(&g_data_queue[dev*2+1], g_ip6[dev].recv_data, status, RT_WAITING_FOREVER);
			}
			else
			{
				rt_kprintf("Thread ip6_r_%d recv error,connection lost\n",dev);
				if(is_right(g_conf.config[dev],CONFIG_SERVER))
				{
					//server&tcp mode need closesocket clientfd
						closesocket(g_ip6[dev].clientfd);
				}
				else
				{
						closesocket(g_ip6[dev].sockfd);
						g_ip6[dev].sockfd= socket(PF_INET6, SOCK_STREAM, 0);
				}
				g_ip6[dev].connected=false;
				cnn_out(dev,0);
			}

		}
		else
		{	
			status=recvfrom(g_ip6[dev].sockfd, g_ip6[dev].recv_data, BUF_SIZE, 0, (struct sockaddr *)&g_ip6[dev].server_addr6, &clientlen);
			//rt_kprintf("socket_ip6_r status %d\n",status);
			if(status>0)
			{
				rt_data_queue_push(&g_data_queue[dev*2+1], g_ip6[dev].recv_data, status, RT_WAITING_FOREVER);	
			}
			else
			{
				rt_kprintf("Thread ip6_r_%d Recvfrom error,connection lost\n",dev);
			}
		}		    
	}
}

bool socket_ip6(int dev,bool init)
{	
	if(init)
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
		/*init sockaddr_in6 */
		if(is_right(g_conf.config[dev],CONFIG_SERVER))
		{//server mode
			g_ip6[dev].server_addr6.sin6_family = AF_INET6;
			memcpy(g_ip6[dev].server_addr6.sin6_addr.s6_addr, IP6_ADDR_ANY, 16);
			g_ip6[dev].server_addr6.sin6_port = htons(g_conf.local_port[dev]);
			if(bind(g_ip6[dev].sockfd, (struct sockaddr *)&g_ip6[dev].server_addr6, sizeof(struct sockaddr)) == -1)
			{
				rt_kprintf("Bind error\n");
				closesocket(g_ip6[dev].sockfd);
				return false;
			}
			if(is_right(g_conf.config[dev],CONFIG_TCP))
			{
				rt_kprintf("socket_ip6 %d to listen %d\n",dev,g_ip6[dev].sockfd);
				if(listen(g_ip6[dev].sockfd, 1) == -1)
				{
					rt_kprintf("Listen error\n");
					closesocket(g_ip6[dev].sockfd);
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
					closesocket(g_ip6[dev].sockfd);
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
				closesocket(g_ip6[dev].sockfd);
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
					closesocket(g_ip6[dev].sockfd);
					return false;
				}
			}
			else
			{
				char a=1;
				setsockopt(g_ip6[dev].sockfd, SOL_SOCKET, SO_KEEPALIVE, &a, sizeof(char));
			}
		}
		/*mall receive buffer*/
		g_ip6[dev].recv_data = rt_malloc(BUF_SIZE);
		if(g_ip6[dev].recv_data == RT_NULL)
		{
			rt_kprintf(" ip6 %d No memory\n",dev);
			closesocket(g_ip6[dev].sockfd);
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
	}
	else
	{
		/*free receive buffer*/
		if(g_ip6[dev].recv_data)
			rt_free(g_ip6[dev].recv_data);
		/*close socket*/
		if(is_right(g_conf.config[dev],CONFIG_SERVER)&&is_right(g_conf.config[dev],CONFIG_TCP))
		{
			closesocket(g_ip6[dev].clientfd);
		}
		closesocket(g_ip6[dev].sockfd);
	}
	return true;
}
void socket_ip4_w(void *paramter)
{
	int dev=(int)paramter;
	rt_size_t data_size;
	const void *last_data_ptr;
	int status;
	rt_kprintf("socket_ip4_w==> %d , %s mode, %s , %s . Thread Enter\r\n",dev,is_right(g_conf.config[dev],CONFIG_SERVER)?"Server":"Client",is_right(g_conf.config[dev],CONFIG_IPV6)?"IPV6":"IPV4",is_right(g_conf.config[dev],CONFIG_TCP)?"TCP":"UDP");
	while(1)
	{

		if(g_ip4[dev].connected==false)
		{
			rt_thread_delay(10);
			continue;
		}
		cnn_out(dev,1);
		rt_data_queue_pop(&g_data_queue[dev*2], &last_data_ptr, &data_size, RT_WAITING_FOREVER);
		if(is_right(g_conf.config[dev],CONFIG_TCP))
		{
			if(is_right(g_conf.config[dev],CONFIG_SERVER))
			{
				status=send(g_ip4[dev].clientfd, last_data_ptr, data_size, 0);
			}
			else
			{
				status=send(g_ip4[dev].sockfd, last_data_ptr, data_size, 0);
			}
		}
		else
		{
			if(is_right(g_conf.config[dev],CONFIG_SERVER))
				status=sendto(g_ip4[dev].sockfd, last_data_ptr, data_size, 0, (struct sockaddr *)&g_ip4[dev].client_addr, sizeof(g_ip4[dev].client_addr));
			else
				status=sendto(g_ip4[dev].sockfd, last_data_ptr, data_size, 0, (struct sockaddr *)&g_ip4[dev].server_addr, sizeof(g_ip4[dev].server_addr));
			//rt_thread_delay(1);
		}
		if( status< 0)
		{
			rt_kprintf("Thread ip4_w%d send error\n",dev);
			if(is_right(g_conf.config[dev],CONFIG_TCP))
			{
				if(is_right(g_conf.config[dev],CONFIG_SERVER))
				{	
					//server&tcp mode need closesocket clientfd
						closesocket(g_ip4[dev].clientfd);
				}
				else
				{	
					//server&tcp mode need closesocket clientfd
						closesocket(g_ip4[dev].sockfd);
						g_ip4[dev].sockfd= socket(PF_INET, SOCK_STREAM, 0);
				}
				cnn_out(dev,0);
			}
		}      
	}
}
void socket_ip4_r(void *paramter)
{
	int dev=(int)paramter;
	struct sockaddr_in server_addr;	
	int status;
	struct sockaddr_in client_addr;
	rt_kprintf("socket_ip4_r==> %d , %s mode, %s , %s . Thread Enter\r\n",dev,is_right(g_conf.config[dev],CONFIG_SERVER)?"Server":"Client",is_right(g_conf.config[dev],CONFIG_IPV6)?"IPV6":"IPV4",is_right(g_conf.config[dev],CONFIG_TCP)?"TCP":"UDP");
	while(1)
	{
		if(g_ip4[dev].connected==false)
		{
			if(is_right(g_conf.config[dev],CONFIG_SERVER))
			{				
				rt_kprintf("socket_ip4_r %d to accept %d\n",dev,g_ip4[dev].sockfd);
				rt_uint32_t  sin_size = sizeof(struct sockaddr_in);
				g_ip4[dev].clientfd = accept(g_ip4[dev].sockfd, (struct sockaddr *)&client_addr, &sin_size);
				rt_kprintf("socket_ip4_r %d I got a connection from (IP:%s, PORT:%d\n) fd %d\n", dev,inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port),g_ip4[dev].clientfd);
				g_ip4[dev].connected=true;
				cnn_out(dev,1);
				char a=1;
				setsockopt(g_ip4[dev].clientfd, SOL_SOCKET, SO_KEEPALIVE, &a, sizeof(char));
			}
			else
			{
				status = connect(g_ip4[dev].sockfd, (struct sockaddr *)&g_ip4[dev].server_addr, sizeof(g_ip4[dev].server_addr));
				if(status < 0)
				{
					closesocket(g_ip4[dev].sockfd);
					g_ip4[dev].sockfd= socket(PF_INET, SOCK_STREAM, 0);
					//rt_kprintf("%d socket_ip4_r connect ...",dev);		
				}
				else
				{
					g_ip4[dev].connected=true;
					cnn_out(dev,1);
				}
			}			
		}
		if(g_ip4[dev].connected==false)
		{
			rt_thread_delay(10);			
			continue;
		}
		cnn_out(dev,1);
		socklen_t clientlen = sizeof(g_ip4[dev].server_addr);
		if(is_right(g_conf.config[dev],CONFIG_TCP))
		{

			if(is_right(g_conf.config[dev],CONFIG_SERVER))
			{
				status=recv(g_ip4[dev].clientfd, g_ip4[dev].recv_data, BUF_SIZE, 0);					
			}
			else
			{
				status=recv(g_ip4[dev].sockfd, g_ip4[dev].recv_data, BUF_SIZE, 0);
			}
			if(status>0)
			{
				rt_data_queue_push(&g_data_queue[dev*2+1], g_ip4[dev].recv_data, status, RT_WAITING_FOREVER);
			}
			else
			{
				rt_kprintf("Thread ip4_r_%d recv error,connection lost\n",dev);
				if(is_right(g_conf.config[dev],CONFIG_SERVER))
				{
					//server&tcp mode need closesocket clientfd
					
						closesocket(g_ip4[dev].clientfd);
				}
				else
				{
						rt_kprintf("to close socket\n");
						closesocket(g_ip4[dev].sockfd);
						g_ip4[dev].sockfd= socket(PF_INET, SOCK_STREAM, 0);
				}
				g_ip4[dev].connected=false;
				cnn_out(dev,0);
			}

		}
		else
		{
			status=recvfrom(g_ip4[dev].sockfd, g_ip4[dev].recv_data, BUF_SIZE, 0, (struct sockaddr *)&g_ip4[dev].server_addr, &clientlen);
			if(status>0)
			{				
				rt_data_queue_push(&g_data_queue[dev*2+1], g_ip4[dev].recv_data, status, RT_WAITING_FOREVER);
			}
			else
			{
				rt_kprintf("Thread ip4_r_%d Recvfrom error,connection lost\n",dev);
			}
		}
	}
}

bool socket_ip4(int dev,bool init)
{	
	if(init)
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
				closesocket(g_ip4[dev].sockfd);
				return false;
			}
			if(is_right(g_conf.config[dev],CONFIG_TCP))
			{
				rt_kprintf("to listen %d\n",g_ip4[dev].sockfd);
				if(listen(g_ip4[dev].sockfd, 1) == -1)
				{
					rt_kprintf("Listen error\n");
					closesocket(g_ip4[dev].sockfd);
					return false;
				}
			}
			if(!is_right(g_conf.config[dev],CONFIG_TCP))
			{
				memset(&g_ip4[dev].client_addr, 0, sizeof(g_ip4[dev].client_addr));
				g_ip4[dev].client_addr.sin_family = AF_INET;
				g_ip4[dev].client_addr.sin_port = htons(g_conf.remote_port[dev]);
				rt_memset(&(g_ip4[dev].client_addr.sin_zero),8, sizeof(g_ip4[dev].client_addr.sin_zero));
				if(inet_pton(AF_INET, (char *)g_conf.remote_ip[dev], &g_ip4[dev].client_addr.sin_addr.s_addr) != 1)
				{
					rt_kprintf("inet_pton() error\n");
					closesocket(g_ip4[dev].sockfd);
					return false;
				}
			}
		}
		else
		{//client mode
			memset(&g_ip4[dev].server_addr, 0, sizeof(g_ip4[dev].server_addr));
			g_ip4[dev].server_addr.sin_family = AF_INET;
			g_ip4[dev].server_addr.sin_port = htons(g_conf.remote_port[dev]);
			rt_memset(&(g_ip4[dev].server_addr.sin_zero),8, sizeof(g_ip4[dev].server_addr.sin_zero));
			if(inet_pton(AF_INET, (char *)g_conf.remote_ip[dev], &g_ip4[dev].server_addr.sin_addr.s_addr) != 1)
			{
				rt_kprintf("inet_pton() error\n");
				closesocket(g_ip4[dev].sockfd);
				return false;
			}
			if(!is_right(g_conf.config[dev],CONFIG_TCP))
			{
				g_ip4[dev].client_addr.sin_family = AF_INET;
				g_ip4[dev].client_addr.sin_addr.s_addr = INADDR_ANY;
				g_ip4[dev].client_addr.sin_port = htons(g_conf.local_port[dev]);
				rt_memset(&(g_ip4[dev].client_addr.sin_zero),8, sizeof(g_ip4[dev].client_addr.sin_zero));
				if(bind(g_ip4[dev].sockfd, (struct sockaddr *)&g_ip4[dev].client_addr, sizeof(struct sockaddr)) == -1)
				{
					rt_kprintf("Bind error\n");
					closesocket(g_ip4[dev].sockfd);
					return false;
				}
			}
			else
			{
				char a=1;
				setsockopt(g_ip6[dev].sockfd, SOL_SOCKET, SO_KEEPALIVE, &a, sizeof(char));
			}
		}
		/*mall receive buffer*/
		g_ip4[dev].recv_data = rt_malloc(BUF_SIZE);
		if(g_ip4[dev].recv_data == RT_NULL)
		{
			rt_kprintf(" socket_ip4 %d No memory\n",dev);
			closesocket(g_ip4[dev].sockfd);
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
	}
	else
	{
		/*free receive buffer*/
		if(g_ip4[dev].recv_data)
			rt_free(g_ip4[dev].recv_data);
		/*close socket*/
		if(is_right(g_conf.config[dev],CONFIG_SERVER)&&is_right(g_conf.config[dev],CONFIG_TCP))
		{
			closesocket(g_ip4[dev].clientfd);
		}
		closesocket(g_ip4[dev].sockfd);
	}
	return true;
}

/*init socket 1,2,3,4*/
void socket_ctl(bool open)
{
	rt_uint8_t *thread_string;
	int i;
	rt_kprintf("g_conf.config %x,%x,%x,%x\r\n",g_conf.config[0],g_conf.config[1],g_conf.config[2],g_conf.config[3]);
	thread_string=(rt_uint8_t *)rt_malloc(20*sizeof(rt_uint8_t));
	for(i=0;i<4;i++)
	{
		
		rt_memset(thread_string,'\0',20);
		rt_kprintf("%s Socket==> %d , %s mode, %s , %s . Thread Enter\r\n",open?"Create":"Delete",i,is_right(g_conf.config[i],CONFIG_SERVER)?"Server":"Client",is_right(g_conf.config[i],CONFIG_IPV6)?"IPV6":"IPV4",is_right(g_conf.config[i],CONFIG_TCP)?"TCP":"UDP");
		if(open)
		{
			g_ip6[i].connected=false;
			g_ip4[i].connected=false;
			if(tid_w[i]==RT_NULL && tid_r[i]==RT_NULL)
			{
				if(is_right(g_conf.config[i],CONFIG_IPV6))
				{//udp client ipv6
					if(socket_ip6(i,true))
					{
						rt_sprintf(thread_string,"socket_%d_6_w",i);
						tid_w[i] = rt_thread_create(thread_string,socket_ip6_w, (void *)i,2048, 20, 10);
						rt_sprintf(thread_string,"socket_%d_6_r",i);
						tid_r[i] = rt_thread_create(thread_string,socket_ip6_r, (void *)i,2048, 15, 10);
					}
				}
				else
				{//udp client ipv4	
					if(socket_ip4(i,true))
					{
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
		}
		else
		{
			if(tid_w[i]!=RT_NULL && tid_r[i]!=RT_NULL)
			{
				int set=1;
				if(is_right(g_conf.config[i],CONFIG_SERVER))
				{
	
					rt_thread_delete(tid_w[i]);
					rt_thread_delete(tid_r[i]);
					tid_w[i]=RT_NULL;
					tid_r[i]=RT_NULL;
					if(is_right(g_conf.config[i],CONFIG_IPV6))
					{
						closesocket(g_ip6[i].clientfd);
						closesocket(g_ip6[i].sockfd); 
						rt_free(g_ip6[i].recv_data);
						
					}
					else
					{
						closesocket(g_ip4[i].clientfd);
						closesocket(g_ip4[i].sockfd); 
						rt_free(g_ip4[i].recv_data);
					
					}

				}
				else
				{				
					if(is_right(g_conf.config[i],CONFIG_IPV6))
					{
						if(g_ip6[i].connected==false)
						{
							closesocket(g_ip6[i].clientfd);
							closesocket(g_ip6[i].sockfd); 
							rt_free(g_ip6[i].recv_data);
							rt_thread_delete(tid_w[i]);
							rt_thread_delete(tid_r[i]);
							tid_w[i]=RT_NULL;
							tid_r[i]=RT_NULL;
						}
						else
						{
							rt_thread_delete(tid_w[i]);
							rt_thread_delete(tid_r[i]);
							tid_w[i]=RT_NULL;
							tid_r[i]=RT_NULL;
							closesocket(g_ip6[i].clientfd);
							closesocket(g_ip6[i].sockfd); 
							rt_free(g_ip6[i].recv_data);
						}
						
					}
					else
					{
						if(g_ip4[i].connected==false)
						{
							closesocket(g_ip4[i].clientfd);
							closesocket(g_ip4[i].sockfd); 
							rt_free(g_ip4[i].recv_data);
							rt_thread_delete(tid_w[i]);
							rt_thread_delete(tid_r[i]);
							tid_w[i]=RT_NULL;
							tid_r[i]=RT_NULL;
						}
						else
						{
							rt_thread_delete(tid_w[i]);
							rt_thread_delete(tid_r[i]);
							tid_w[i]=RT_NULL;
							tid_r[i]=RT_NULL;
								closesocket(g_ip4[i].clientfd);
							closesocket(g_ip4[i].sockfd); 
							rt_free(g_ip4[i].recv_data);
						}
					}
					
				}
			}
		}
	}
	rt_free(thread_string);
}
#ifdef RT_USING_FINSH
#include <finsh.h>
/* 输出udpclient函数到finsh shell中 */
FINSH_FUNCTION_EXPORT(socket_ctl, ctl socket);
#endif
