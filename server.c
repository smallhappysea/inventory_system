//socket第二次
//1.创建2.绑定地址端口3.监听4.接受连接5.回复6.关闭
#include<stdio.h>
#include<string.h>
#include<unistd.h>//关闭socket
#include<sys/socket.h>
#include<netinet/in.h>//ipve地址结构等

int main()
{
    //创建
    int server_fd=socket(AF_INET,SOCK_STREAM,0);

    //绑定端口与地址
    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port=htons(8888);
    addr.sin_addr.s_addr=INADDR_ANY;
    bind(server_fd,(struct sockaddr*)&addr,sizeof(addr));

    //监听
    listen(server_fd,5);
    printf("服务器启动，等待连接...\n");

    //接受客户端连接(阻塞)
    int client_fd = accept(server_fd,NULL,NULL);

    //接受客户端信息
    char buffer[1024]={0};
    recv(client_fd,buffer,sizeof(buffer),0);
    printf("收到客户端消息：%s\n",buffer);

    //回复客户端
    char *reply ="服务端已收到";
    send(client_fd,reply,strlen(reply),0);

    //关闭连接
    close(client_fd);
    close(server_fd);
    return 0;

    
}
