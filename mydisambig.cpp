#include <iostream>
#include <fstream>
#include <sstream>
#include <map>
#include <vector>
#include <string>
#include <queue>
#include "Vocab.h"
#include "Ngram.h"

using namespace std;

//global
int ngram_order = 2;
map<string, int> myIdx;
vector<vector<string> > myList;

//build ZhuYin-Big5 map
int buildMap(char* filename){
    fstream mapfile;
    mapfile.open(filename);
    string rawline;
    int cnt = 0;
    while(getline(mapfile, rawline)){

        stringstream ssin(rawline);
        if(ssin.good()){
            string key;
            int value = cnt;
            ssin >> key;
            myIdx.insert(pair<string, int>(key, value));
            myList.push_back(vector<string>());
            while(ssin.good()){
                string character;
                ssin >> character;
                myList[cnt].push_back(character);
            }
            cnt++;
        }
    }
}

//get log P(w2|w1)
double getProb(string w1, string w2, Ngram &lm, Vocab &voc){    
    //implementation for bigram
    VocabIndex dne = voc.getIndex(Vocab_Unknown);
    VocabIndex w1id = voc.getIndex(w1.c_str());
    VocabIndex w2id = voc.getIndex(w2.c_str());
    if(w1id == Vocab_None) w1id = dne;
    if(w2id == Vocab_None) w2id = dne;
    VocabIndex context[] = {w1id, Vocab_None};
    LogP Pbigram = lm.wordProb(w2id, context);
    if(Pbigram == LogP_Zero){
        VocabIndex context[] = {Vocab_None};
        LogP Punigram = lm.wordProb(w2id, context);
        if(Pbigram == LogP_Zero){
            return -1e9;
        }else{
            //cout<<Punigram<<endl;
            return Punigram;
        }
    }else{
        //cout<<Pbigram<<endl;
        return Pbigram;
    }
}

//viterbi search
string viterbiSearch(string rawline, Ngram &lm, Vocab &voc){
    vector<string> orig;
    vector<vector<string> > grid;
    vector<vector<double> > prob;
    vector<vector<int> > psi;

    //raw to vocab index
    stringstream ssin(rawline);
    int len = 0;
    while(ssin.good()){
        string character;
        ssin>>character;
        orig.push_back(character);
        grid.push_back(vector<string>());
        prob.push_back(vector<double>());
        psi.push_back(vector<int>());
        int thisIdx = myIdx[character];
        for(int i = 0; i < myList[thisIdx].size(); ++i){
            grid[len].push_back(myList[thisIdx][i]);
            prob[len].push_back(-1e9);
            psi[len].push_back(0);
        }
        len++;
    }
    //run log version viterbi
    for(int j = 0; j < grid[0].size(); ++j){
        prob[0][j] = getProb("<s>", grid[0][j], lm, voc);
        //cout<<prob[0][j]<<endl;
    }
    for(int i = 1; i < len; ++i){
        for(int j = 0; j < grid[i].size(); ++j){
            for(int k = 0; k < grid[i-1].size(); ++k){
                double Pw1w2 = getProb(grid[i-1][k], grid[i][j], lm, voc);
                //cout<<prob[i-1][k]<<Pw1w2<<endl;
                if(prob[i-1][k] + Pw1w2 > prob[i][j]){
                    prob[i][j] = prob[i-1][k] + Pw1w2;
                    psi[i][j] = k;
                }
            }
        }
    }

    //backtrack
    double max = -1e9;
    int arg_max = 0;
    for(int j = 0; j < grid[len-2].size(); ++j){
        if(prob[len-2][j] >= max){
            max = prob[len-2][j];
            arg_max = j;
        }
    }
    vector<string> backtrack;
    for(int i = len-2; i >= 0; --i){
        backtrack.push_back(grid[i][arg_max]);
        arg_max = psi[i][arg_max];
    }
    
    string result = "<s>";
    for(int i = backtrack.size()-1; i >= 0; --i){
        result = result + " " +backtrack[i];
    }
    result = result + " </s>";
    return result;
    
    return "debug";
}
//testing
int testData(char* filename, Ngram &lm, Vocab &voc){
    fstream testfile;
    testfile.open(filename);
    string rawline;
    while(getline(testfile, rawline)){
        string result = viterbiSearch(rawline, lm, voc);
        cout << result << endl;
    }
}

int main(int argc, char *argv[]){

    Vocab voc;
    Ngram lm(voc, ngram_order);
    if(argc < 4){
        perror("Too few arguments, expected 4");
    }else if(argc > 4){
        perror("Too many arguments, expected 4");
    }
    
    //load language model
    File lmfile(argv[1], "r");
    lm.read(lmfile);
    lmfile.close();
    //cout << "Language Model Loaded" << endl;

    //build zhuin-big5 map
    buildMap(argv[2]);
    //cout << "Character Map Built" << endl;
    
    //call testing
    testData(argv[3], lm, voc);
    //cout << "Testing Complete" << endl;

    return 0;
}