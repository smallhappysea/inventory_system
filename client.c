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
    printf("连接服务器成功,输入命令(list / get ID / quit)\n");

    //发送消息
    char input[256];
    char buffer[4096]={0};
    while(1)
    {
        printf("> ");
        fflush(stdout);
        if(!fgets(input,sizeof(input),stdin)) break;
        input[strcspn(input,"\n")]=0;

        if(strcmp(input,"quit")==0) break;
        send(sock,input,strlen(input),0);

        int len=recv(sock,buffer,sizeof(buffer)-1,0);
        if(len<=0)
        {
        
            printf("断开连接/接受失败");
            break;
        }
        buffer[len]='\0';
        printf("%s\n",buffer);
    }

    //接收消息
    
    
    

    //关闭
    close(sock);
    return 0;
}