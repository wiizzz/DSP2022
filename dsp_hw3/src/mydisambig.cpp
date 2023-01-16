#include <iostream>
#include <stdio.h>
#include <vector>
#include <map>
#include <string.h>
#include <limits.h>
#include "Ngram.h"

using namespace std;
#define MAX_LINE_LENGTH 100000
#define PROB_MIN -100

typedef struct{
    vector<vector<double> > prob;
    vector<vector<int> > index;
}Table;

void createMap(FILE *mapFile, map<string, vector<string> > &Map){
    char line[MAX_LINE_LENGTH] = {0};
    while (fgets(line, MAX_LINE_LENGTH, mapFile)){
        string data(line);  //Big5 is 2bytes in cpp, char[2]
        string key = data.substr(0,2);  //key = ZhuYin (or Big5 to Big5 case)
        vector<string> words;   
        words.push_back(data.substr(3,2));  //first Big5 word
        int pos = 2;
        while(1) {
            pos = data.find(" ", pos+1);
            if (pos == string::npos) break;
            words.push_back(data.substr(pos+1, 2));
        }
        Map[key] = words;
    }
    vector<string> begin(1, "<s>"), end(1, "</s>");
    Map["<s>"] = begin;
    Map["</s>"] = end;  
    fclose(mapFile);
}

// Get P(W2 | W1) -- bigram
double getBigramProb(Vocab& voc, Ngram& lm,const char *w1, const char *w2){
    VocabIndex wid1 = voc.getIndex(w1);
    VocabIndex wid2 = voc.getIndex(w2);

    if(wid1 == Vocab_None)  //OOV
        wid1 = voc.getIndex(Vocab_Unknown);
    if(wid2 == Vocab_None)  //OOV
        wid2 = voc.getIndex(Vocab_Unknown);

    VocabIndex context[] = { wid1, Vocab_None };
    return lm.wordProb( wid2, context);
}

vector<string> Viterbi(vector<string>& words, Vocab& voc, Ngram &lm, map<string, vector<string> > Map){
    Table T;

    for (int i = 0; i < words.size(); i++) {
        int size;
        if (i == 0)
            size = 1;
        else {
            vector<string> s = Map[words[i]];
            size = s.size();
        }
        vector<double> prob(size, PROB_MIN);
        vector<int>index (size, 0);
        T.prob.push_back(prob);
        T.index.push_back(index);
    }
    T.prob[0][0] = 1;

    for (int i = 1; i < words.size(); i++) {
        vector<string> cur = Map[words[i]];
        vector<string> prev = Map[words[i-1]];

        int j = 0;
        for (auto it = cur.begin(); it != cur.end(); j++, it++) {
            double prob, max_prob = INT_MIN;
            for (int k = T.prob[i-1].size()-1; k >= 0; k--) {
                prob = getBigramProb(voc, lm, prev[k].c_str(), (*it).c_str());
                if (T.prob[i-1][k] + prob > max_prob) {
                    max_prob = T.prob[i-1][k] + prob;
                    T.prob[i][j] = max_prob;
                    T.index[i][j] = k;
                }
            }
        }
    }

    int idx = words.size()-1;
    int maxProb_idx = -1;
    double max_prob = INT_MIN;
    vector<string> vec = Map[words[idx]];
    int w_size = vec.size();
    for (int i = w_size-1; i >= 0; i--) {
        if (T.prob[idx][i] > max_prob) {
            max_prob = T.prob[idx][i];
            maxProb_idx = i;
        }
    }

    vector<string> output (words.size(), "");
    int best[MAX_LINE_LENGTH] = {0};
    for (int i = idx; i >= 0; i--) {
        best[i] = maxProb_idx;
        maxProb_idx = T.index[i][maxProb_idx];
    }

    for (int i = 0; i <= idx; i++) {
        vector<string> vec = Map[words[i]];
        output[i] = vec[best[i]];
    }

    return output;
}

int main(int argc, char *argv[]){
    if(argc != 5){
        cout << "Error input argment" << endl;
        return 0;
    }

    const int ngram_order = 2;    //bigram
    Vocab voc;
    Ngram lm( voc, ngram_order );

    //reading test data 
    FILE *inFile = fopen(argv[1], "r");
    if(!inFile){
        cout << "Reading segmented file error!" << endl ;
        return 0;
    }

    //reading map
    FILE *mapFile = fopen(argv[2], "r");
    if(!mapFile){
        cout << "Reading ZhuYin map error!" << endl;
        return 0;
    }

    map<string, vector<string> > mapping ;
    createMap(mapFile, mapping);
    //read language model
    File lmFile( argv[3], "r" );
    lm.read(lmFile);
    lmFile.close();

    FILE *outFile = fopen(argv[4], "w");
    char data[MAX_LINE_LENGTH] = {0};
    while(fgets(data, MAX_LINE_LENGTH, inFile)){
        vector<string> words;
        words.push_back("<s>");
        for (int i = 0; i < MAX_LINE_LENGTH; i++) {
            if (data[i] == ' ') continue;
            if (data[i] == '\n' || data[i] == '\0') break;
            char substr[3] = {0};
            substr[0] = data[i];
            substr[1] = data[i+1];
            string s(substr);
            words.push_back(s);
            i++;
        }
        words.push_back("</s>");
        vector<string> result = Viterbi(words, voc, lm, mapping);
        result.pop_back();

        for (auto it = result.begin(); it != result.end(); it++) {
            fprintf(outFile, "%s ", (*it).c_str());
        }
        fputs("</s>\n", outFile);

    }
    fclose(inFile);
    fclose(outFile);
    return 0;
}
