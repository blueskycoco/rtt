#include "lwip/opt.h"
#include "lwip/tcp.h"
#include "con_socket.h"
/*client use socket,server use netconn*/
extern struct rt_data_queue g_data_queue[8];
#define BUF_SIZE 1024
rt_thread_t tid_w[4],tid_r[4];

typedef struct ip6
{
	struct sockaddr_in6 server_addr6;
	int sockfd;
	int clientfd;
	char *recv_data;
	bool connected;
}ip6_t,*pip6_t;
typedef struct ip4
{
	struct sockaddr_in server_addr;
	int sockfd;
	char *recv_data;
}ip4_t,*pip4_t;

ip6_t g_ip6[4];
ip4_t g_ip4[4];
bool is_right(char config,char flag)
{
	if((config&flag)==flag)
		return true;
	else
		return false;
}
void ip6_w(void *paramter)
{
	int dev=*(int *)paramter;
	rt_size_t data_size;
	const void *last_data_ptr;
	int status;
	struct sockaddr_in6 client_addr6;
	if(g_ip6[dev].connected==false)
	{
		if(is_right(g_conf.config[dev],CONFIG_SERVER))
		{
			if(bind(g_ip6[dev].sockfd, (struct sockaddr *)&g_ip6[dev].server_addr6, sizeof(struct sockaddr)) == -1)
			{
				rt_kprintf("Bind error\n");
				closesocket(g_ip6[dev].sockfd);
				rt_free(g_ip6[dev].recv_data);
				return ;
			}
			if(listen(g_ip6[dev].sockfd, 1) == -1)
			{
				rt_kprintf("Listen error\n");
				rt_free(g_ip6[dev].recv_data);
				closesocket(g_ip6[dev].sockfd);
				return ;
			}
			rt_uint32_t  sin_size = sizeof(struct sockaddr_in6);
			g_ip6[dev].clientfd = accept(g_ip6[dev].sockfd, (struct sockaddr *)&client_addr6, &sin_size);
			rt_kprintf("I got a connection from (IP:%s, PORT:%d\n)", inet6_ntoa(client_addr6.sin6_addr), ntohs(client_addr6.sin6_port));
		}
		else
		{
			status = connect(g_ip6[dev].sockfd, (struct sockaddr *)&g_ip6[dev].server_addr6, sizeof(g_ip6[dev].server_addr6));
			if(status < 0)
			{
				rt_kprintf("Thread client6_r_%d Connect error:%d\n", dev,status);
				rt_free(g_ip6[dev].recv_data);
				closesocket(g_ip6[dev].sockfd);
				return ;
			}
		}
		g_ip6[dev].connected=true;
	}

	while(1)
	{
		rt_data_queue_pop(&g_data_queue[dev*2], &last_data_ptr, &data_size, RT_WAITING_FOREVER);
		if(data_size!=0 && last_data_ptr)
		{			
			char *ptr=(char *)rt_malloc((data_size+1)*sizeof(char));
			rt_memcpy(ptr,last_data_ptr,data_size);
			ptr[data_size]='\0';
			rt_kprintf("=>%d\n%s",data_size,ptr);
			rt_free(ptr);
		}
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
			status=sendto(g_ip6[dev].sockfd, last_data_ptr, data_size, 0, (struct sockaddr *)&g_ip6[dev].server_addr6, sizeof(g_ip6[dev].server_addr6));
		}
		if( status< 0)
		{
			rt_kprintf("Thread client6_w_%d Sendto error\n",dev);
			closesocket(g_ip6[dev].sockfd);
			if(is_right(g_conf.config[dev],CONFIG_SERVER)&&is_right(g_conf.config[dev],CONFIG_TCP))
			{
				closesocket(g_ip6[dev].clientfd);
			}
			rt_free(g_ip6[dev].recv_data);
			return ;
		}
	}
}
void ip6_r(void *paramter)
{
	int dev=*(int *)paramter;
	struct sockaddr_in6 server_addr6;	
	int status;
	while(1)
	{
		socklen_t clientlen = sizeof(g_ip6[dev].server_addr6);
		if(g_conf.tcp[dev])
		{
			if(g_ip6[dev].connected)
			{
				if(g_ip6[dev].clientfd!=0)
				{
					status=recv(g_ip6[dev].sockfd, g_ip6[dev].recv_data, BUF_SIZE, 0);
					if(status>0)
					{
						rt_kprintf("Thread client6_r %d got '%s'\n", status,g_ip6[dev].recv_data);
						rt_data_queue_push(&g_data_queue[dev*2+1], g_ip6[dev].recv_data, status, RT_WAITING_FOREVER);
					}
					else
					{
						rt_kprintf("Thread client6_r_%d Recvfrom error\n",dev);
						rt_free(g_ip6[dev].recv_data);
						closesocket(g_ip6[dev].clientfd);
						closesocket(g_ip6[dev].sockfd);
						return ;
					}
				}
				else
				{
					rt_kprintf("Thread ip6_r_%d need wait connection in\n",dev);
					rt_thread_delay(10);
				}
			}
			else
			{
				rt_kprintf("Thread ip6_r_%d need wait connect to server\n",dev);
				rt_thread_delay(10);
			}
		}
		else
		{
			status=recvfrom(g_ip6[dev].sockfd, g_ip6[dev].recv_data, BUF_SIZE, 0, (struct sockaddr *)&g_ip6[dev].server_addr6, &clientlen);
			if(status>0)
			{
				rt_kprintf("Thread client6_r %d got '%s'\n", clientlen,g_ip6[dev].recv_data);
				rt_data_queue_push(&g_data_queue[dev*2+1], g_ip6[dev].recv_data, clientlen, RT_WAITING_FOREVER);
			}
			else
			{
				rt_kprintf("Thread client6_r_%d Recvfrom error\n",dev);
				rt_free(g_ip6[dev].recv_data);
				closesocket(g_ip6[dev].sockfd);
				return ;
			}
		}
	}
}

