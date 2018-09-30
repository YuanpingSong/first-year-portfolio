//
//  eval.cpp
//  CS32_HW_2
//
//  Created by Yuanping Song on 2/6/18.
//  Copyright © 2018 Yuanping Song. All rights reserved.
//


#include "Map.h"
#include <iostream>
#include <cctype>
#include <cassert>

#include <stack>

using namespace std;


int evaluate(string infix, const Map& values, string& postfix, int& result);
// Evaluates an integer arithmetic expression
//   If infix is a syntactically valid infix integer expression whose
//   only operands are single lower case letters (whether or not they
//   appear in the values map), then postfix is set to the postfix
//   form of the expression; otherwise postfix may or may not be
//   changed, result is unchanged, and the function returns 1.  If
//   infix is syntactically valid but contains at least one lower
//   case letter operand that does not appear in the values map, then
//   result is unchanged and the function returns 2.  If infix is
//   syntactically valid and all its lower case operand letters
//   appear in the values map, then if evaluating the expression
//   (using for each letter in the expression the value in the map
//   that corresponds to it) attempts to divide by zero, then result
//   is unchanged and the function returns 3; otherwise, result is
//   set to the value of the expression and the function returns 0.

bool isOperand(char c) {
    return islower(c);
}

bool hasLesserOrEqualPrecendence(char curr, char top) {
    if (curr == '+' || curr == '-') {
        return true;
    }
    if (top == '*' || top == '/') {
        return true;
    }
    
    return false;
}

// this function checks arithmetic errors (dividing by zero) as well as mapping error (operand not present in map)
int evaluatePostfix(string postfix, const Map& values, bool &keyNotInMap, bool &divideByZero) {
    
    
    // varaible initialization
    divideByZero = false;
    keyNotInMap = false;
    stack<int> ostack;
    
    for (int i = 0; i < postfix.size(); i++) {
        if (isblank(postfix[i])) { // safeguard, however the postfix string passed into this function should not contain any whitespace characters
            continue;
        }
        if (isOperand(postfix[i])) {
            int val = 0;
            if (!values.get(postfix[i], val)) {
                keyNotInMap = true;
                return INT_MIN;
            }
            ostack.push(val);
        } else { // case operator
            ValueType operandTwo = ostack.top();
            ostack.pop();
            ValueType operandOne = ostack.top();
            ostack.pop();
            switch (postfix[i]) {
                case '+':
                    ostack.push(operandOne + operandTwo);
                    break;
                case '-':
                    ostack.push(operandOne - operandTwo);
                    break;
                case '*':
                    ostack.push(operandOne * operandTwo);
                    break;
                case '/':
                    if (operandTwo == 0) {
                        divideByZero = true;
                        ostack.push(1); // continue looking for other operands that might not be in the map.
                        break;
                    }
                    ostack.push(operandOne / operandTwo);
                    
                    break;
                default:
                    cerr << "We should not come here! There is a character in the postfix string that is not an operand, an operator, or a space. the postfix string passed into evaluatePostfix does not have correcxt syntax";
                    break;
            } // end switch
        } // end else
    } // end while
    return ostack.top();
}

