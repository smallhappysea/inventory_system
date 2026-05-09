#include<stdio.h>
#include<stdlib.h>//用于exit函数
#include "product.h"
int main()
{
    int choice;
    while(1) {
        printf("\n1.添加 2.查看 3.退出\n");
        if(scanf("%d",&choice)!=1)
        {
            while(getchar()!='\n');
            printf("无效输入，请输入数字\n");
            continue;
        }
        while(getchar()!='\n');
        switch (choice)
        {
        case 1:
            add_product();
            break;
        case 2:
            list_product();
            break;
        case 3:
            exit(0);
        default:
            printf("无效选择\n");
        }
    }
    return 0;
}