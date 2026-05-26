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
    if(connect(sock,(struct sockaddr*)&server_addr,sizeof(server_addr)) < 0)//单纯break会导致就算没连接也会输出连接成功
    {
        perror("connect failed\n");
        close(sock);
        return 1;
    }
    printf("连接服务器成功,输入命令(list / get ID / add / quit)\n");

    //发送
    char input[256];
    char buffer[4096]={0};
    while(1)
    {
        printf("> ");
        fflush(stdout);
        if(!fgets(input,sizeof(input),stdin)) break;
        input[strcspn(input,"\n")]=0;

        if(strcmp(input,"quit")==0) break;
        if(send(sock,input,strlen(input),0) < 0)
        {
            perror("send");
            break;
        }

        int len=recv(sock,buffer,sizeof(buffer)-1,0);
        if(len<=0)
        {
            if(len == 0) printf("服务器关闭连接\n");
            else perror("recv");
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