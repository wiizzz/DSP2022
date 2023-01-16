#include <iostream>
#include "hmm.h"
#include <string.h>


using namespace std;
//calc alpha
void forward(HMM *hmm, char* observ_seq, int seq_len, double alpha[][MAX_STATE]){
    //initial
    for(int i = 0; i < hmm->state_num; i++){
        alpha[0][i] = (hmm->initial[i])*(hmm->observation[observ_seq[0] - 'A'][i]);  
    }
    //induction
    for(int t = 0; t < seq_len-1; t++){
        for(int j = 0; j < hmm->state_num; j++){
        double sum = 0;
        for(int i = 0; i < hmm->state_num; i++){
            sum += alpha[t][i]* hmm->transition[i][j];
        }
        alpha[t+1][j] = sum * hmm->observation[observ_seq[t+1] - 'A'][j];
        }
    }
}
//calc beta
void backward(HMM *hmm, char* observ_seq, int seq_len, double beta[][MAX_STATE]) {
    //initialization
    for(int i = 0; i < hmm->state_num; i++){
        beta[seq_len-1][i] = 1;
    }
    // induction
    for(int t = seq_len-2; t >= 0; t--){
        for(int i = 0; i < hmm->state_num; i++){
            beta[t][i] = 0.0;
            for(int j = 0; j < hmm->state_num; j++){
                beta[t][i] += hmm->transition[i][j]*hmm->observation[observ_seq[t + 1] - 'A'][j]*beta[t+1][j];
                
            }
        }
    }
}
//calc gamma
void Gamma(HMM *hmm, char* observ_seq, int seq_len,double alpha[][MAX_STATE], double beta[][MAX_STATE], double gamma[][MAX_STATE]) {
    //initialization
    for(int t = 0; t < seq_len; t++){
        double D = 0;   //denominator
        for(int i = 0; i < hmm->state_num; i++){
            gamma[t][i] = alpha[t][i]*beta[t][i]; 
            D += gamma[t][i];
        }

        for(int i = 0; i < hmm->state_num; i++){
            gamma[t][i] /= D;
        }
    }
}
//calc epsilon
void Epsilon(HMM *hmm, char* observ_seq, int seq_len, double alpha[][MAX_STATE], double beta[][MAX_STATE], double epsilon[][MAX_STATE][MAX_STATE]) {
    //initialization
    for(int t = 0; t < seq_len-1; t++){
        double s = 0;   //denominator
        for(int i = 0; i < hmm->state_num; i++){
            for(int j = 0;j < hmm->state_num; j++){
                epsilon[t][i][j] = alpha[t][i]*hmm->transition[i][j]*hmm->observation[observ_seq[t + 1] - 'A'][j]*beta[t+1][j];
                s += epsilon[t][i][j];
            }
        }

        for(int i = 0; i < hmm->state_num; i++){
            for(int j = 0;j < hmm->state_num; j++){
                epsilon[t][i][j] /= s; 
            }
        }
    }
}
//update initial prob. per sequence
void update_initial(HMM *hmm,double gamma[][MAX_STATE], double new_pi[]){
    for(int i = 0; i < hmm->state_num; i++)
            new_pi[i] += gamma[0][i];  //accumulate of gamma1(i)
}

void sum_gamma(HMM* hmm,int seq_len, double gamma[][MAX_STATE], double gammaN1[], double gammaN2[]){
    for(int i = 0; i < hmm->state_num; i++){
        for(int t = 0; t < seq_len;t++){
            gammaN2[i] += gamma[t][i];
            if(t >= seq_len-1)
                break;
            else
                gammaN1[i] += gamma[t][i];
        }
    }
}

void sum_epsilon(HMM* hmm,int seq_len, double epsilon[][MAX_STATE][MAX_STATE],double epsilonN[][MAX_STATE]){
    //update transition prob.
 for(int i = 0; i < hmm->state_num; i++){
    for(int j = 0; j < hmm->state_num; j++){
        for(int t = 0; t < seq_len-1; t++){
            epsilonN[i][j] += epsilon[t][i][j];
        }
    }
   }
}

void sum_observ(HMM* hmm,char* observ_seq,int seq_len, double gamma[][MAX_STATE], double observ[][MAX_STATE]){
    for(int i = 0; i < hmm->state_num; i++){
        for(int t = 0; t < seq_len; t++){
            observ[observ_seq[t]-'A'][i] += gamma[t][i];
        }
    }
}

void trainHMM(HMM *hmm,  char *data_filename, int iteration){
   char seq[MAX_SEQ] ;  //a row in data file

   for (int n = 0; n < iteration; n++){
      double new_pi[MAX_STATE] = {0};
      double gammaN1[MAX_STATE] = {0};  //summation of gamma from 1 to T-1
      double gammaN2[MAX_STATE] = {0};  //summation of gamma from 1 to T
      double gamma_observ[MAX_OBSERV][MAX_STATE] = {0}; //summation of observation O = k in state i
      double epsilonN[MAX_STATE][MAX_STATE] ={0};

      double T[MAX_OBSERV][MAX_STATE] = {0};
      cout  << n+1 << "-th training" << endl;

      int data_num = 0; //total row number of data
      FILE *seq_file = open_or_die(data_filename, "r");
      while (fscanf(seq_file, "%s", seq) > 0){
        int seq_len = strlen(seq);  //length of a row in data file
        double alpha[MAX_SEQ][MAX_STATE] = {0};
        double beta[MAX_SEQ][MAX_STATE] = {0};
        double gamma[MAX_SEQ][MAX_STATE] = {0};
        double epsilon[MAX_SEQ][MAX_STATE][MAX_STATE] = {0};
        forward(hmm,seq, seq_len,alpha);
        backward(hmm,seq,seq_len,beta);
        Gamma(hmm,seq, seq_len,alpha,beta,gamma);
        Epsilon(hmm,seq,seq_len,alpha,beta,epsilon);
        update_initial(hmm,gamma,new_pi);
        sum_gamma(hmm,seq_len,gamma,gammaN1,gammaN2);
        sum_epsilon(hmm,seq_len,epsilon,epsilonN);
        sum_observ(hmm,seq,seq_len,gamma,gamma_observ);
        data_num++;
      }
        // Update Initial Prob. 
      for (int i = 0; i < hmm->state_num; i++)
        hmm->initial[i] = new_pi[i] / data_num;
        // Update Transition Prob.
      for (int i = 0; i <hmm->state_num; i++){
        for (int j = 0; j < hmm->state_num; j++)
            hmm->transition[i][j] = epsilonN[i][j] / gammaN1[i];
      }
        //Update Observation Prob.
      for (int k = 0; k < MAX_OBSERV; k++){
        for (int i = 0; i < hmm->state_num; i++)
            hmm->observation[k][i] = gamma_observ[k][i] / gammaN2[i];
      }  
      fclose(seq_file);      
   }
}

int main(int argc, char* argv[]){
    int iteration = atoi(argv[1]);
    char* dataFileName= argv[3];
    HMM hmm_model;
    //load initial model
    loadHMM( &hmm_model, argv[2]);  //loadHMM( &hmm_model, "../model_init.txt" );
    trainHMM(&hmm_model, dataFileName, iteration);
    //write output file
    FILE *out = open_or_die(argv[4], "w");
    dumpHMM(out, &hmm_model);
    fclose(out);
	return 0;
}
