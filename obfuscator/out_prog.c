#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#define vr_fc1 34	
#define vr_fc2 256
void vr_fc3(char vr_fc4[]){for(int vr_fc5=0;vr_fc5<strlen(vr_fc4);vr_fc5++){vr_fc4[vr_fc5]^=vr_fc1;}}int vr_fc6(char*vr_fc7,char*vr_fc8){if(strcmp(vr_fc8,vr_fc7)==0){return 1;}else{return 0;}}void main(){FILE*vr_fc9=fopen("password.txt","r");char vr_fc8[vr_fc2];char vr_fc7[vr_fc2];fscanf(vr_fc9,"%s",vr_fc8);printf("Input the password: ");scanf("%255s",vr_fc7);vr_fc3(vr_fc7);int success=vr_fc6(vr_fc7,vr_fc8);if(success){printf("Access is allowed!\n");}else{printf("Access is denied!\n");}printf("//");fclose(vr_fc9);getchar();getchar();}