//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "Chi-Hsin Lo";
const char *studentID   = "A53311981";
const char *email       = "c2lo@eng.ucsd.edu";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = { "Static", "Gshare",
                          "Tournament", "Custom" };

//define number of bits required for indexing the BHT here. 
int ghistoryBits = 15; // Number of bits used for Global History
int bpType;       // Branch Prediction Type
int verbose;

//tournament
int pcBits_tour = 12; // Number of bits used for pc to indicate the BHT
int lhistoryBits_tour = 10; // Number of bits used for Branch History
int ghistoryBits_tour = 11;

//perceptron
int weight_bits = 9;
const int ghistoryBits_per = 28;
#define top_entries 112
int threshold = 0;
//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//
//
//TODO: Add your own Branch Predictor data structures here
//
//gshare
uint8_t *bht_gshare;
uint64_t ghistory;

//tournament
uint32_t *local_pht_tour; // branch pattern history table
uint8_t *local_bht_tour; // branch history table for branch pattern
uint8_t *global_bht_tour; // global history table for branch pattern
uint8_t *choice_predictor_tour; // chooser
uint32_t ghistory_tour; // global history register

//perceptron
short int* top_perceptron[top_entries]; // table of perceptrons
uint32_t ghistory_per;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//
//perceptron functions
void init_perceptron() {
    // initialize threshold and top
    //printf("size of short int = %ld\n", sizeof(short int));
    printf("size of history = %d\n", ghistoryBits_per);
    threshold = floor(1.93*ghistoryBits_per) + 14;
    threshold/=4;
    for (int i = 0; i < top_entries; i++){
        top_perceptron[i] = (short int*)malloc((ghistoryBits_per+1) * sizeof(short int));
        for (int j = 0; j < (ghistoryBits_per+1); j++){
            top_perceptron[i][j] = 0;
        }
    }
   
    // initialize ghistory
    ghistory_per = 0;
}

int perceptron_predict(uint32_t pc) {
    //get hash value from pc
    uint32_t hash_value = pc % top_entries;
    short int* weights = top_perceptron[hash_value];
    
    int y_out = weights[0];
    for (int i = 0; i<ghistoryBits_per; i++){
        // get the bit in ghistoryBits_per
        int bit = (ghistory_per>>i)&1;
        if(bit!=0) y_out += weights[i+1];
        else y_out -= weights[i+1];
        //printf("bit = %d\n",bit); fflush(stdout);
    }
    return y_out;
}

void train_perceptron(uint32_t pc, uint8_t outcome) {
    //get lower ghistoryBits of pc
    //uint32_t pc_lower_bits = pc & ((1<<ghistoryBits_per)-1);
    //uint32_t hash_value = pc_lower_bits % top_entries;
    //printf("outcome = %d\n",outcome); fflush(stdout);
    uint32_t hash_value = (pc) % top_entries;
    short int* weights = top_perceptron[hash_value];
    
    //Update weights
    int y_out = perceptron_predict(pc);
    if( ((y_out>0) != outcome) || (abs(y_out) <= threshold) ){
        if ((outcome == 1) && (weights[0]<floor(pow(2, weight_bits)))){
            weights[0] += 1;
        }
        else if ((outcome == 0) && (weights[0]>-floor(pow(2, weight_bits)))){
            weights[0] -= 1;
        }
        for (int i = 0; i<ghistoryBits_per; i++){
            int bit = (ghistory_per>>i)&1;
            if ((outcome == bit) && (weights[i+1]<floor(pow(2, weight_bits)))){
                weights[i+1] += 1;
            }
            else if ((outcome != bit) && (weights[i+1]>-floor(pow(2, weight_bits)))){
                weights[i+1] -= 1;
            }
        }
    }

    //Update history register
    ghistory_per = ((ghistory_per << 1) | outcome) & ((1<<ghistoryBits_per)-1);
}

void cleanup_perceptron(){
    for (int i = 0; i < top_entries; i++){
        free(top_perceptron[i]);
    }
}

// tournament functions
void init_tournament() {
    //printf("init_tournament\n");
    uint32_t pht_entries = 1 << pcBits_tour;
    uint32_t local_bht_entries = 1 << lhistoryBits_tour;
    uint32_t global_bht_entries = 1 << ghistoryBits_tour;
    
    local_pht_tour = (uint32_t*)malloc(pht_entries * sizeof(uint32_t));
    local_bht_tour = (uint8_t*)malloc(local_bht_entries * sizeof(uint8_t));
    global_bht_tour = (uint8_t*)malloc(global_bht_entries * sizeof(uint8_t));
    choice_predictor_tour = (uint8_t*)malloc(global_bht_entries * sizeof(uint8_t));
    
    for(int i = 0; i< pht_entries; i++){
        local_pht_tour[i] = WN;
    }
    for(int i = 0; i< local_bht_entries; i++){
        local_bht_tour[i] = WN;
    }
    for(int i = 0; i< global_bht_entries; i++){
        global_bht_tour[i] = WN;
        choice_predictor_tour[i] = WN;
    }
    ghistory_tour = 0;
}

