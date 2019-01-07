#include <cstddef>
#include <iostream>
#include <string>

#define SIN 's'
#define COS 'c'
#define TAN 't'
#define CSC 'k'
#define SEC 'z'
#define COT 'g'
#define LOG 'l'
#define LN 'n'

using namespace std;

int charToInt(char c)
{
    return c-48;
}

bool isInt(char c)
{
    return c >=48 && c <= 57;
}

// Includes letters
bool isOp(char c)
{
    return !(isInt(c) || c == ' ' || c == '.' || c == '(' || c == ')');
}

bool isLowerLetter(char c)
{
    return c >= 97 && c <= 122;
}

double a_to_i(string arg, int begin, int end)
{
    double result = 0;
    int digit;
    bool is_neg = 0;
    // Parse negative number
    if (arg.at(begin) == '-'){
        is_neg = 1;
    }

    // Find index of decimal point
    int ptIndex = end;
    for (int i = is_neg + begin; i < end; ++i){
        if (arg.at(i) == '.'){
            ptIndex = i;
            break;
        }
    }

    // Add numbers before decimal point
    for (int i = is_neg + begin; i < ptIndex; ++i){
        if (!isInt(arg.at(i))){
            return 0;
        }
        // Get char as an int
        digit = charToInt(arg.at(i));

        // Multiply by appropriate power of 10
        result += pow(10.0, ptIndex-1-i) * digit;
    }

    // Add numbers after decimal point
    for (int i = ptIndex + 1; i < end; ++i){
        // Make sure char is in "0123456789"
        if (!isInt(arg.at(i))){
            return 0;
        }
        // Get char as an int
        digit = charToInt(arg[i]);
        // Multiply by appropriate power of 10
        result += pow(10.0, ptIndex-i) * digit;    
    }
    if (is_neg){
        return -result;
    }
    return result;
}

// arg.at(i) == operator
void insertParenBefore(string& arg, size_t i)
{
    --i; // move to char before op
    if (i == 0){
        arg.insert(0, 1, '(');
        return;
    }

    size_t numParens = 0;
    bool hitParens = false;
    while (i > 0 && (!hitParens || numParens != 0) ){
        if (!hitParens && isOp(arg.at(i)) && !isLowerLetter(arg.at(i))){
            break;
        }
        else if (arg.at(i) == ')'){
            hitParens = true;
            ++numParens;
        }
        else if (arg.at(i) == '('){
            --numParens;
        }
        --i;
    } 
    arg.insert(i+1, 1, '(');
}   

// arg.at(i) == operator
void insertParenAfter(string& arg, size_t i)
{
    ++i; // Move to char after op
    size_t length = arg.length();
    size_t numParens = 0;
    bool hitParens = false;
    // Traverse until
    while (i < length && (!hitParens || numParens != 0) ){
        // If we reach another operator, insert the parenthesis
        if (!hitParens && isOp(arg.at(i)) && !isLowerLetter(arg.at(i))){
            break;
        }
        else if (arg.at(i) == '('){
            hitParens = true;
            ++numParens;
        }
        else if (arg.at(i) == ')'){
            --numParens;
        }
        ++i;
    } 
    arg.insert(i, 1, ')');
}

/* Adds parentheses to ensure correct order of operations */
void pemdas(string& arg)
{
    size_t i = 0;
    size_t length = arg.length();
    // ^
    while (i < length){
        if (arg.at(i) == '^'){
            insertParenAfter(arg, i);
            insertParenBefore(arg, i);
            ++i; // '^' is moved forward, need to shift index forward as well
            length += 2; // arg is longer b/c we inserted parentheses
        }
        ++i;
    }

    // * and /
    i = 0;
    while (i < length){
        if (arg.at(i) == '*' || arg.at(i) == '/'){
            insertParenAfter(arg, i);
            insertParenBefore(arg, i);
            ++i; // '/' or '*' is moved forward, need to shift index forward as well
            length += 2; // arg is longer b/c we inserted parentheses
        }
        ++i;
    }
    
}

/* Gets the next number and changes index start to 
 * position at the end of this number 
 */
double getNextNum(string arg, size_t& i)
{
    size_t length = arg.length();
    size_t start = i;
    // For negative #s
    if (arg.at(i) == '-'){
        ++i;
    }
    // Move index to end of number
    while (i < length && (isInt(arg.at(i)) || arg.at(i) == '.') ){
        ++i;
    }

    return a_to_i(arg, start, i);
}

