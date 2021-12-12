//========================================================//
//  predictor.c                                           //
//  Source file for the Branch Predictor                  //
//                                                        //
//  Implement the various branch predictors below as      //
//  described in the README                               //
//========================================================//
#include <stdio.h>
#include <string.h>
#include "predictor.h"

//
// TODO:Student Information
//
const char *studentName = "NAME";
const char *studentID = "PID";
const char *email = "EMAIL";

//------------------------------------//
//      Predictor Configuration       //
//------------------------------------//

// Handy Global for use in output routines
const char *bpName[4] = {"Static", "Gshare",
                         "Tournament", "Custom"};

int ghistoryBits; // Number of bits used for Global History
int lhistoryBits; // Number of bits used for Local History
int pcIndexBits;  // Number of bits used for PC index
int bpType;       // Branch Prediction Type
int verbose;

//------------------------------------//
//      Predictor Data Structures     //
//------------------------------------//

//
//TODO: Add your own Branch Predictor data structures here
//
uint32_t ghistory;
uint8_t *gshareBHT;

// data structures for tournament predictor
uint32_t *localPHT;
uint8_t *localBHT;
uint8_t *chooser;
uint8_t *globalBHT;
uint32_t globalHistory;
uint8_t localOutcome, globalOutcome;

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Shifting time states
void shift_states(uint8_t *state, uint8_t outcome)
{
    if (outcome == TAKEN) {
        if (*state != ST)
            (*state)++;
    } else {
        if (*state != SN)
            (*state)--;
    }
}

// GShare predictor
uint8_t gshare_prediction(uint32_t pc) {
    uint32_t gBHTIndex = (ghistory ^ pc) & ((1 << ghistoryBits) - 1);
    uint8_t g_pred = gshareBHT[gBHTIndex];
    return (g_pred == SN || g_pred == WN) ? NOTTAKEN : TAKEN;
}

// local predictor
void local_prediction(uint32_t pc) {
    uint32_t pattern = pc & ((1 << pcIndexBits) - 1);
    uint32_t localBHTIndex = localPHT[pattern];
    uint8_t prediction = localBHT[localBHTIndex];

    localOutcome = (prediction == WT || prediction == ST) ? TAKEN : NOTTAKEN;
}

// global predictor
void global_prediction(uint32_t pc) {
    uint32_t globalBHTIndex = (globalHistory) & ((1 << ghistoryBits) - 1);
    uint8_t prediction = globalBHT[globalBHTIndex];

    globalOutcome = (prediction == WT || prediction == ST) ? TAKEN : NOTTAKEN;
}

// tournament prediction
uint8_t hybrid_prediction(uint32_t pc) {
    uint32_t globalBHTIndex = globalHistory & ((1 << ghistoryBits) - 1);
    uint32_t predictor = chooser[globalBHTIndex];
    local_prediction(pc);
    global_prediction(pc);

    return (predictor == WT || predictor == ST) ? localOutcome : globalOutcome;
}

//
void tournament_train(uint32_t pc, uint8_t outcome) {
    if (localOutcome != globalOutcome) {
        shift_states(&chooser[globalHistory], (localOutcome == outcome) ? TAKEN : NOTTAKEN);
    }

    uint32_t localPHTIndex = pc & ((1 << pcIndexBits) - 1);
    uint32_t localBHTIndex = localPHT[localPHTIndex];

    // update PHT states
    shift_states(&localBHT[localBHTIndex], outcome);
    localPHT[localPHTIndex] <<= 1;
    localPHT[localPHTIndex] &= ((1 << lhistoryBits) - 1);
    localPHT[localPHTIndex] |= outcome;
    // update global history states
    shift_states(&globalBHT[globalHistory], outcome);
    globalHistory <<= 1;
    globalHistory &= ((1 << ghistoryBits) - 1);
    globalHistory |= outcome;
}

// Initialize the predictor
//
void init_predictor()
{
    //
    //TODO: Initialize Branch Predictor Data Structures
    //
    switch (bpType)
    {
    case STATIC:
        return;
    case GSHARE:
        ghistory = 0;
        gshareBHT = malloc((1 << ghistoryBits) * sizeof(uint8_t));
        memset(gshareBHT, WN, (1 << ghistoryBits) * sizeof(uint8_t));
        break;
    case TOURNAMENT:
        // initialize local predictor
        localPHT = malloc((1 << pcIndexBits) * sizeof(uint32_t));
        localBHT = malloc((1 << lhistoryBits) * sizeof(uint8_t));
        chooser = malloc((1 << ghistoryBits) * sizeof(uint8_t));
        memset(localPHT, 0, (1 << pcIndexBits) * sizeof(uint32_t));
        memset(localBHT, WN, (1 << lhistoryBits) * sizeof(uint8_t));
        memset(chooser, WN, (1 << ghistoryBits) * sizeof(uint8_t));
        // initialize glocal predictor
        globalHistory = 0;
        globalBHT = malloc((1 << ghistoryBits) * sizeof(uint8_t));
        memset(globalBHT, WN, (1 << ghistoryBits) * sizeof(uint8_t));

        break;
    case CUSTOM:
        // TODO
    default:
        break;
    }
}

// Make a prediction for conditional branch instruction at PC 'pc'
// Returning TAKEN indicates a prediction of taken; returning NOTTAKEN
// indicates a prediction of not taken
//
uint8_t
make_prediction(uint32_t pc)
{
    //
    //TODO: Implement prediction scheme
    //

    // Make a prediction based on the bpType
    switch (bpType)
    {
    case STATIC:
        return TAKEN;
    case GSHARE:
        return gshare_prediction(pc);
    case TOURNAMENT:
        return hybrid_prediction(pc);
    case CUSTOM:
        return NOTTAKEN;
    default:
        return NOTTAKEN;
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
    //
    //TODO: Implement Predictor training
    //
    switch (bpType)
    {
    case STATIC:
        break;
    case GSHARE:
        // first, shift the state
        shift_states(&gshareBHT[(ghistory ^ pc) & ((1 << ghistoryBits) - 1)], outcome);
        ghistory = ghistory << 1;
        ghistory = ghistory | outcome;
        break;
    case TOURNAMENT:
        tournament_train(pc, outcome);
        break;
    case CUSTOM:
        break;
    default:
        break;
    }
}
