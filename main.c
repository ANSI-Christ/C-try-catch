#define TRY_CATCH_IMPL
#include "trycatch.h"

void test_1(){
    TRY(
        THROW(int,4);
        printf("cant get here\n");
    )CATCH(int, x)(
        printf("catch int %d\n",x);
    )
}

void test_2(){
    TRY(
        THROW(float,3.3);
        printf("cant get here\n");
    )CATCH(int, x)(
        printf("catch int %d\n",x);
    )CATCH(float)(
        printf("catch float\n");
    )
}

void test_3(){
    TRY(
        THROW(const char *,"bla");
        printf("cant get here\n");
    )CATCH()(
        printf("catch %s from %s\n",EXCEPTION->type,EXCEPTION->where);
    )
}

void test_4(){
    TRY(
        TRY(
            THROW(const char *,"bla");
            printf("cant get here\n");
        )CATCH(int)(
            printf("cant get here\n");
        )
        printf("cant get here\n");
    )CATCH(const char *,s)(
        printf("catch string %s\n",s);
    )
    FINALLY(
        printf("finally\n");
    )
}

struct t_ExceptionUser{
    int a;
    float b;
};

void test_5(){
    TRY(
        THROW(struct t_ExceptionUser,{3,1.1});
        printf("cant get here\n");
    )CATCH(struct t_ExceptionUser,u)(
        printf("catch user %d / %f\n",u.a,u.b);
    )
}
  
int main()
{
    TryCatchInit();
    test_1();
    test_2();
    test_3();
    test_4();
    test_5();
    TryCatchClose();
}