char getNextOp(string arg, size_t& i)
{
    size_t length = arg.length();
    char opName[4] = "   ";
    char c;
    size_t opIndex = 0;

    while (i < length && isOp(arg[i])){
        c = arg[i];
        if (!isLowerLetter(arg[i])){
            ++i;
            return c;
        }
        else{
            opName[opIndex] = c;
            ++opIndex;
            ++i;
        }
    }

    // If opName is a word, return the char that represents it
    if (!strcmp(opName, "sin")){
        return SIN;
    }
    if (!strcmp(opName, "cos")){
        return COS;
    }
    if (!strcmp(opName, "tan")){
        return TAN;
    }
    if (!strcmp(opName, "csc")){
        return CSC;
    }
    if (!strcmp(opName, "sec")){
        return SEC;
    }
    if (!strcmp(opName, "cot")){
        return COT;
    }
    if (!strcmp(opName, "log")){
        return LOG;
    }
    if (!strcmp(opName, "ln ")){
        return LN;
    }
    else{
        cerr << "Invalid operator." << endl;
        exit(EXIT_FAILURE);
    }
}

double doOp(double result, double operand, char op)
{
    switch (op){
        case '+':
            result += operand;
            break;
        case '-':
            result -= operand;
            break;
        case '*':
            result *= operand;
            break;
        case '/':
            result /= operand;
            break;
        case '^':
            result = pow(result, operand);
            break;
    }
    return result;
}

// arg.at(i) == '(' to start or segfault
string getEqInParens(string arg, size_t& i)
{
    size_t start = i;
    size_t numParens = 0;

    // This will move i to one past the last parenthesis
    while (numParens != 0 || i == start){
        if (arg.at(i) == '('){
            ++numParens;
        }
        else if (arg.at(i) == ')'){
            --numParens;
        }
        ++i;
    }
    
    // We don't want the parentheses in our return equation
    string eq;
    eq.reserve(i-start-2);

    for (size_t j = 0; j < i-start-2; ++j){
        eq.append(1, arg[start+j+1]); 
    }

    return eq;
}

// TODO: Treat these operations as if they have parentheses around them
// These will have to be treated differently because now an equation can 
// start with a non-number character
// double doWordOp(//.....){
//     switch (op){
//         case SIN:
//             result = sin(result);
//             break;
//         case COS:
//             result = cos(result);
//             break;
//         case TAN:
//             result = tan(result);
//             break;
//         case CSC:
//             result = 1/sin(result);
//             break;
//         case SEC:
//             result = 1/cos(result);
//             break;
//         case COT:
//             result = 1/tan(result);
//             break;
//     }

//     return result;
// }



double evaluate(string eq, char var, double val)
{
    // TODO: what if first char not an int or (-)?
    double result = 0;
    
    double nextNum = 0.0;
    char op = '+';
    if (eq.at(0) == '-'){
        op = '-';
    }
    bool opNext = false;
    size_t length = eq.length();
    size_t i = 0;
    
    while (i < length){
        // Ignore spaces
        if (eq[i] == ' '){
            ++i;
        }
        // If the dependent variable (e.g. x or n) is the next char
        else if (eq[i] == var){
            result = doOp(result, val, op);
            opNext = true;
            ++i;
        }
        else if (eq[i] == '('){
            nextNum = evaluate(getEqInParens(eq, i), var, val); // This fcn advances i to correct position
            result = doOp(result, nextNum, op);
            opNext = true;
        }
        else{
            // Get a number
            if (!opNext){
                nextNum = getNextNum(eq, i); // This fcn advances i to correct position
                result = doOp(result, nextNum, op);
                opNext = true;
                // cout << "nextNum=" << nextNum << endl;
                // cout << "result=" << result << endl;
            }

            // Get an operator
            else{
                op = getNextOp(eq, i); // This fcn advances i to correct position
                opNext = false;
            }
        }
    }
    
    return result;
}

double integrate(string eq, char var, double start, double end)
{
    double result = 0;
    size_t iterations = 100000; // How many rectangles?
    double deltaX = (end-start)/iterations;
    double xVal = start + deltaX/2; // Uses middle rectangles

    for (size_t i = 0; i < iterations; ++i){
        result += deltaX * evaluate(eq, var, xVal);
        xVal += deltaX;
    }

    return result;  

}

int main(int argc, char** argv)
{
    string operation = argv[1]; 
    string equation = argv[2];
    pemdas(equation);
    //cout << equation << endl;

    // Evaluate
    if (!operation.compare("eval")){
        if (argc == 3){
            cout << evaluate(equation, 'x', 0) << endl;
        }
        else if (argc == 5){
            char var = argv[3][0];
            int val = atoi(argv[4]);
            cout << evaluate(equation, var, val) << endl;
        }
        else{
            cerr << "Usage: equation eval \"<equation>\" <var> <val>" << endl;
        }
    }    
    else if (!operation.compare("int")){
        if (argc == 6){
            char var = argv[3][0];
            int start = atoi(argv[4]);
            int end = atoi(argv[5]);
            cout << integrate(equation, var, start, end) << endl;
        }
        else{
            cerr << "Usage: equation int \"<equation>\" <var> <start> <end>" << endl;
        }
    }
    else{
        cerr << "Invalid operation. Use either eval or int." << endl;
    }

    return 0;
}

// TODO: Multiple variables
