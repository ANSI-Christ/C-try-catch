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

void _test_6(){
    TRY(
        THROW(int,999);
        printf("cant get here\n");
    )CATCH(struct t_ExceptionUser,u)(
        printf("catch user %d / %f\n",u.a,u.b);
    )
}

void test_6(){
    TRY(
        _test_6();
        printf("cant get here\n");
    )CATCH(int,x)(
        printf("catch int %d\n",x);
    )
}


const char *execName;

const char *_addr2lineOpt(){
    static const char *s=(void*)0x1;
    if(s==(void*)0x1){
        if(system("addr2line -h 1,2>log"))
            s=NULL;
        else{
            const char *opt[]={" ","-p","-a","-a -p"};
            s=opt[((!system("addr2line -a -h 1,2>log"))<<1) | (!system("addr2line -p -h 1,2>log"))];
        }
    }
    return s;
}

void addr2line(void * const *address,unsigned int count){
    if(execName){
        const char *opt=_addr2lineOpt();
        if(opt){
            char str[2048];
            unsigned int l=snprintf(str,sizeof(str)-1,"addr2line %s -f -e \"%s\"",opt,execName);
            while(count--) l+=snprintf(str+l,sizeof(str)-l-1," %p",address[count]);
            system(str);
            return;
        }
    }
    while(count--) printf("%p: ?? at ??:0\n",address[count]);
}



typedef struct{
    int sig,count;
    void *trace[8];
}t_exceptionSignal;

void test_7(){
    TRY(
        void **a=(void*)123;
        printf("SIGSEGV %p\n",*a);
    )CATCH(t_exceptionSignal,sig)(
        printf("catch sig %d\n",sig.sig);
        addr2line(sig.trace,sig.count);
    )
}

#include <signal.h>
#include <execinfo.h>

void sigHandler(int sig){
    signal(sig,sigHandler);
    {
        t_exceptionSignal e={sig};
        e.count=backtrace(e.trace,sizeof(e.trace)/sizeof(*e.trace));
        THROW(t_exceptionSignal,e);
    }
}

void sigInitializer(void *arg){
    signal(SIGFPE,sigHandler);
    signal(SIGSEGV,sigHandler);
}
  
int main(int argc,char **argv)
{
    execName=argv[0];
    if(TryCatchInit()) atexit(TryCatchClose);
    TryCatchSetSignalInitializer(sigInitializer,NULL);
    
    test_1();
    test_2();
    test_3();
    test_4();
    test_5();
    test_6();
    test_7();
    
    return 0;
}
