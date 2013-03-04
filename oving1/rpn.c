#include "rpn.h"
#include <stdlib.h>

RpnCalc newRpnCalc(){
    RpnCalc calc;
    calc.stack = (double*)malloc(sizeof(double)*8);
    calc.top = -1;
    calc.size = 8;
    return calc;
}

void push(RpnCalc* rpnCalc, double n){
    if(rpnCalc->top == (rpnCalc->size-1)){
        rpnCalc->size *= 2;
        rpnCalc->stack = realloc(rpnCalc->stack, sizeof(double)*rpnCalc->size);
    }
    rpnCalc->top += 1;   
    rpnCalc->stack[rpnCalc->top] = n;
}

void performOp(RpnCalc* rpnCalc, char op){
    double op2 = rpnCalc->stack[rpnCalc->top];
    rpnCalc->top -= 1;
    double op1 = rpnCalc->stack[rpnCalc->top];
    if(op == '+'){
        rpnCalc->stack[rpnCalc->top] = op1+op2;
    } else if(op == '-'){
        rpnCalc->stack[rpnCalc->top] = op1-op2;
    } else if(op == '*'){
        rpnCalc->stack[rpnCalc->top] = op1*op2;
    } else{
        rpnCalc->stack[rpnCalc->top] = op1/op2;
    }
}

double peek(RpnCalc* rpnCalc){
    return rpnCalc->stack[rpnCalc->top];
}
