#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "hmm.h"

int main(int argc,char *argv[]){

	int iter, i, j, t, k;
	int iteration, seq_len;
	HMM model;
	char sequence[100]={0};
	
	double sum;

	iteration = atoi(argv[1]);
	loadHMM(&model, argv[2]);
	FILE *fp1 = open_or_die( argv[3], "r");
	FILE *fp2 = open_or_die( argv[4], "w");

	int states = model.state_num;

	for(iter=0; iter<iteration; iter++){

		double alpha[MAX_SEQ][MAX_STATE]={0.0}, beta[MAX_SEQ][MAX_STATE]={0.0}, gamma[MAX_SEQ][MAX_STATE]={0.0}, epsilon[MAX_SEQ][MAX_STATE][MAX_STATE]={0.0};
		double sum_gamma[MAX_STATE]={0.0}, sum_epsilon[MAX_STATE][MAX_STATE]={0.0}, sum_gamma1[MAX_STATE]={0.0}, sum_gammak[MAX_SEQ][MAX_STATE]={0.0}, sum_gammaT[MAX_SEQ]={0.0};

		while(fscanf(fp1, "%s", sequence)!=EOF){
			//printf("%s\n", sequence);
			//Forward Algorithm
			//memset(alpha, 0, sizeof(alpha));
			//initialization
			for(i=0; i<states; i++){
				k = sequence[0]-65;
				alpha[0][i] = model.initial[i]*model.observation[k][i];
			}
			//induction
			seq_len = strlen(sequence);
			for(t=0; t<seq_len-1; t++){
				for(j=0; j<states; j++){
					sum = 0;
					for(i=0; i<states; i++)
						sum += alpha[t][i]*model.transition[i][j];
					k = sequence[t+1]-65;
					alpha[t+1][j] = model.observation[k][j]*sum;
				}
			}
			//terminate


			//Backward Algorithm
			//memset(beta, 0, sizeof(beta));
			//initialization
			for(i=0; i<states; i++)
				beta[seq_len-1][i] = 1;
			//induction
			for(t=seq_len-2; t>=0; t--){
				k = sequence[t+1]-65;
				for(i=0; i<states; i++){
					sum = 0;
					for(j=0; j<states; j++)
						sum += model.transition[i][j]*model.observation[k][j]*beta[t+1][j];
					beta[t][i] = sum;
				}
			}

			/*for(t=0; t<seq_len; t++){
				for(i=0; i<states; i++){
					printf("%.5lf ",beta[t][i]);
				}
				printf("\n");
			}*/

			//gamma
			//memset(gamma, 0, sizeof(gamma));
			for(t=0; t<seq_len; t++){
				sum = 0;
				k = sequence[t]-65;
				for(i=0; i<states; i++)
					sum += alpha[t][i]*beta[t][i];
				for(i=0; i<states; i++){
					gamma[t][i] = (alpha[t][i]*beta[t][i])/sum;
					//accumulate gamma for every k
					//sum_gammak[k][i] += gamma[t][i];
				}
			}

			/*for(t=0; t<seq_len; t++){
				for(i=0; i<states; i++){
					printf("%.5lf ",gamma[t][i]);
				}
				printf("\n");
			}*/

			//epsilon
			//memset(epsilon, 0, sizeof(epsilon));
			for(t=0; t<seq_len-1; t++){
				k = sequence[t+1]-65;
				sum = 0;
				for(i=0; i<states; i++){
					for(j=0; j<states; j++){
						sum += alpha[t][i]*model.transition[i][j]*model.observation[k][j]*beta[t+1][j];
					}
				}
				for(i=0; i<states; i++){
					for(j=0; j<states; j++){
						epsilon[t][i][j] = (alpha[t][i]*model.transition[i][j]*model.observation[k][j]*beta[t+1][j])/sum;
					}
				}
			}
			//accumulate gamma t =1
			for(i=0; i<states; i++){
				sum_gamma1[i] += gamma[0][i];
			}
			//accumulate gamma
			for(i=0; i<states; i++){
				for(t=0; t<seq_len-1; t++){
					sum_gamma[i] += gamma[t][i];
				}
			}
			//accumulate gamma to T
			for(i=0; i<states; i++){
				for(t=0; t<seq_len; t++){
					sum_gammaT[i] += gamma[t][i];
				}
			}
			//accumulate gamma for observation k
			for(t=0; t<seq_len; t++){
				k = sequence[t]-65;
				for(i=0; i<states; i++){
					sum_gammak[k][i] += gamma[t][i];
				}
			}
			//accumulate epsilon
			for(i=0; i<states; i++){
				for(j=0; j<states; j++){
					for(t=0; t<seq_len-1; t++){
						sum_epsilon[i][j] += epsilon[t][i][j];
					}
				}
			}
		}
		//update
		//initial
		for(i=0; i<states; i++)
			model.initial[i] = sum_gamma1[i]/10000;
		//transition
		for(i=0; i<states; i++){
			for(j=0; j<states; j++){
				model.transition[i][j] = sum_epsilon[i][j]/sum_gamma[i];
			}
		}
		//observation
		for(k=0; k<model.observ_num; k++){
			for(i=0; i<states; i++){
				model.observation[k][i] = sum_gammak[k][i]/sum_gammaT[i];
			}
		}
		rewind(fp1);
	}
	dumpHMM(fp2, &model);
	fclose(fp1);
	fclose(fp2);
	return 0;
}