#include "api.h"
#include "params.h"

int main(){

    /*Keygen*/
    GNTRU_MSK_Data * GNTRU_MSKD = new GNTRU_MSK_Data;
    GNTRU_MPK_Data * GNTRU_MPKD = new GNTRU_MPK_Data;
    char MPK[10000];
    memset(MPK, 0, 10000);
    Api_Keygen(GNTRU_MPKD, GNTRU_MSKD, MPK);

    GNTRU_MSK_Data * GNTRU_MSKD1 = new GNTRU_MSK_Data;
    GNTRU_MPK_Data * GNTRU_MPKD1 = new GNTRU_MPK_Data;
    char MPK1[10000];
    memset(MPK1, 0, 10000);
    Api_Keygen(GNTRU_MPKD1, GNTRU_MSKD1, MPK1);
    // printf("%s\n",MPK);

    /*Encrypt & Decrypt*/
    char mes[] = "Hello,Bob!";
    char c[100000];
    memset(c, 0, 100000);
    Api_Enc(MPK, c, mes, strlen(mes));
    // printf("%s\n",c);

    char m0[100000];
    int check = Api_Dec(GNTRU_MPKD, GNTRU_MSKD, m0, c, strlen(c));
    printf("Bob's message length = %d, message is * %s *\n\n",check, m0);

    char c1[100000];
    memset(c1, 0, 100000);
    Api_Enc(MPK1, c1, mes, strlen(mes));
    // printf("%s\n",c);

    check = Api_Dec(GNTRU_MPKD, GNTRU_MSKD, m0, c1, strlen(c1));
    printf("Eve's check = %d\n\n",check);


    /*Signature & Verify*/
    char Sign[100000];
    memset(Sign, 0, 100000);
    Api_Sign(GNTRU_MSKD, Sign, mes, strlen(mes));
    check = Api_Vrfy(MPK, Sign, mes, strlen(mes));
    printf("\n");
    

    Api_Sign(GNTRU_MSKD1, Sign, mes, strlen(mes));
    check = Api_Vrfy(MPK, Sign, mes, strlen(mes));
    printf("Eve's check = %d\n\n",check);


    /*Signcrypt & Unsigncrypt*/
    memset(c, 0, 100000);
    Api_Signcrypt(MPK, GNTRU_MSKD, c, mes, strlen(mes));
    memset(m0, 0, 100000);
    check = Api_Unsigncrypt(GNTRU_MPKD, GNTRU_MSKD, MPK, m0, c, strlen(c));
    printf("Bob's message length = %d, message is * %s *\n\n",check, m0);

    memset(m0, 0, 100000);
    check = Api_Unsigncrypt(GNTRU_MPKD1, GNTRU_MSKD1, MPK, m0, c, strlen(c));
    printf("Eve's check = %d\n\n",check);

    memset(m0, 0, 100000);
    check = Api_Unsigncrypt(GNTRU_MPKD, GNTRU_MSKD, MPK1, m0, c, strlen(c));
    printf("Eve's check = %d\n\n",check);
}