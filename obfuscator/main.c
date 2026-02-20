#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define BUF 128
#define MAX_LEN_NAMES 64
#define MAX_LEN_ARR_NAMES 64

typedef struct {
	char* text;
	unsigned int len;
}text_s;

typedef struct {
	char before[MAX_LEN_NAMES];
	char after[MAX_LEN_NAMES];
}arrName;

text_s prog_text;
unsigned int arr_names_cnt;
arrName prog_var[MAX_LEN_ARR_NAMES];

char* keywords[] = {
"int","char","float","double","void","short","long","signed","unsigned","struct","union","enum","const","volatile","static","extern","register","auto","if","else","switch","case","default","for","while","do","break","continue","goto","return","sizeof","typedef","printf","scanf","fopen","fclose","fscanf","getchar","strlen","strcmp","malloc","free","rand","srand","atoi","main", "FILE", NULL };

char* garbage_var[] = { "int var_r;\nvar_r = 423;\n", "char var_r2;\nvar_r2 = 'c';\n", NULL };
char* garbage_cycles[] = { "int sm_vr_1 = 0;\nfor(int i = 0; i < 32; i++){\nsm_vr_1++;\n}\n", "int sm_vr_2 = 0;\nwhile(sm_vr_2 != 43){\nsm_vr_2++;\n}\n", NULL };
char* garbage_funcs[] = { "int func_var_1(){\nint a = 10;\nint b = 12;\n int sum = (a + b)*b;\nreturn sum;\n}\n", "int func_var_2(){\nint c = 213;\nint j = 64;\n int result = c * j + j;\nreturn result;\n}\n", NULL };
char* garbage_funcs_call[] = { "func_var_1();\n", "func_var_2();\n", NULL };

void read_file(FILE* f) {
	prog_text.text = (char*)malloc(BUF);
	char cur_elem = fgetc(f);
	int i = 0;
	while (cur_elem != EOF) {
		if (i % BUF == 0 && i != 0) {
			prog_text.text = (char*)realloc(prog_text.text, (i + BUF));
		}
		prog_text.text[i] = cur_elem;
		i++;
		cur_elem = fgetc(f);
	}
	prog_text.text = (char*)realloc(prog_text.text, (i + 1));
	prog_text.text[i] = '\0';
	prog_text.len = i;
}

int shift(unsigned int start, unsigned int stop) {
	int i = 0;
	int razn = (stop - start);
	for (i; i + stop < prog_text.len; i++) {
		prog_text.text[start + i] = prog_text.text[stop + i + 1];
	}

	return razn;
}

void del_comms() {
	int i = 0;
	int start = -1, finish = 0;
	for (i; i < prog_text.len; i++) {
		if (prog_text.text[i] == '\"' && start == -1) {
			i++;
			while (prog_text.text[i] != '\"') i++;
		}
		if (prog_text.text[i] == '/' && prog_text.text[i + 1] == '/') {
			start = i;
			while (prog_text.text[i + 1] != '\n' && prog_text.text[i + 1] != '\0') i++;
		}
		else if (prog_text.text[i] == '/' && prog_text.text[i + 1] == '*') {
			start = i;
			while (!(prog_text.text[i - 1] == '*' && prog_text.text[i] == '/')) i++;
			i++;
			//добавить  i++, чтобы сдвигов не было (типа \t)
		}
		if (start != -1) {
			finish = i;
			int razn = shift(start, finish) + 1;
			i -= razn - 1;
			prog_text.len -= razn;
			start = finish = -1;
		}
	}
	prog_text.text = (char*)realloc(prog_text.text, (i + 1));
}

void del_space() {
	int i = 0;
	int start = -1, finish = 0;
	for (i; i < prog_text.len; i++) {
		if (prog_text.text[i] == '#' && start == -1) {
			while (prog_text.text[i] != '\n' && prog_text.text[i] != '\0') i++;
			continue;
		}
		if (prog_text.text[i] == '\"' && start == -1) {
			i++;
			while (prog_text.text[i] != '\"') i++;
		}
		if (prog_text.text[i] == '/' && prog_text.text[i+1] == '/' && start == -1) {
			while (prog_text.text[i] != '\n') i++;
			prog_text.text[i] = ' ';
			continue;
		}
		if (prog_text.text[i] == '/' && prog_text.text[i + 1] == '*' && start == -1) {
			while (!(prog_text.text[i] == '*' && prog_text.text[i + 1] == '/')) i++;
			i++;
			continue;
		}
		else if ((prog_text.text[i] == ' ' || prog_text.text[i] == '\t' || prog_text.text[i] == '\n')) {
			if (i != 0 && ((isalpha(prog_text.text[i - 1]) && isalpha(prog_text.text[i + 1])) || (isdigit(prog_text.text[i - 1]) && isalpha(prog_text.text[i + 1])) || (isdigit(prog_text.text[i + 1]) && isalpha(prog_text.text[i - 1])))) {
				prog_text.text[i] = ' ';
			}
			else if (start == -1) start = i;
		}
		else {
			if (start != -1) {
				finish = i - 1;
				int razn = shift(start, finish) + 1;
				i -= razn + 1;
				prog_text.len -= razn;
				start = finish = -1;
			}
		}
	}
	prog_text.text = (char*)realloc(prog_text.text, (i + 1));
	//printf("%c\n", prog_text.text[i]);
}