uint8_t tournament_predict(uint32_t pc) {
  //printf("tournament_predict\n"); fflush(stdout);
  //get lower ghistoryBits of pc
  uint32_t pht_entries = 1 << pcBits_tour;
  uint32_t global_bht_entries = 1 << ghistoryBits_tour;
  
  uint32_t pc_lower_bits = pc & (pht_entries-1);
  uint32_t local_bht_index = local_pht_tour[pc_lower_bits];
  uint32_t global_bht_index = ghistory_tour & (global_bht_entries-1);
  //-----------------------------------------------------------------
  //local & global prediction
  uint8_t local_prediction = local_bht_tour[local_bht_index]/2;
  uint8_t global_prediction = global_bht_tour[global_bht_index]/2;
  /*printf("local_bht_index = %d\n", local_bht_index);
    printf("local_prediction = %d\n", local_prediction);
    printf("gocal_bht_index = %d\n", global_bht_index);
    printf("global_prediction = %d\n", global_prediction);
    fflush(stdout);*/
  //-----------------------------------------------------------------
  //predictions match
  if (local_prediction == global_prediction) return local_prediction;

  //predictions doesn't match
  switch(choice_predictor_tour[global_bht_index]){
    case WN: // 01 local
          return local_prediction;
    case SN: // 00 local
          return local_prediction;
    case WT: // 10 global
          return global_prediction;
    case ST: // 11 global
          return global_prediction;
    default:
      printf("Warning: Undefined state of entry in TOURNAMENT BHT!\n");
      return NOTTAKEN;
  }
}

void train_tournament(uint32_t pc, uint8_t outcome) {
    //printf("train_tournament\n");
    //-----------------------------------------------------------------
    //get lower ghistoryBits of pc
    uint32_t pht_entries = 1 << pcBits_tour;
    uint32_t local_bht_entries = 1 << lhistoryBits_tour;
    uint32_t global_bht_entries = 1 << ghistoryBits_tour;
    
    uint32_t pc_lower_bits = pc & (pht_entries-1);
    uint32_t local_bht_index = local_pht_tour[pc_lower_bits];
    uint32_t global_bht_index = ghistory_tour & (global_bht_entries-1);
    //-----------------------------------------------------------------
    //local & global prediction
    uint8_t local_prediction = local_bht_tour[local_bht_index]/2;
    uint8_t global_prediction = global_bht_tour[global_bht_index]/2;
    //-----------------------------------------------------------------
    //Update local_pht and local_bht
    switch(local_bht_tour[local_bht_index]){
      case WN:
        local_bht_tour[local_bht_index] = (outcome==TAKEN)?WT:SN;
        break;
      case SN:
        local_bht_tour[local_bht_index] = (outcome==TAKEN)?WN:SN;
        break;
      case WT:
        local_bht_tour[local_bht_index] = (outcome==TAKEN)?ST:WN;
        break;
      case ST:
        local_bht_tour[local_bht_index] = (outcome==TAKEN)?ST:WT;
        break;
      default:
        printf("Warning: Undefined state of entry in TOURNAMENT LOCAL BHT!\n");
    }
    local_pht_tour[pc_lower_bits] = ((local_bht_index << 1) | outcome) & (local_bht_entries-1);

    //Update global_bht
    switch(global_bht_tour[global_bht_index]){
      case WN:
        global_bht_tour[global_bht_index] = (outcome==TAKEN)?WT:SN;
        break;
      case SN:
        global_bht_tour[global_bht_index] = (outcome==TAKEN)?WN:SN;
        break;
      case WT:
        global_bht_tour[global_bht_index] = (outcome==TAKEN)?ST:WN;
        break;
      case ST:
        global_bht_tour[global_bht_index] = (outcome==TAKEN)?ST:WT;
        break;
      default:
        printf("Warning: Undefined state of entry in TOURNAMENT GLOBAL BHT!\n");
    }
    
    //-----------------------------------------------------------------
    //Update chooser only when global and local predictor disagree
    if(local_prediction != global_prediction){
        switch(choice_predictor_tour[global_bht_index]){
          case WN:
            choice_predictor_tour[global_bht_index] = (outcome==global_prediction)?WT:SN;
            break;
          case SN:
            choice_predictor_tour[global_bht_index] = (outcome==global_prediction)?WN:SN;
            break;
          case WT:
            choice_predictor_tour[global_bht_index] = (outcome==global_prediction)?ST:WN;
            break;
          case ST:
            choice_predictor_tour[global_bht_index] = (outcome==global_prediction)?ST:WT;
            break;
          default:
            printf("Warning: Undefined state of entry in TOURNAMENT GLOBAL BHT!\n");
        }
    }
    //Update history register
    ghistory_tour = ((ghistory_tour << 1) | outcome) & (global_bht_entries-1);
}

