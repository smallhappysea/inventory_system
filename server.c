//socket第二次
//1.创建2.绑定地址端口3.监听4.接受连接5.回复6.关闭
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>//关闭socket
#include<sys/socket.h>
#include<netinet/in.h>//ipve地址结构等
#include"product.h"
#include <pthread.h>
#include <string.h>
pthread_mutex_t file_mutex = PTHREAD_MUTEX_INITIALIZER;

void get_product_list(char *buf,int size);

void get_product_by_id(int id,char *buf,int size);

void *handle_client(void *arg);

void add_product_remote(const char *name,float price,int stock) ;

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
    //用while(1)进行持续接受客户端信息
    while(1)
    {
        int client_fd = accept(server_fd,NULL,NULL);
        printf("新客户端连接\n");
        
        pthread_t tid;
        pthread_create(&tid,NULL,handle_client,(void*)(long)client_fd);
        pthread_detach(tid);//结束进程
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


//多线程并发
void *handle_client(void *arg)
{
    int client_fd=(int)(long)arg;//安全转化

    char buffer[1024]={0};
    printf("新客户端连接\n");
    //接受客户端信息
    while(1)
    {
        memset(buffer,0,sizeof(buffer));
        int len= recv(client_fd,buffer,sizeof(buffer)-1,0);
        if(len<=0)
        {
            if(len==0) printf("客户端断开连接\n");
            else
                perror("recv");
                break;
        }
        buffer[len]='\0';//防止越界
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
        else if(strncmp(buffer,"add,",4)==0){
            char name[100];
            float price;
            int stock;
            if(sscanf(buffer,"add,%[^,],%f,%d",name,&price,&stock)==3){
                add_product_remote(name,price,stock);
                send(client_fd,"添加成功\n",10,0);
            }
            else{
                char msg[]="格式错误,正确格式为:add,名称,价格,库存\n";
                send(client_fd,msg,strlen(msg),0);//第四个参数用strlen,灵活借用读取的字符串，防止越界
            }
        }
        else if(strncmp(buffer,"quit",4)==0)
        {
            send(client_fd,"bye\n",4,0);
            break;
        }
        else{
            send(client_fd,"未知命令",8,0);
        }
    }
    /*//回复客户端
        char *reply ="服务端已收到";
        send(client_fd,reply,strlen(reply),0);
        */
        //关闭连接
    close(client_fd);
    return NULL;
}

void add_product_remote(const char *name,float price,int stock)
{
    Product p;
    FILE *fp =fopen(FILE_NAME,"rb");
    int max_id=0;
    if(fp) {
        Product temp;
        while (fread(&temp,sizeof(Product),1,fp)==1)
        {
            if(temp.id>max_id) max_id=temp.id;
            
        }
        fclose(fp);
        p.id=max_id+1;
        strncpy(p.name,name,MAX_NAME_LEN-1);
        p.name[MAX_NAME_LEN-1]='\0';
        p.price=price;
        p.stock=stock;
        
        pthread_mutex_lock(&file_mutex);
        fp=fopen(FILE_NAME,"ab");
        if(fp) {
            fwrite(&p,sizeof(Product),1,fp);
            fclose(fp);
        }
        pthread_mutex_unlock(&file_mutex);
    }
}
//断开连接recv返回<=0