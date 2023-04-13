//  fibonacci
//  Created by Pandora on 2023/4/11.

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/time.h>
#include <pthread.h>
#include "/usr/local/gmp.h"
#include "gmp.h"

#define ROW 2
#define COL 2

mpz_t a,b,ab,aMul_aAndb,aSquare,bSquare;
mpz_t aSquareAdd_bSquare,abAdd_aMul_aAndb;

mpz_t A_powerAdd;
mpz_t A[ROW][COL],A_power[ROW][COL];

mpz_t result;

mpz_t F_N_Subtract1_divide2_Add1,F_N_Subtract1_divide2;
mpz_t F_N_divide2,F_N_divide2_Subtract1;

mpz_t A11B12,A12B22,A21B12,A22B22;
mpz_t Add_A11B11_A12B21,Add_A21B11_A22B22,Add_A11B12_A12B22,Add_A21B12_A22B22;

pthread_mutex_t myIndexChange;

pthread_t get_abThread;
pthread_t get_aMul_aAndbThread;
pthread_t get_aSquareThread;
pthread_t get_bSquareThread;

pthread_t threadMpzMulPool[4];

unsigned threadStatus[4];
unsigned n=10000000;
unsigned myIndexGlobal=0;;

bool threadCotinueMain=false;
bool threadExit=false;

// threadStatus[0]
void* get_ab(){
    while(!threadExit){
        if(threadStatus[0]){
            continue;
        }
        mpz_mul(ab,a,b);
        threadStatus[0]=1;
    }
    return NULL;
}

// threadStatus[1]
void* get_aMul_aAndb(){
    while(!threadExit){
        if(threadStatus[1]){
            continue;
        }
        mpz_add(aMul_aAndb,a,b);
        mpz_mul(aMul_aAndb,a,aMul_aAndb);
        threadStatus[1]=1;
    }
    return NULL;
}

// threadStatus[2]
void* get_aSquare(){
    while(!threadExit){
        if(threadStatus[2]){
            continue;
        }
        mpz_pow_ui(aSquare,a,2);
        threadStatus[2]=1;
    }
    return NULL;
}

// threadStatus[3]
void* get_bSquare(){
    while(!threadExit){
        if(threadStatus[3]){
            continue;
        }
        mpz_pow_ui(bSquare,b,2);
        threadStatus[3]=1;
    }
    return NULL;
}

void *threadMpzMul(){
    pthread_mutex_lock(&myIndexChange);
    myIndexGlobal++;
    unsigned myIndex=myIndexGlobal;
    pthread_mutex_unlock(&myIndexChange);
    switch(myIndex){
        case 1:
            mpz_mul(A11B12,A[0][0],A_power[0][1]);
            break;
        case 2:
            mpz_mul(A12B22,A[0][1],A_power[1][1]);
            break;
        case 3:
            mpz_mul(A21B12,A[1][0],A_power[0][1]);
            break;
        case 4:
            mpz_mul(A22B22,A[1][1],A_power[1][1]);
            break;
        default:
            printf("WHY I GOT NO NUMBER TO PROCESS ? ? ?\n");
    }
    return NULL;
}