bool ip6(int dev,bool init)
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
			g_ip6[dev].sin6_family = AF_INET6;
			memcpy(g_ip6[dev].sin6_addr.s6_addr, IP6_ADDR_ANY, 16);
			g_ip6[dev].sin6_port = htons(g_conf.local_port);
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
		}
		/*mall receive buffer*/
		g_ip6[dev].recv_data = rt_malloc(BUF_SIZE);
		if(g_ip6[dev].recv_data == RT_NULL)
		{
			rt_kprintf(" udpclient6_r_%d No memory\n",dev);
			closesocket(g_ip6[dev].sockfd);
			return false;
		}
		if(is_right(g_conf.config[dev],CONFIG_TCP))
			g_ip6[dev].connected=false;
		else
			g_ip6[dev].connected=true;	
		g_ip6[dev].clientfd=0;
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

/*init socket 1,2,3,4*/
int socket_init()
{
	rt_uint8_t *thread_string;
	int i;
	
	thread_string=(rt_uint8_t *)rt_malloc(20*sizeof(rt_uint8_t));
	for(i=0;i<4;i++)
	{
		rt_memset(thread_string,'\0',20);
		rt_kprintf("Socket==> %d , %s mode, %s , %s . Thread Enter\r\n",i,is_right(g_conf.config[i],CONFIG_SERVER)?"Server":"Client",is_right(g_conf.config[i],CONFIG_IPV6)?"IPV6":"IPV4",is_right(g_conf.config[i],CONFIG_TCP)?"TCP":"UDP");
		if(is_right(g_conf.config[i],CONFIG_IPV6))
		{//udp client ipv4
			ip6(i,true);
			rt_sprintf(thread_string,"socket_%d_4_w",i);
			tid_w[i] = rt_thread_create(thread_string,ip6_w, (void *)(&i),2048, 20, 10);
			rt_sprintf(thread_string,"socket_%d_4_r",i);
			tid_r[i] = rt_thread_create(thread_string,ip6_r, (void *)(&i),2048, 20, 10);
		}
		else
		{//udp client ipv6	
			ip4(i,true);
			rt_sprintf(thread_string,"socket_%d_6_w",i);
			tid_w[i] = rt_thread_create(thread_string,ip4_w, (void *)(&i),2048, 20, 10);
			rt_sprintf(thread_string,"socket_%d_6_r",i);
			tid_r[i] = rt_thread_create(thread_string,ip4_r, (void *)(&i),2048, 20, 10);
		}
	
		if (tid_w[i] != RT_NULL)
			rt_thread_startup(tid_w[i]);
		if (tid_r[i] != RT_NULL)
			rt_thread_startup(tid_r[i]);
	}
	rt_free(thread_string);
}
