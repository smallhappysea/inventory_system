#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
int main()
{
    //创建
    int sock=socket(AF_INET,SOCK_STREAM,0);
    
    //创建地址
    struct sockaddr_in server_addr;
    server_addr.sin_family=AF_INET;
    server_addr.sin_port=htons(8888);
    inet_pton(AF_INET,"127.0.0.1",&server_addr.sin_addr);

    //连接服务器
    connect(sock,(struct sockaddr*)&server_addr,sizeof(server_addr));
    printf("连接服务器成功\n");

    //发送消息
    char *msg = "list";
    send(sock,msg,strlen(msg),0);

    //接收消息
    char buffer[1024]={0};
    int len=recv(sock,buffer,sizeof(buffer)-1,0);
    if(len>0)
    {
        buffer[len]='\0';
        printf("商品列表：\n%s\n",buffer);
    }else{
        printf("接受失败或连接关闭\n");
    }
    

    //关闭
    close(sock);
    return 0;
}