void fibonacci(){
    struct timeval stopM,startM;
//    gettimeofday(&startM,NULL);
    if(!n){
        printf("n MUST BE GREATER THAN 0!\n");
        return;
    }
    if(n<=2){
        mpz_set_ui(result,1);
        return;
    }
    
    for(unsigned char threadStatusIndex=0;threadStatusIndex<4;threadStatusIndex++){
        threadStatus[threadStatusIndex]=1;
    }
    
    unsigned nCopy=n;
    if(nCopy&1){
        nCopy=nCopy>>1;
        nCopy--;
    }
    else{
        nCopy=nCopy>>1;
        nCopy-=2;
    }
    while(nCopy){
//        gettimeofday(&startM,NULL);
        if(nCopy&1){
            mpz_set(A_power[0][1],abAdd_aMul_aAndb);
            mpz_set(A_power[1][1],aSquareAdd_bSquare);
            mpz_add(A_powerAdd,abAdd_aMul_aAndb,aSquareAdd_bSquare);
            mpz_set(A_power[0][0],A_powerAdd);
            mpz_set(A_power[1][0],abAdd_aMul_aAndb);
            for(unsigned char threadIndex=0;threadIndex<4;threadIndex++){
                pthread_t currentThread;
                pthread_create(&currentThread,NULL,threadMpzMul,NULL);
                threadMpzMulPool[threadIndex]=currentThread;
            }
            for(unsigned char threadIndex=0;threadIndex<4;threadIndex++){
                pthread_join(threadMpzMulPool[threadIndex],NULL);
            }
            myIndexGlobal=0;
            mpz_add(Add_A11B12_A12B22,A11B12,A12B22);
            mpz_add(Add_A21B12_A22B22,A21B12,A22B22);
            mpz_add(Add_A11B11_A12B21,Add_A11B12_A12B22,Add_A21B12_A22B22);
            mpz_set(A[0][0],Add_A11B11_A12B21);
            mpz_set(A[0][1],Add_A11B12_A12B22);
            mpz_set(A[1][0],Add_A11B12_A12B22);
            mpz_set(A[1][1],Add_A21B12_A22B22);
        }
//        gettimeofday(&stopM,NULL);
//        printf("MATRIX MUL time: %ld milliseconds\n",(stopM.tv_sec-startM.tv_sec)*1000+(stopM.tv_usec-startM.tv_usec)/1000);
        nCopy=nCopy>>1;
        if(!nCopy){
            break;
        }
        for(unsigned char threadStatusIndex=0;threadStatusIndex<4;threadStatusIndex++){
            threadStatus[threadStatusIndex]=0;
        }
        unsigned threadIndex=0;
        while(1){
            if(!(threadStatus[threadIndex])){
                threadIndex=0;
                continue;
            }
            threadIndex++;
            if(threadIndex==4){
                threadIndex=0;
                break;
            }
        }
        
        mpz_add(abAdd_aMul_aAndb,ab,aMul_aAndb);
        mpz_add(aSquareAdd_bSquare,aSquare,bSquare);
        mpz_set(a,abAdd_aMul_aAndb);
        mpz_set(b,aSquareAdd_bSquare);
        
    }
    threadExit=true;
    if(n&1){
        mpz_add(F_N_Subtract1_divide2_Add1,A[0][0],A[0][1]);
        mpz_add(F_N_Subtract1_divide2,A[1][0],A[1][1]);
        mpz_pow_ui(F_N_Subtract1_divide2_Add1,F_N_Subtract1_divide2_Add1,2);
        mpz_pow_ui(F_N_Subtract1_divide2,F_N_Subtract1_divide2,2);
        mpz_add(result,F_N_Subtract1_divide2_Add1,F_N_Subtract1_divide2);
    }
    else{
        mpz_add(F_N_divide2,A[0][0],A[0][1]);
        mpz_add(F_N_divide2_Subtract1,A[1][0],A[1][1]);
        mpz_mul_2exp(F_N_divide2_Subtract1,F_N_divide2_Subtract1,1);
        mpz_add(F_N_divide2_Subtract1,F_N_divide2,F_N_divide2_Subtract1);
        mpz_mul(result,F_N_divide2,F_N_divide2_Subtract1);
    }
//    gettimeofday(&stopM,NULL);
//    printf("IN time: %ld milliseconds\n",(stopM.tv_sec-startM.tv_sec)*1000+(stopM.tv_usec-startM.tv_usec)/1000);
}

int main(int argc, const char * argv[]) {
    if(argc>=2){
        n=strtoul(argv[1],NULL,10);
    }
    for(unsigned indexR=0;indexR<ROW;indexR++){
        for(unsigned indexC=0;indexC<COL;indexC++){
            mpz_init(A[indexR][indexC]);
            mpz_init(A_power[indexR][indexC]);
        }
    }
//  STE A AS E AT THE BEGINNING
    mpz_set_ui(A[0][0],1);
    mpz_set_ui(A[0][1],0);
    mpz_set_ui(A[1][0],0);
    mpz_set_ui(A[1][1],1);
    
    mpz_set_ui(A_power[0][0],1);
    mpz_set_ui(A_power[0][1],1);
    mpz_set_ui(A_power[1][0],1);
    mpz_set_ui(A_power[1][1],0);
    
    mpz_init(result);
    
    mpz_inits(a,b,ab,aMul_aAndb,aSquare,bSquare,NULL);
    mpz_set_ui(a,1);
    mpz_set_ui(b,0);
    mpz_set_ui(ab,0);
    mpz_set_ui(aSquare,1);
    mpz_set_ui(bSquare,0);
    
    mpz_inits(aSquareAdd_bSquare,abAdd_aMul_aAndb,NULL);
    mpz_set_ui(abAdd_aMul_aAndb,1);
    mpz_set_ui(aSquareAdd_bSquare,0);
    mpz_init(A_powerAdd);
    
    mpz_inits(F_N_Subtract1_divide2_Add1,F_N_Subtract1_divide2,NULL);
    mpz_inits(F_N_divide2,F_N_divide2_Subtract1,NULL);
    
    mpz_inits(A11B12,A12B22,A21B12,A22B22,NULL);
    mpz_inits(Add_A11B11_A12B21,Add_A21B11_A22B22,Add_A11B12_A12B22,Add_A21B12_A22B22,NULL);
    
    pthread_mutex_init(&myIndexChange,NULL);
    
    pthread_create(&get_abThread,NULL,get_ab,NULL);
    pthread_create(&get_aMul_aAndbThread,NULL,get_aMul_aAndb,NULL);
    pthread_create(&get_aSquareThread,NULL,get_aSquare,NULL);
    pthread_create(&get_bSquareThread,NULL,get_bSquare,NULL);
    
    struct timeval stopM,startM;
    gettimeofday(&startM,NULL);
    fibonacci();
    gettimeofday(&stopM,NULL);
    printf("Elapsed time: %ld milliseconds\n",(stopM.tv_sec-startM.tv_sec)*1000+(stopM.tv_usec-startM.tv_usec)/1000);
    pthread_join(get_abThread,NULL);
    pthread_join(get_aMul_aAndbThread,NULL);
    pthread_join(get_aSquareThread,NULL);
    pthread_join(get_bSquareThread,NULL);
//    gmp_printf("%Zd\n",result);
    
    return 0;
}
