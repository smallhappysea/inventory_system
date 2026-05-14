#include<stdio.h>
#include<string.h>
#include<stdlib.h>//解决没有realloc和free
#include<time.h>
#include"product.h"
#define FILE_NAME "product.dat"//把product.dat宏定义为FILE_NAME

//以下为静态函数（static 只在当前文件内可见，不会被其他函数调用）
static int get_next_id()//自动生成不重复的商品id
{
    FILE *fp=fopen(FILE_NAME,"rb");//二进制读取
    if(!fp) return 1; //如果文件不存在则，第一个id为1
    Product p; //将product结构体用p代替
    int max=0;
    while (fread(&p,sizeof(Product),1,fp)==1)//能读到内容
    //（指针，数据单元大小，写入个数，文件指针）==1 返回值（是否成功）
    //这里的product也可以写p 编译器——自适应之力
        if(p.id > max) max = p.id;
    fclose(fp);
    return max+1;
}
//添加商品函数
void add_product(){
    Product p;
    char buffer[100];
    printf("名称：");
    fgets(p.name,MAX_NAME_LEN,stdin);//（读取的数据，大小，输入流） stdin:标准输入
    //scanf会在空格处停，而fgets是包含的，长度在第二个元素
    p.name[strcspn(p.name,"\n")] = 0;//去掉末尾换行符
    //strcspn获取第一个'\n'前的字符串个数换成\0就读不到了
    printf("价格：");
    //防止格式错误提前返回 (因为换行符的存在)scanf("%f",&p.price);//用buffer存储
    while(1){
    fgets(buffer,sizeof(buffer),stdin);
    if(sscanf(buffer,"%f",&p.price)==1) break;//（字符串，格式，存入的变量）
    //sscanf返回值不是字符串内容，而是成功解析的变量个数
    printf("价格无效,重新输入：\n");
    }
    printf("库存：");
    //scanf("%d",&p.stock);
    while(1){
    fgets(buffer,sizeof(buffer),stdin);
    if(sscanf(buffer,"%d",&p.stock)==1) break;
    printf("库存无效，重新输入\n");
    }
    //单纯getchar(); 可能无法解决多空格的情况 //吸收回车，防止影响后续fgets
    //while(getchar()!='\n');   前面已经用fgets和ssanf了，fgets会消耗

    p.id = get_next_id(); //自动生成id

    FILE *fp=fopen(FILE_NAME,"ab"); //追加二进制模式 b:二进制模式 a:追加
    if(fp==NULL)
    {
        printf("文件打开失败！\n");
        return;
    }
    fwrite(&p,sizeof(Product),1,fp);    //写入结构体（这里的1是结构体的单元个数），fp已打开的文件指针
    fclose(fp);
    printf("添加成功,ID=%d\n",p.id);

    FILE *log=fopen("operation.log","a");
    if(log)
    {
        time_t now=time(NULL);
        fprintf(log,"[添加] ID:%d 名称:%s 价格:%.2f 库存:%d 时间:%s",
        p.id,p.name,p.price,p.stock,ctime(&now));
        //返回当前静态存储的时间，多线程用ctime_r
        fclose(log);

    }

}
void stock_in()//1.输入名称 2.找到名称 3.进行添加 
{
    Product p;
    char buffer[100];
    int pid,qty,found=0;
    list_product();
    printf("请输入要入库的商品ID:");
    fgets(buffer,sizeof(buffer),stdin);
    sscanf(buffer,"%d",&pid);//这里怎么搭配的？

    printf("请输入入库数量:");
    while(1) {
        fgets(buffer,sizeof(buffer),stdin);
        if(sscanf(buffer,"%d",&qty)==1 && qty> 0 ) break;
        printf("输入错误，请输入正整数:");
    }
    //先打开文件才能进行后续真正意义上的添加
    FILE *fp=fopen(FILE_NAME,"r+b");//这里是读写模式
    if(!fp) {
        printf("无法打开文件！\n");
        return ;
    }
    while(fread(&p,sizeof(Product),1,fp)==1)
    {
        if(p.id==pid) {
            found = 1;
            p.stock += qty;
            //文件指针回退，然后用新的结构体数据进行覆盖
            fseek(fp,-sizeof(Product),SEEK_CUR);//移动指针，偏移量，文件开头移动，基准点是当前指针位置
            //复写用的，目的，不影响其他记录
            fwrite(&p,sizeof(Product),1,fp);
            break;
        }
    }
    fclose(fp);

    if(!found) {
        printf("商品ID %d 不存在！ \n",pid);
        return;
    }
    FILE *log=fopen("stock_log.txt","a");
    if(log)
    {
        time_t now=time(NULL);
        fprintf(log,"[入库] ID:%d 入库数:%d  新库存:%d 时间:%s",
        p.id,qty,p.stock,ctime(&now));
        fclose(log);

    }
    printf("入库成功！%s 的新库存为 %d\n",p.name,p.stock);
}
void delect_product()
{
    //二进制文件本身不支持“删除中间某条记录”——因此我们可以转为覆盖
    int pid,found=0;
    char buffer[100];
    Product p;
    Product *list = NULL;//不能用p *list=NULL;p是变量名 不能代替product这个类
    int count=0;//

    list_product();
    printf("请输入要删除的商品ID:");
    fgets(buffer,sizeof(buffer),stdin);
    sscanf(buffer,"%d",&pid);
    
    FILE *fp=fopen(FILE_NAME,"rb");//rb:只读二进制；r+b:读写二进制
    if(!fp) {
        printf("无法打开文件！\n");
        return ;
    }
    while(fread(&p,sizeof(Product),1,fp)==1)
    {
        if(p.id==pid) 
        {
            found = 1;
            continue;//不加入list
        }
        //动态扩容列表
        //realloc的作用是一遍读取一遍扩容
        //防止realloc失败(内存不足)，会返回null 添加检查返回值
        Product *new_list=realloc(list,(count+1)*sizeof(Product));
        if(new_list==NULL){
            printf("内存分配失败！ \n");
            free(list);
            fclose(fp);
            return;
        }
        list = new_list;
        list[count]=p;
        count++;
    }
    fclose(fp);
    if(!found) {
        printf("商品ID %d 不存在！ \n",pid);
        free(list);
        return ;
    }
    fp=fopen(FILE_NAME,"wb");
    if(!fp) {
        printf("无法打开文件写入! \n");
        free(list);
        return;
    }
    for(int i=0;i<count;i++){
        fwrite(&list[i],sizeof(Product),1,fp);
    }
    fclose(fp);
    free(list);
    FILE *log=fopen("operation.log","a");//a:追加
    if(log) {
        time_t now=time(NULL);
        fprintf(log,"[删除]ID:%d 时间：%s",pid,ctime(&now));
        fclose(log);
    }
    printf("删库成功!ID为:%d ",pid);
}
void list_product()
{
    Product p;
    FILE *fp = fopen(FILE_NAME,"rb");   //只读二进制
    if(!fp){
        printf("暂无商品\n");
        return;
    }
    printf("ID\t名称\t价格\t库存\n");   //:/t为转义文字   对应输出    使内容对齐
    while(fread(&p,sizeof(Product),1,fp)==1)
    {
        printf("%d\t%s\t%.2f\t%d\n",p.id,p.name,p.price,p.stock);
    }
    fclose(fp);
}

