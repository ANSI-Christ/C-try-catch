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
            goto _finally_1;
        )
        printf("cant get here\n");
        if(0){
            _finally_1:
            printf("finally 1\n");
        }
    )CATCH(const char *,s)(
        printf("catch string %s\n",s);
        goto _finally_2;
    )

    if(0){
        _finally_2:
        printf("finally 2\n");
    }
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
    int i=15;
    while(i--){
        TRY(
            if(i%2){
                void **a=(void*)123;
                printf("SIGSEGV %p\n",*a);
            }else{
                int x=(i%2)>>30;
                printf("SIGFPE %d\n",10/x);
            }
        )CATCH(t_exceptionSignal,sig)(
            printf("catch sig %d\n",sig.sig);
            addr2line(sig.trace,sig.count);
        )
    }
}

void test_8(void){
    TRY(
        TRY(
            THROW(int,1);
        )CATCH()(
            TRY(
                THROW(float,2.3);
            )CATCH()(

            )
            THROW();
        )
    )CATCH(int,x)(
        printf("got correct level %d\n",x);
    )CATCH(float,x)(
        printf("got wrong level %f\n",x);
    )
}





#include <signal.h>

#ifdef _WIN32

#include <windows.h>

static void set_sighandler(const int s,void (* const f)(int)){
    signal(s,f);
}

#define backtrace(_arr_,_cnt_) CaptureStackBackTrace(0,(_cnt_),(_arr_),NULL);

#else

extern int backtrace(void**,int);

static void set_sighandler(const int s,void (* const f)(int)){
    signal(s,f);
    {sigset_t set[1];
    sigemptyset(set); sigaddset(set,s);
    pthread_sigmask(SIG_UNBLOCK,set,NULL);}
}

#endif

static void sigHandler(int sig){
    set_sighandler(sig,sigHandler);
    {t_exceptionSignal e={sig};
    e.count=backtrace(e.trace,sizeof(e.trace)/sizeof(*e.trace));
    THROW(t_exceptionSignal,e);}
}
static void sigInitializer(void){
    set_sighandler(SIGFPE,sigHandler);
    set_sighandler(SIGSEGV,sigHandler);
}

int main(int argc,char **argv)
{
    execName=argv[0];
    TryCatchSignal=sigInitializer;

    test_1();
    test_2();
    test_3();
    test_4();
    test_5();
    test_6();
    test_7();
    test_8();

    getchar();

    return 0;
}
