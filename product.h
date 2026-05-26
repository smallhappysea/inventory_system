#ifndef PRODUCT_H //这里的意思是if no define 即定义条件
#define PRODUCT_H
#define MAX_NAME_LEN 100
#define FILE_NAME "product.dat"//宏定义
typedef struct {
    int id;
    char name[MAX_NAME_LEN];
    float price;
    int stock;
}Product;
void add_product();
void list_product();
void stock_in();
void delect_product();
void modify_product();
#endif // !PRODUCT_H //结束