int is_keyword(char str[]) {
	int i = 0;
	while (keywords[i] != NULL) {
		if (strcmp(str, keywords[i]) == 0) return 1;
		i++;
	}
	return 0;
}

void find_and_replace_var() {
	int i = 0;
	int macros_flag = 0;
	int start = -1;
	int razn;
	for (i; i < prog_text.len; i++) {
		int flag_added = 0;
		if (isalpha(prog_text.text[i]) || prog_text.text[i] == '_') {
			char cur_word[MAX_LEN_NAMES];
			int pos = 0;
			start = i;
			while (i < prog_text.len && (isalnum(prog_text.text[i]) || prog_text.text[i] == '_')) {
				cur_word[pos++] = prog_text.text[i++];
			}
				cur_word[pos] = '\0';
				if (macros_flag) {
				macros_flag = 0;
				if ((i + 1) < prog_text.len && !(isdigit(prog_text.text[i + 1]))) {
					continue;
				}
			}
			if (is_keyword(cur_word)) continue;
			int k = 0;
			for (k; k < arr_names_cnt; k++) {
				if (strcmp(cur_word, prog_var[k].before) == 0) {
					flag_added = 1;
					break;
				}
			}
			if (!flag_added) {	//add
				strcpy(prog_var[arr_names_cnt].before, cur_word);
				sprintf(prog_var[arr_names_cnt].after, "vr_fc%d", (arr_names_cnt + 1));
				arr_names_cnt++;
			}
			if (strlen(prog_var[k].before) > strlen(prog_var[k].after)){
				int i_aft = 0;
				while(prog_var[k].after[i_aft] != '\0') {
					prog_text.text[start + i_aft] = prog_var[k].after[i_aft];
					i_aft++;
				}
				int new_i = start + strlen(prog_var[k].after);
				while (prog_text.text[i] != '\0') {
					prog_text.text[start + i_aft] = prog_text.text[i];
					i++; i_aft++;
				}
				prog_text.text[start + i_aft] = '\0';
				razn = strlen(prog_var[k].before) - strlen(prog_var[k].after);
				prog_text.len -= razn;
				prog_text.text = (char*)realloc(prog_text.text, (prog_text.len + 1));
				i = new_i;
			}
			else if (strlen(prog_var[k].before) < strlen(prog_var[k].after)) {
				razn = abs(strlen(prog_var[k].before) - strlen(prog_var[k].after));
				prog_text.len += razn;
				int i_aft = 0;
				int new_i = start + strlen(prog_var[k].after);

				prog_text.text = (char*)realloc(prog_text.text, (prog_text.len + 1));
				for (int i_2 = prog_text.len; i_2 >= (i + razn); i_2--) {
					prog_text.text[i_2] = prog_text.text[i_2 - razn];
				}
				while (prog_var[k].after[i_aft] != '\0') {
					prog_text.text[start + i_aft] = prog_var[k].after[i_aft];
					i_aft++;
				}
				i += razn;
				//printf("%s", prog_text.text);
			}



			start = -1;
			printf("before: %s		 after: %s\n", prog_var[arr_names_cnt - 1].before, prog_var[arr_names_cnt - 1].after);
		}
		else {
			if (prog_text.text[i] == '\"') {
				i++;
				while (i < prog_text.len && prog_text.text[i] != '\"') {
					i++;
				} continue;
			}
			if (prog_text.text[i] == '#') {
				macros_flag = 1;
			}
			if (prog_text.text[i] == '/' && prog_text.text[i + 1] == '/') {
				while (i < prog_text.len && prog_text.text[i] != '\n') {
					i++;
				} continue;
			}
			if (prog_text.text[i] == '/' && prog_text.text[i + 1] == '*') {
				while (i < prog_text.len && !(prog_text.text[i] == '*' && prog_text.text[i + 1] == '/')) {
					i++;
				} continue;
			}
			if (prog_text.text[i] == '(' || prog_text.text[i] == '*' || prog_text.text[i] == '+' || prog_text.text[i] == '-' || prog_text.text[i] == '/' || prog_text.text[i] == '^') {
				continue;
			}
			while (i < prog_text.len && prog_text.text[i] != ' ' && prog_text.text[i] != '\n' && prog_text.text[i] != '\t') i++;
		}

	}
}

