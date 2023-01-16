#include "hmm.h"
#include <string.h>

#ifndef MODEL_NUM
#	define MODEL_NUM    5
#endif

typedef struct{
    int arg;   //argment of max prob.
    double prob;  //max prob.
} Result;

void viterbi(HMM *hmms, char* observ_seq, Result *result){
   int seq_len = strlen(observ_seq);
   //iterate through models
   for(int n = 0; n < MODEL_NUM; n++){
      HMM model = hmms[n];
      int state_num = model.state_num;
      double delta[MAX_SEQ][MAX_STATE] = {0};
      
      //initialization
      for(int i = 0; i < state_num; i++ ){
         delta[0][i] = model.initial[i]*model.observation[observ_seq[0]-'A'][i];
      }

      //Recursion
      for(int t = 1; t < seq_len; t++){
         for(int j = 0; j < state_num; j++){
            double temp = 0;
            for(int i = 0; i < state_num; i++){
               temp = delta[t-1][i]*model.transition[i][j]*model.observation[observ_seq[t]-'A'][j];
               if(temp > delta[t][j]){
                  delta[t][j] = temp;
               }
            }
         }
      }

      //Termination
      double Prob = 0;
      for(int i = 0; i < state_num; i++){
         if(delta[seq_len-1][i] > Prob){
            Prob = delta[seq_len-1][i];
         }
      }

      //compare to different model
      if(Prob > result->prob){
         result->arg = n; //update arg
         result->prob = Prob;
      }
   }
}

void testHMM(HMM *hmms,  char *data_filename, char* result_filename){
   char seq[MAX_SEQ] = {0};
   //read data file
   FILE *test_data = open_or_die(data_filename,"r");
   FILE *output = open_or_die(result_filename,"w");
   while (fscanf(test_data, "%s", seq) > 0){
      Result result = { .arg = 0, .prob = 0 };
      viterbi(hmms,seq,&result);
      fprintf(output, "%s %e\n", hmms[result.arg].model_name,result.prob);
   }
   fclose(test_data);
   fclose(output);
}

int main(int argc, char *argv[]){
   char* test_data_name= argv[2];
   char* result_filename = argv[3];
   //loads models
   HMM hmms[MODEL_NUM];
	load_models( argv[1], hmms, MODEL_NUM);
   testHMM(hmms,test_data_name,result_filename);
   return 0;
}