void modify_product() {
    int modified=0;
    int pid,found=0,choice;
    char buffer[100];
    Product p;

    list_product();
    printf("请输入要修改的商品ID:");
    fgets(buffer,sizeof(buffer),stdin);
    sscanf(buffer,"%d",&pid);

    FILE *fp=fopen(FILE_NAME,"r+b");
    if(!fp){
        printf("无法打开文件！\n");
        return ;
    }

    while(fread(&p,sizeof(Product),1,fp)==1 && !found)
    {
        if(p.id==pid)
        {
            found=1;
            printf("当前商品信息: %s,价格: %.2f,库存:%d\n",p.name,p.price,p.stock);
            printf("1.修改名称  2.修改价格  3.修改名称+价格 0.取消\n");
            fgets(buffer,sizeof(buffer),stdin);
            sscanf(buffer,"%d",&choice);
            if(choice == 1 || choice ==3){
                printf("新名称:");
                fgets(p.name,MAX_NAME_LEN,stdin);
                p.name[strcspn(p.name,"\n")]=0;
                modified=1;
            }

            if(choice==2 || choice ==3){
                printf("新价格:");
                fgets(buffer,sizeof(buffer),stdin);
                sscanf(buffer,"%f",&p.price);
                modified=1;
            }

            if(modified)
            {
                fseek(fp,-sizeof(Product),SEEK_CUR);
                fwrite(&p,sizeof(Product),1,fp);
                printf("修改成功\n");

                //日志
                FILE *log = fopen("operation.log","a");
                if(log) {
                    time_t now=time(NULL);
                    fprintf(log,"[修改] ID:%d 新名称:%s 新价格:%.2f 时间:%s",
                    p.id,p.name,p.price,ctime(&now));
                    fclose(log);
                }
            }
            else if(choice==0){
                printf("取消修改\n");
            }
            else{
                printf("无效选项,不做修改");
            }
            break;
        }
    }
    fclose(fp);
    if(!found)
    {
        printf("商品ID不存在!\n");
        return;
    }
}