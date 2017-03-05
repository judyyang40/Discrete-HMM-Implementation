#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "hmm.h"


int main(int argc,char *argv[]){
	HMM models[5];
	double delta[MAX_SEQ][MAX_STATE], max_p[5];
	char data[MAX_LINE]={0};
	int i, j, k, n, t, count = 0,data_len, states, result_state;
	double max, max_prob[MAX_LINE];

	FILE *fp1 = open_or_die(argv[2], "r");
	FILE *fp2 = open_or_die(argv[3], "w");

	load_models(argv[1], models, 5);

	while(fscanf(fp1, "%s", data)!=EOF){
		data_len = strlen(data);

		for(n=0; n<5; n++){
			states = models[n].state_num;

			//initialization
			for(i=0; i<states; i++){
				k = data[0]-65;
				delta[0][i] = models[n].initial[i]*models[n].observation[k][i];
			}
			//induction
			for(t=1; t<data_len; t++){
				for(i=0; i<states; i++){
					max = 0;
					for(j=0; j<states; j++){
						if(max < delta[t-1][j]*models[n].transition[j][i])
							max = delta[t-1][j]*models[n].transition[j][i];
					}
					k = data[t]-65;
					delta[t][i] = max*models[n].observation[k][i];
				}
			}
			max = 0;
			for(i=0; i<states; i++){
				if(delta[data_len-1][i] > max)
					max = delta[data_len-1][i];
			}
			max_p[n] = max;
		}
		max = 0;
		for(n=0; n<5; n++){
			if(max_p[n] > max){
				max = max_p[n];
				result_state = n;

			}
		}
		fprintf(fp2, "%s %e\n", models[result_state].model_name, max);
	}
	fclose(fp1);
	fclose(fp2);

	return 0;
}