// this function checks the syntax of the infix string
bool infixToPostfix(string infix, string &postfix) {
    // empty string check
    if (infix.empty()) {
        return false;
    }
    
    // Parentheses Correspondence Check
    int openCount = 0, closeCount = 0;
    for (char c: infix) {
        if(c == '(') {
            openCount++;
        } else if (c == ')') {
            closeCount++;
        }
        if (openCount < closeCount) {
            return false;
        }
    }
    if (openCount != closeCount) {
        return false;
    }
    
    // argument initializaiton
    postfix = "";
    
    // Local Variable Declaration
    string condensedInfix;
    stack<char> opstack; // a stack of operators
    
    // Create condensedInfix string without spaces
    for (char c: infix) {
        if (!isblank(c)) {
            condensedInfix += c;
        }
    }
    
    // now infix is free of spaces
    infix = condensedInfix;
    // the general technique in this program for ensuring the correctness of the infix string is through induction. the algorithm fist checks whether the fist character may be part of a valid infix expression. Then, assuming the current charactrer is correct, the algorithm checks if the next expression is valid.
    
    // first element check. infix expression must start with an operand or an open parentheses.
    if (!isOperand(infix[0]) && infix[0] != '(') {
        return false;
    }
    
    // Iterate through infix string
    for (int i = 0; i < infix.size(); i++) {
        
        // case operand:
        if (isOperand(infix[i])) {
            // syntax check: Operand can be followed by an operator, a close parentheses, or nothing. But not another operand or an open parentheses.
            // the boolean expression reads: if operand is followed by something and that something is either an open parentheses or another operand, return false
            if (i + 1 < infix.size() && (infix[i + 1] == '(' || isOperand(infix[i + 1]) ) ) {
                return false;
            }
            postfix += infix[i];
        } else {
            switch (infix[i]) {
                case '(':
                    // Syntax Check: An open parentheses must be followed by an operand or another open parentheses. It cannot be followed by an operator, immediately by a close parentheses, or nothing. Furthermore, an open parentheses needs to correspond to a close parentheses down the road.
                    // the boolean expression reads: if the open parentheses is not followed by anything or if the thing that follows it is not an operand or another open parentheses, return false
                    if ( (i + 1) >= infix.size() || !( isOperand(infix[i + 1]) || infix[i + 1] == '(' ) ) {
                        return false;
                    }
                    opstack.push(infix[i]);
                    
                    break;
                case ')':
                    // Syntax Check: A close parentheses can be followed by an operator, another close parentheses or nothing. It cannot be followed by an open parentheses or an operand.
                    // the boolean expression reads: if the close parentheses is followed by something and that something is an open parentheses or an operand, return false
                    if ( (i + 1) < infix.size() && ( infix[i + 1] == '(' || isOperand(infix[i + 1]) ) ) {
                        return false;
                    }
                    while (opstack.top() != '(') {
                        postfix += opstack.top();
                        opstack.pop();
                    }
                    opstack.pop();
                    break;
                case '+': case '-': case '*': case '/':
                    // Syntax Check: Operator must be followed by an operand, or an open parentheses. Operator can not be followed by another operator, an close parentheses, or nothing
                    // the boolean expression reads: if the operator is not followed by anything, or if the thing that follows it is neither an operand nor an open bracket, return false;
                    if (        (i + 1 >= infix.size())    ||    (  !isOperand(infix[i + 1])  &&  infix[i + 1] != '('  )        ) {
                        return false;
                    }
                    while (!opstack.empty() && opstack.top() != '(' && hasLesserOrEqualPrecendence(infix[i], opstack.top())) {
                        postfix += opstack.top();
                        opstack.pop();
                    }
                    opstack.push(infix[i]);
                    break;
                default:
//                    cerr << "We should not come here! There is a character in the postfix string that is not an operand, an operator, a parentheses, or a space";
                    // the syntax of the infix expression is not valid because it contains illegal character.
                    return false;
                    break;
            } // end switch
        } // end else
    } // end for
    while (!opstack.empty()) {
        postfix += opstack.top();
        opstack.pop();
    }
    
    return true;
}

int evaluate(string infix, const Map& values, string& postfix, int& result) {
    if (!infixToPostfix(infix, postfix)) { // modify postfix no matter what. postfix will be correct if infix has correct syntax
        return 1;
    }
    bool keyNotInMap, divideByZero;
    int outcome = evaluatePostfix(postfix, values, keyNotInMap, divideByZero);
    if (keyNotInMap) {
        return 2;
    }
    if (divideByZero) {
        return 3;
    }
    result = outcome;
    return 0;
}
int main()
{
    char vars[] = { 'a', 'e', 'i', 'o', 'u', 'y', '#' };
    int  vals[] = {  3,  -9,   6,   2,   4,   1  };
    Map m;
    for (int k = 0; vars[k] != '#'; k++)
        m.insert(vars[k], vals[k]);
    string pf;
    int answer;
    assert(evaluate("a+ e", m, pf, answer) == 0  &&
           pf == "ae+"  &&  answer == -6);
    answer = 999;
    assert(evaluate("", m, pf, answer) == 1  &&  answer == 999);
    assert(evaluate("a+", m, pf, answer) == 1  &&  answer == 999);
    assert(evaluate("a i", m, pf, answer) == 1  &&  answer == 999);
    assert(evaluate("ai", m, pf, answer) == 1  &&  answer == 999);
    assert(evaluate("()", m, pf, answer) == 1  &&  answer == 999);
    assert(evaluate("y(o+u)", m, pf, answer) == 1  &&  answer == 999);
    assert(evaluate("a+E", m, pf, answer) == 1  &&  answer == 999);
    assert(evaluate("(a+(i-o)", m, pf, answer) == 1  &&  answer == 999);
    // unary operators not allowed:
    assert(evaluate("-a", m, pf, answer) == 1  &&  answer == 999);
    assert(evaluate("a*b", m, pf, answer) == 2  &&
           pf == "ab*"  &&  answer == 999);
    assert(evaluate("y +o *(   a-u)  ", m, pf, answer) == 0  &&
           pf == "yoau-*+"  &&  answer == -1);
    answer = 999;
    assert(evaluate("o/(y-y)", m, pf, answer) == 3  &&
           pf == "oyy-/"  &&  answer == 999);
    assert(evaluate(" a  ", m, pf, answer) == 0  &&
           pf == "a"  &&  answer == 3);
    assert(evaluate("((a))", m, pf, answer) == 0  &&
           pf == "a"  &&  answer == 3);
    cout << "Passed all tests" << endl;
}
