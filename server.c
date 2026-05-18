//socket第二次
//1.创建2.绑定地址端口3.监听4.接受连接5.回复6.关闭
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>//关闭socket
#include<sys/socket.h>
#include<netinet/in.h>//ipve地址结构等
#include"product.h"
void get_product_list(char *buf,int size);

void get_product_by_id(int id,char *buf,int size);

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
    //用while(1)进行多线程
    while(1)
    {
        int client_fd = accept(server_fd,NULL,NULL);
        printf("新客户端连接\n");
        //接受客户端信息
        char buffer[1024]={0};
        recv(client_fd,buffer,sizeof(buffer),0);
        printf("收到客户端消息：%s\n",buffer);
        if(strcmp(buffer,"list")==0)
        {
            char list_buff[4096];
            get_product_list(list_buff,sizeof(list_buff));
            send(client_fd,list_buff,strlen(list_buff),0);
        }
        else if(strncmp(buffer,"id=",3)==0)
        {
            int id=atoi(buffer+3);
            char replay[4096];
            get_product_by_id(id,replay,sizeof(replay));
            send(client_fd,replay,strlen(replay),0);
        }
        else{
            send(client_fd,"未知命令",8,0);
        }


        /*//回复客户端
        char *reply ="服务端已收到";
        send(client_fd,reply,strlen(reply),0);
        */
        //关闭连接
        close(client_fd);
    }
    close(server_fd);
    return 0;

    
}

void get_product_list(char *buf,int size)
{
    Product p;
    FILE *fp = fopen(FILE_NAME,"rb");
    if(!fp)
    {
        snprintf(buf,size,"暂无商品\n");//一般会自动换行可加\n可不加
        return;
    }
    char temp[256];
    buf[0]='\0';//初始化空字符串
    while(fread(&p,sizeof(Product),1,fp)==1)
    {
        snprintf(temp,sizeof(temp),"ID:%d 名称:%s 价格:%.2f 库存:%d\n",
        p.id,p.name,p.price,p.stock);
        strncat(buf , temp , size - strlen(buf) -1);
    }
    fclose(fp);
}


void get_product_by_id(int id,char *buf,int size)
{
    Product p;
    FILE *fp=fopen(FILE_NAME,"rb");
    if(!fp)
    {
        snprintf(buf,size,"暂无商品\n");
        return;
    }
    while(fread(&p,sizeof(Product),1,fp)==1)
    {
        if(p.id==id)
        {
            snprintf(buf,size,"ID:%d 名称:%s 价格:%.2f 库存:%d",
            p.id,p.name,p.price,p.stock);
            fclose(fp);
            return ;
        }
    }
    snprintf(buf,size,"商品ID %d 不存在",id);
    fclose(fp);
}