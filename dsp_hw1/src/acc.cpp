#include <stdio.h>
#include <stdlib.h>
#include "hmm.h"

int main(int argc, char const *argv[])
{
   FILE *pred_file = open_or_die(argv[1], "r");
   FILE *ans_file = open_or_die(argv[2], "r");
   double correct = 0.;
   double total = 0.;
   char pred[MAX_LINE] = "";
   char prob[MAX_LINE] = "";
   char ans[MAX_LINE] = "";

   while (fscanf(pred_file, "%s %s", pred, prob) > 0 && fscanf(ans_file, "%s", ans) > 0)
   {
      if (strcmp(pred, ans) == 0)
         correct++;
      total++;
   }
   fclose(pred_file);
   fclose(ans_file);

   FILE *output = open_or_die(argv[3], "a");
   fprintf(output, "%f\n", correct / total);
   fclose(output);
   return 0;
}