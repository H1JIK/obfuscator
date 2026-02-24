/*sdffd
sfdsfdfd
dfs*/

#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#define XOR_CONST 34	//just comment
#define MAX_BUF 256
int func_var_1() {
	int a = 10;
	int b = 12;
	int sum = (a + b)*b;
	return sum;
}
int func_var_2() {
	int c = 213;
	int j = 64;
	int result = c * j + j;
	return result;
}
/*comm
so
big*/
void xor_pass(char user_pswd[]) {
	for (int i = 0; i < strlen(user_pswd); i++) {
		user_pswd[i] ^= XOR_CONST;
	}
}

int check_pass(char* input_user, char* password) {
	if (strcmp(password, input_user) == 0) {
		return 1;
	}
	else {
		return 0;
	}
}

void main() {
	int sm_vr_1 = 0;
	for(int i = 0; i < 32; i++){
		sm_vr_1++;
	}
	int sm_vr_2 = 0;
	while(sm_vr_2 != 43){
		sm_vr_2++;
	}
	func_var_1();
	func_var_2();

	FILE* password_f = fopen("password.txt", "r");
	char password[MAX_BUF]; char input_user[MAX_BUF];
	fscanf(password_f, "%s", password);
	printf("Input the password: ");
	scanf("%255s", input_user);
	xor_pass(input_user);
	int success = check_pass(input_user, password);
	if (success) {
		printf("Access is allowed!\n");
	}
	else {
		printf("Access is denied!\n");
	}
	printf("//");


	fclose(password_f);
	getchar();
	getchar();
}//testikk