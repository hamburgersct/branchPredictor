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

//------------------------------------//
//        Predictor Functions         //
//------------------------------------//

// Shifting time states
void shift_timeStates(uint8_t *state, uint8_t outcome)
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
        break;
    case GSHARE:
        ghistory = 0;
        gshareBHT = malloc((1 << ghistoryBits) * sizeof(uint8_t));
        memset(gshareBHT, WN, (1 << ghistoryBits) * sizeof(uint8_t));
        break;
    case TOURNAMENT:
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
        break;
    case TOURNAMENT:
        return NOTTAKEN;
        break;
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
        shift_timeStates(&gshareBHT[(ghistory ^ pc) & ((1 << ghistoryBits) - 1)], outcome);
        ghistory = ghistory << 1;
        ghistory = ghistory | outcome;
        break;
    case TOURNAMENT:
        break;
    case CUSTOM:
        break;
    default:
        break;
    }
}