void insert_garbage() {
	int i = 0;
	int flag_main = 0;
	while (prog_text.text[i] != '\0') {
		int start = -1;
		int macros_flag = 0;
		int sum_len_shift = 0;
		int i_mas = 0;
		if (prog_text.text[i] == '#') {
			while (prog_text.text[i] != '\n') i++;
			i++;
			if (prog_text.text[i] == '#') continue;
			
			start = i;
			while (garbage_funcs[i_mas] != NULL) {
				sum_len_shift += strlen(garbage_funcs[i_mas]);
				i_mas++;
			}
			i_mas = 0;
			int new_len = prog_text.len + sum_len_shift;
			prog_text.text = (char*)realloc(prog_text.text, (new_len + 1));
			prog_text.len = new_len;
			for (int i_2 = prog_text.len; i_2 >= start; i_2--) {
				prog_text.text[i_2] = prog_text.text[i_2 - sum_len_shift];
			}
			while (garbage_funcs[i_mas] != NULL) {
				for (int i_func = 0; i_func < strlen(garbage_funcs[i_mas]); i_func++) {
					prog_text.text[i++] = garbage_funcs[i_mas][i_func];
				}
				i_mas++;
			}
			continue;
		}
		if (prog_text.text[i] == '\"') {
			while (prog_text.text[i] != '\"') i++;
			i++;
			continue;
		}


		if ((i > 0) && (i + 5 < prog_text.len) &&  prog_text.text[i - 1] == ' ' && prog_text.text[i] == 'm' && prog_text.text[i + 1] == 'a' && prog_text.text[i + 2] == 'i' && prog_text.text[i + 3] == 'n'  && (prog_text.text[i + 4] == '(' || (prog_text.text[i + 4] == ' ' && prog_text.text[i + 5] == '('))) {
			while (prog_text.text[i] != '{') i++;
			i++;
			flag_main = 1;
			continue;
		}
		if (flag_main){
			start = i;
			i_mas = 0;
			while (garbage_funcs[i_mas] != NULL) {
				sum_len_shift += strlen(garbage_funcs[i_mas]);
				i_mas++;
			}
			i_mas = 0;
			while (garbage_cycles[i_mas] != NULL) {
				sum_len_shift += strlen(garbage_cycles[i_mas]);
				i_mas++;
			}
			i_mas = 0;
			while (garbage_funcs_call[i_mas] != NULL) {
				sum_len_shift += strlen(garbage_funcs_call[i_mas]);
				i_mas++;
			}
			i_mas = 0;
			int new_len = prog_text.len + sum_len_shift;
			prog_text.text = (char*)realloc(prog_text.text, (new_len + 1));
			prog_text.len = new_len;
			for (int i_2 = prog_text.len; i_2 >= start; i_2--) {
				prog_text.text[i_2] = prog_text.text[i_2 - sum_len_shift];
			}
			while (garbage_funcs[i_mas] != NULL) {
				for (int i_func = 0; i_func < strlen(garbage_funcs[i_mas]); i_func++) {
					prog_text.text[i++] = garbage_funcs[i_mas][i_func];
				}
				i_mas++;
			}
			i_mas = 0;
			while (garbage_cycles[i_mas] != NULL) {
				for (int i_func = 0; i_func < strlen(garbage_cycles[i_mas]); i_func++) {
					prog_text.text[i++] = garbage_cycles[i_mas][i_func];
				}
				i_mas++;
			}
			i_mas = 0;
			while (garbage_funcs_call[i_mas] != NULL) {
				for (int i_func = 0; i_func < strlen(garbage_funcs_call[i_mas]); i_func++) {
					prog_text.text[i++] = garbage_funcs_call[i_mas][i_func];
				}
				i_mas++;
			}
			flag_main = 0;
			//printf("%c", prog_text.text[prog_text.len]);
		}
		i++;
	}
}

void main() {
	FILE* input_f = fopen("prog.c", "r");
	FILE* output_f = fopen("out_prog.c", "w");
	read_file(input_f);

	insert_garbage();
	find_and_replace_var();

	del_comms();
	del_space();
	printf("%s", prog_text.text);
	fprintf(output_f, "%s", prog_text.text);
	//printf("%d %d", isdigit('{'), isalpha('{'));

	free(prog_text.text);
	fclose(input_f);

}