void cleanup_tournament() {
    free(local_pht_tour);
    free(local_bht_tour);
    free(global_bht_tour);
    free(choice_predictor_tour);
}


//gshare functions
void init_gshare() {
 int bht_entries = 1 << ghistoryBits;
  bht_gshare = (uint8_t*)malloc(bht_entries * sizeof(uint8_t));
  int i = 0;
  for(i = 0; i< bht_entries; i++){
    bht_gshare[i] = WN;
  }
  ghistory = 0;
}

uint8_t
gshare_predict(uint32_t pc) {
  //get lower ghistoryBits of pc
  uint32_t bht_entries = 1 << ghistoryBits;
  uint32_t pc_lower_bits = pc & (bht_entries-1);
  uint32_t ghistory_lower_bits = ghistory & (bht_entries -1);
  uint32_t index = pc_lower_bits ^ ghistory_lower_bits;
  switch(bht_gshare[index]){
    case WN:
      return NOTTAKEN;
    case SN:
      return NOTTAKEN;
    case WT:
      return TAKEN;
    case ST:
      return TAKEN;
    default:
      printf("Warning: Undefined state of entry in GSHARE BHT!\n");
      return NOTTAKEN;
  }
}

void
train_gshare(uint32_t pc, uint8_t outcome) {
  //get lower ghistoryBits of pc
  uint32_t bht_entries = 1 << ghistoryBits;
  uint32_t pc_lower_bits = pc & (bht_entries-1);
  uint32_t ghistory_lower_bits = ghistory & (bht_entries -1);
  uint32_t index = pc_lower_bits ^ ghistory_lower_bits;

  //Update state of entry in bht based on outcome
  switch(bht_gshare[index]){
    case WN:
      bht_gshare[index] = (outcome==TAKEN)?WT:SN;
      break;
    case SN:
      bht_gshare[index] = (outcome==TAKEN)?WN:SN;
      break;
    case WT:
      bht_gshare[index] = (outcome==TAKEN)?ST:WN;
      break;
    case ST:
      bht_gshare[index] = (outcome==TAKEN)?ST:WT;
      break;
    default:
      printf("Warning: Undefined state of entry in GSHARE BHT!\n");
  }

  //Update history register
  ghistory = ((ghistory << 1) | outcome);
}


void
cleanup_gshare() {
  free(bht_gshare);
}

void init_predictor()
{
  switch (bpType) {
    case STATIC:
    case GSHARE:
        init_gshare();
        break;
    case TOURNAMENT:
        init_tournament();
        break;
    case CUSTOM:
        init_perceptron();
        break;
    default:
        break;
  }
  
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t make_prediction(uint32_t pc)
{

  // Make a prediction based on the bpType
  switch (bpType) {
    case STATIC:
        return TAKEN;
    case GSHARE:
        return gshare_predict(pc);
    case TOURNAMENT:
        return tournament_predict(pc);
    case CUSTOM:
        return perceptron_predict(pc)>0;
    default:
        break;
  }

  // If there is not a compatable bpType then return NOTTAKEN
  return NOTTAKEN;
}

// Train the predictor the last executed branch at PC 'pc' and with
// outcome 'outcome' (true indicates that the branch was taken, false
// indicates that the branch was not taken)
//

void train_predictor(uint32_t pc, uint8_t outcome)
{

  switch (bpType) {
    case STATIC:
    case GSHARE:
        return train_gshare(pc, outcome);
    case TOURNAMENT:
        return train_tournament(pc, outcome);
    case CUSTOM:
        return train_perceptron(pc, outcome);
    default:
      break;
  }
}

void cleanup_predictor()
{

  switch (bpType) {
    case STATIC:
    case GSHARE:
          return cleanup_gshare();
    case TOURNAMENT:
          return cleanup_tournament();
    case CUSTOM:
          return cleanup_perceptron();
    default:
      break;
  }
  

}
