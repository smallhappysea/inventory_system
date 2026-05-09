#include<stdio.h>
#include<string.h>
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
    p.name[strcspn(p.name,"\n")] = 0;//去掉末尾换行符
    //strcspn获取第一个'\n'前的字符串个数换成\0就读不到了
    printf("价格：");
    //防止格式错误提前返回 scanf("%f",&p.price);//用buffer存储
    while(1){
    fgets(buffer,sizeof(buffer),stdin);
    if(sscanf(buffer,"%f",&p.price)==1) break;
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
    fwrite(&p,sizeof(Product),1,fp);    //写入结构体
    fclose(fp);
    printf("添加成功,ID=%d\n",p.id);

    FILE *log=fopen("operation.log","a");
    if(log)
    {
        time_t now=time(NULL);
        fprintf(log,"[添加] ID:%d 名称:%s 价格:%.2f 库存:%d 时间:%s",
        p.id,p.name,p.price,p.stock,ctime(&now));
        fclose(log);

    }

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