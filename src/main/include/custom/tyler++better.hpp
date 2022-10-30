// better Tyler++

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include "StringStream.hpp"
#include <map>
#include <functional>

namespace tpp{
    class TPPInterpreter;
    struct tpp_type;
    std::vector<tpp_type*> _allTPPObjects;

    #define TPP_FUNCTION_ARGS TPPInterpreter* interpreter
    typedef void (TPP_EXTERNAL_FUNCTION_TYPE)(TPP_FUNCTION_ARGS);

    struct tpp_type{
        tpp_type(){
            _allTPPObjects.push_back(this);
        }
        ~tpp_type(){
            _allTPPObjects.erase(std::remove(_allTPPObjects.begin(), _allTPPObjects.end(), this), _allTPPObjects.end());
        }
        bool* boolean = 0;
        std::string* string = 0;
        double* number = 0;
        TPP_EXTERNAL_FUNCTION_TYPE* externFunction = 0;
        std::string* function = 0;
        std::string* symbol = 0; // Variable name basically

        std::string name = "";
        int rID; // TODO: implement

        int rCount = 0;
    };

    void TPP_INCREF(tpp_type* thing){ // Call TPP_INCREF when a reference to a tpp object is *stored*.
        thing -> rCount ++;
    }

    void TPP_DECREF(tpp_type* thing){ // Call TPP_DECREF when a stored reference to a tpp object is removed.
        thing -> rCount --;
    }

    tpp_type* MAKE_TPP_EXTERN_FUNCTION(TPP_EXTERNAL_FUNCTION_TYPE function){
        tpp_type* ret = new tpp_type;
        ret -> externFunction = function;
        return ret;
    }

    tpp_type* MAKE_TPP_STRING(std::string init){
        tpp_type* ret = new tpp_type;
        ret -> string = new std::string(init);
        return ret;
    }

    tpp_type* MAKE_TPP_NUMBER(double init){
        tpp_type* ret = new tpp_type;
        ret -> number = new double{init};
        return ret;
    }

    tpp_type* MAKE_TPP_FUNCTION(std::string function){
        tpp_type* ret = new tpp_type;
        ret -> function = new std::string(function);
        return ret;
    }

    tpp_type* MAKE_TPP_BOOLEAN(bool init){
        tpp_type* ret = new tpp_type;
        ret -> boolean = new bool{init};
        return ret;
    }

    tpp_type* MAKE_TPP_BOOLEAN(std::string init){
        bool retBool;
        if (init == "true"){
            retBool = true;
        }
        else if (init == "false"){
            retBool = false;
        }
        else{
            std::cout << "ERROR: It's not a boolean. Returning null pointer." << std::endl;
            return nullptr;
        }
        return MAKE_TPP_BOOLEAN(retBool);
    }

    tpp_type* MAKE_TPP_SYMBOL(std::string init){
        tpp_type* ret = new tpp_type;
        ret -> symbol = new std::string(init);
        return ret;
    }

    bool isNumber(std::string val){
        if (val.size() == 0){
            return false;
        }
        for (char x : val){
            if (x != '.' && (x < '0' || x > '9')){
                return false;
            }
        }
        return true;
    }

    tpp_type* MAKE_TPP_TYPE(std::string init){
        if (isNumber(init)){
            return MAKE_TPP_NUMBER(std::stoi(init));
        }
        else if (init == "true" || init == "false"){
            return MAKE_TPP_BOOLEAN(init);
        }
        else{
            return MAKE_TPP_SYMBOL(init);
        }
        std::cout << "ERROR: Somehow your input doesn't fit any possible type, including symbol." << std::endl << "THIS IS AN IMPLEMENTATION ISSUE! Your code is not invalid; the interpreter is." << std::endl << "Returning null pointer." << std::endl;
        return nullptr;
    }

    void TPP_TYPE_SET_NAME(tpp_type* thing, std::string name){
        thing -> name = name;
    }

    int TPP_GET_TYPE(tpp_type* obj){
        if (obj -> boolean){
            return 0;
        }
        else if (obj -> string){
            return 1;
        }
        else if (obj -> number){
            return 2;
        }
        else if (obj -> externFunction){
            return 3;
        }
        else if (obj -> function){
            return 4;
        }
        else if (obj -> symbol){
            return 5;
        }
        return -1;
    }

    tpp_type* TPP_TYPE_DUPLICATE(tpp_type* toDupe){
        int type = TPP_GET_TYPE(toDupe);
        if (type == 0){
            return MAKE_TPP_BOOLEAN(*(toDupe -> boolean));
        }
        else if (type == 1){
            return MAKE_TPP_STRING(*(toDupe -> string));
        }
        else if (type == 2){
            return MAKE_TPP_NUMBER(*(toDupe -> number));
        }
        else if (type == 3){
            return MAKE_TPP_EXTERN_FUNCTION(*(toDupe -> externFunction));
        }
        else if (type == 4){
            return MAKE_TPP_FUNCTION(*(toDupe -> function));
        }
        else if (type == 5){
            return MAKE_TPP_SYMBOL(*(toDupe -> symbol));
        }
        std::cout << "ERROR: The object to duplicate has no type. THIS IS NOT A PROBLEM WITH YOUR CODE; IT IS AN IMPLEMENTATION ISSUE. Please report this immediately." << std::endl;
        std::cout << "Returning null pointer." << std::endl;
        return nullptr;
    }

    std::string PRINTABLE_STRING_TYPE(int type){
        switch (type){
            case 0:
                return "boolean";
                break;
            case 1:
                return "string";
                break;
            case 2:
                return "number";
                break;
            case 3:
                return "external function";
                break;
            case 4:
                return "function";
                break;
            case 5:
                return "symbol";
                break;
        }
        return "typeless";
    }

    std::string PRINTABLE_STRING_NAME(tpp_type* obj){
        if (obj -> name == ""){
            return "UNNAMED";
        }
        return obj -> name;
    }

    std::string PRINTABLE_STRING(tpp_type* obj){
        switch (TPP_GET_TYPE(obj)){
            case 0:
                return (*obj -> boolean ? "true" : "false");
                break;
            case 1:
                return *(obj -> string);
                break;
            case 2:
                return std::to_string(*obj -> number);
                break;
            case 3:
                return "<C++ Defined Method>";
                break;
            case 4:
                return ">" + *obj -> function;
                break;
            case 5:
                return *obj -> symbol;
                break;
        }
        return "<UNPRINTABLE>";
    }


    std::string PRINTABLE_FULL(tpp_type* obj){
        return "(" + PRINTABLE_STRING_TYPE(TPP_GET_TYPE(obj)) + " " + PRINTABLE_STRING_NAME(obj) + "): " + PRINTABLE_STRING(obj);
    }


    bool TPP_EQUALS(tpp_type* first, tpp_type* second){
        int firstType = TPP_GET_TYPE(first);
        int secondType = TPP_GET_TYPE(second);
        if (firstType != secondType){ // This is not Javascript.
            return false;
        }
        switch (firstType){
            case 0:
                return *(first -> boolean) == *(second -> boolean);
                break;
            case 1:
                return *(first -> string) == *(second -> string);
                break;
            case 2:
                return *(first -> number) == *(second -> number);
                break;
            // Comparing methods ain't allowed, so no 3 or 4.
            // Case 5 is not included because comparing symbols should never be done
        }
        return false;
    }


    struct TPP_METHODS{
        static void pStack(TPPInterpreter* interpreter);
        static void push(TPPInterpreter* interpreter);
        static void call(TPPInterpreter* interpreter);
        static void store(TPPInterpreter* interpreter);
        static void pGlobals(TPPInterpreter* interpreter);
        static void def(TPPInterpreter* interpreter);
        static void endDef(TPPInterpreter* interpreter);
        static void whileL(TPPInterpreter* interpreter);
        static void forL(TPPInterpreter* interpreter);
        static void dupl(TPPInterpreter* interpreter);
        static void breakL(TPPInterpreter* interpreter);
        static void getStored(TPPInterpreter* interpreter);
        static void pop(TPPInterpreter* interpreter);

        static void add(TPPInterpreter* interpreter);
        static void sub(TPPInterpreter* interpreter);
        static void mul(TPPInterpreter* interpreter);
        static void div(TPPInterpreter* interpreter);

        static void equals(TPPInterpreter* interpreter);
        static void andB(TPPInterpreter* interpreter);
        static void orB(TPPInterpreter* interpreter);
        static void notB(TPPInterpreter* interpreter);
        static void ifF(TPPInterpreter* interpreter);
        static void breakIf(TPPInterpreter* interpreter);
    };


    class TPPInterpreter{
    public:
        std::vector<tpp_type*> stack;
        std::vector<tpp_type*> callStack;
        StringStream curLine;
        std::map <std::string, tpp_type*> namedGlobals; // All named things go here.
        std::vector <tpp_type*> allGlobals; // All things in general go here.

        std::map <std::string, std::function<void (TPPInterpreter*)>> methods;

        int storeFunctionLevel = 0;
        std::string stored;
        std::string storeEndCommand;

        bool breakLoop = false;

        void storeAll(std::string endCommand){
            stored = "";
            storeEndCommand = endCommand;
            storeFunctionLevel ++;
        }

        void pushStack(tpp_type* item){
            TPP_INCREF(item);
            stack.push_back(item);
        }

        tpp_type* popStack(){
            tpp_type* ret;
            if (stack.size() > 0){
                ret = stack[stack.size() - 1];
            }
            else{
                std::cout << "WARNING: Attempt to pop from an empty stack. Methods and functions without a null check (or equivalent) will likely fail and segfault." << std::endl;
                std::cout << "Returning nullptr." << std::endl;
                return nullptr;
            }
            stack.pop_back();
            return ret;
        }

        void setVariable(tpp_type* var, std::string name = ""){
            bool doesAlreadyExist = namedGlobals[name];
            if (name != ""){
                TPP_INCREF(var);
                TPP_TYPE_SET_NAME(var, name);
                namedGlobals[name] = var;
            }
            if (!doesAlreadyExist){
                allGlobals.push_back(var);
            }
        }

        void registerFunction(TPP_EXTERNAL_FUNCTION_TYPE function, std::string name = ""){
            setVariable(MAKE_TPP_EXTERN_FUNCTION(function), name);
        }

        void call(std::string name = ""){
            tpp_type* fun;
            if (name == ""){
                fun = popStack();
                pushStack(fun); // Don't actually remove functions from the stack; that'd be bad
            }
            else{
                fun = getGlobalByName(name);
            }
            call(fun);
        }

        void call(tpp_type* fun){
            int type = TPP_GET_TYPE(fun);
            if (type == 3){
                fun -> externFunction(this);
            }
            else if (type == 4){
                interpret(*(fun -> function));
            }
            else{
                std::cout << "ERROR: " << PRINTABLE_FULL(fun) << " is not a function." << std::endl;
            }
        }

        tpp_type* getGlobalByName(std::string name){
            return namedGlobals[name];
        }

        void interpretLine(){
            std::string op = curLine.readUntil(' ');
            //std::cout << op << std::endl;
            if (storeFunctionLevel > 0){
                if (op == "def"){
                    storeFunctionLevel ++;
                }
                if (op == storeEndCommand){
                    if (storeFunctionLevel == 1){
                        methods[op](this);
                    }
                    else{
                        stored += curLine.buffer + "; ";
                    }
                    storeFunctionLevel --;
                }
                else{
                    stored += curLine.buffer + "; ";
                }
                return;
            }
            bool string = false;
            std::string stringBuf;
            bool escape = false;
            while (!curLine.isEmpty()){
                if (string){
                    escape = false;
                    if (curLine.peek() == '\\'){
                        escape = true;
                        curLine.read();
                    }
                    char item = curLine.read();
                    if (escape && item == 'n'){
                        stringBuf += '\n';
                    }
                    else if (!escape && item == '"'){
                        pushStack(MAKE_TPP_STRING(stringBuf));
                        stringBuf.erase();
                        string = false;
                    }
                    else{
                        stringBuf += item;
                    }
                    continue;
                }
                if (curLine.peek() == '"'){
                    curLine.read();
                    string = true;
                    continue;
                }
                std::string thing = curLine.readUntil(' ');
                if (thing.size() > 0){
                    pushStack(MAKE_TPP_TYPE(thing));
                }
            }
            if (methods[op]){
                methods[op](this);
            }
            else{
                std::cout << "ERROR: Invalid operation " << op << std::endl;
            }
        }

        TPPInterpreter(){
            methods["push"] = TPP_METHODS::push;
            methods["pStack"] = TPP_METHODS::pStack;
            methods["call"] = TPP_METHODS::call;
            methods["store"] = TPP_METHODS::store;
            methods["pGlobals"] = TPP_METHODS::pGlobals;
            methods["def"] = TPP_METHODS::def;
            methods["endDef"] = TPP_METHODS::endDef;
            methods["dupl"] = TPP_METHODS::dupl;
            methods["break"] = TPP_METHODS::breakL;
            methods["while"] = TPP_METHODS::whileL;
            methods["for"] = TPP_METHODS::forL;
            methods["getStored"] = TPP_METHODS::getStored;
            methods["pop"] = TPP_METHODS::pop;

            methods["add"] = TPP_METHODS::add;
            methods["sub"] = TPP_METHODS::sub;
            methods["div"] = TPP_METHODS::div;
            methods["mul"] = TPP_METHODS::mul;

            methods["if"] = TPP_METHODS::ifF;
            methods["breakIf"] = TPP_METHODS::breakIf;
            methods["equals"] = TPP_METHODS::equals;
            methods["and"] = TPP_METHODS::andB;
            methods["or"] = TPP_METHODS::orB;
            methods["not"] = TPP_METHODS::notB;
        }

        void interpret(std::string program){
            uint8_t state = 0;
            std::string line;
            bool comment = false;
            for (char character : program){
                if (character == '/'){
                    comment = true;
                }
                if (comment){
                    if (character == '\n'){
                        comment = false;
                    }
                    continue;
                }
                if (state == 0){
                    if (character != ' '){
                        state = 1;
                    }
                }
                if (state == 1){
                    if (character == ';'){
                        if (line[line.size() - 1] == ' '){
                            line.erase(line.size() - 1, 1); // Erase the last character if it's a space; we know the last two are never gonna both be space because of the below code.
                        }
                        curLine.flush();
                        curLine.add(line);
                        line.erase();
                        interpretLine();
                        state = 0;
                        continue;
                    }
                    if (character == '\n'){
                        character = ' ';
                    }
                    if (character == ' ' && line.size() > 0 && line[line.size() - 1] == ' '){
                        continue; // Trim extra spaces
                    }
                    if (character != ' ' || line.size() > 0){
                        line += character;
                    }
                }
            }
        }

        tpp_type* getFunctionFromStack(){
            tpp_type* fun = popStack();
            int type = TPP_GET_TYPE(fun);
            if (type == 5){
                fun = getGlobalByName(*(fun -> symbol));
            }
            else if (type != 3 && type != 4){
                std::cout << "ERROR: No function or function name specified to call." << std::endl;
                std::cout << "Returning nullptr." << std::endl;
                return nullptr;
            }
            else{
                pushStack(fun);
            }
            return fun;
        }
    };


    void TPP_METHODS::pStack(TPPInterpreter* interpreter){
        std::cout << "   Tyler++ stack" << std::endl;
        for (tpp_type* obj : interpreter -> stack){
            std::cout << "> " + PRINTABLE_FULL(obj) << std::endl;
        }
    }

    void TPP_METHODS::push(TPPInterpreter* interpreter){ /* Leave it empty - the push happens automatically during interpretation. */

    }

    void TPP_METHODS::call(TPPInterpreter* interpreter){
        interpreter -> call(interpreter -> getFunctionFromStack());
    }

    void TPP_METHODS::store(TPPInterpreter* interpreter){
        interpreter -> setVariable(interpreter -> popStack(), *(interpreter -> popStack() -> symbol));
    }

    void TPP_METHODS::pGlobals(TPPInterpreter* interpreter){
        std::cout << "   Tyler++ globals" << std::endl;
        for (tpp_type* obj : interpreter -> allGlobals){
            std::cout << "> " + PRINTABLE_FULL(obj) << std::endl;
        }
    }

    void TPP_METHODS::def(TPPInterpreter* interpreter){
        interpreter -> storeAll("endDef"); // Store all lines until it reaches "endDef".
    }

    void TPP_METHODS::endDef(TPPInterpreter* interpreter){
        if (!interpreter -> storeFunctionLevel){
            std::cout << "ERROR: endDef called without a coinciding def first. THIS IS A PROBLEM WITH YOUR CODE." << std::endl;
            return;
        }
        tpp_type* name = interpreter -> popStack();
        tpp_type* function = MAKE_TPP_FUNCTION(interpreter -> stored);
        if (name == nullptr || TPP_GET_TYPE(name) != 5){
            if (name != nullptr){ // Null check.
                interpreter -> pushStack(name); // It's not a function name, so restore it to stack. It probably has a good reason to be there.
            }
            interpreter -> setVariable(function);
        }
        else{
            interpreter -> setVariable(function, *(name -> symbol));
        }
        interpreter -> pushStack(function);
    }

    void TPP_METHODS::dupl(TPPInterpreter* interpreter){
        tpp_type* toDupe = interpreter -> popStack();
        interpreter -> pushStack(TPP_TYPE_DUPLICATE(toDupe));
        interpreter -> pushStack(TPP_TYPE_DUPLICATE(toDupe));
    }

    void TPP_METHODS::add(TPPInterpreter* interpreter){
        tpp_type* first = interpreter -> popStack();
        tpp_type* second = interpreter -> popStack();
        interpreter -> pushStack(MAKE_TPP_NUMBER(*(first -> number)  +  *(second -> number)));
    }

    void TPP_METHODS::sub(TPPInterpreter* interpreter){
        tpp_type* first = interpreter -> popStack();
        tpp_type* second = interpreter -> popStack();
        interpreter -> pushStack(MAKE_TPP_NUMBER(*(first -> number)  -  *(second -> number)));
    }

    void TPP_METHODS::div(TPPInterpreter* interpreter){
        tpp_type* first = interpreter -> popStack();
        tpp_type* second = interpreter -> popStack();
        interpreter -> pushStack(MAKE_TPP_NUMBER(*(first -> number)  /  *(second -> number)));
    }

    void TPP_METHODS::mul(TPPInterpreter* interpreter){
        tpp_type* first = interpreter -> popStack();
        tpp_type* second = interpreter -> popStack();
        interpreter -> pushStack(MAKE_TPP_NUMBER(*(first -> number)  *  *(second -> number)));
    }

    void TPP_METHODS::breakL(TPPInterpreter* interpreter){
        interpreter -> breakLoop = true;
    }

    void TPP_METHODS::whileL(TPPInterpreter* interpreter){
        tpp_type* fun = interpreter -> getFunctionFromStack();
        while (!interpreter -> breakLoop){
            interpreter -> call(fun);
        }
        interpreter -> breakLoop = false;
    }

    void TPP_METHODS::forL(TPPInterpreter* interpreter){
        tpp_type* its = interpreter -> popStack();
        tpp_type* fun = interpreter -> getFunctionFromStack();
        if (TPP_GET_TYPE(its) == 2){
            for (int i = 0; i < *(its -> number); i ++){
                interpreter -> call(fun);
            }
        }
        else{
            std::cout << "ERROR: No iterator specified!" << std::endl;
        }
    }

    void TPP_METHODS::getStored(TPPInterpreter* interpreter){
        tpp_type* var = interpreter -> popStack();
        if (TPP_GET_TYPE(var) == 5){
            interpreter -> pushStack(interpreter -> getGlobalByName(*var -> symbol));
        }
        else{
            std::cout << "ERROR: Not a valid variable name." << std::endl;
        }
    }

    void TPP_METHODS::equals(TPPInterpreter* interpreter){
        interpreter -> pushStack(MAKE_TPP_BOOLEAN(TPP_EQUALS(interpreter -> popStack(), interpreter -> popStack())));
    }

    void TPP_METHODS::ifF(TPPInterpreter* interpreter){
        tpp_type* fun = interpreter -> getFunctionFromStack();
        tpp_type* condition = interpreter -> popStack();
        if (*(condition -> boolean)){
            interpreter -> call(fun);
        }
    }

    void TPP_METHODS::andB(TPPInterpreter* interpreter){
        interpreter -> pushStack(MAKE_TPP_BOOLEAN(*(interpreter -> popStack() -> boolean) && *(interpreter -> popStack() -> boolean)));
    }

    void TPP_METHODS::orB(TPPInterpreter* interpreter){
        interpreter -> pushStack(MAKE_TPP_BOOLEAN(*(interpreter -> popStack() -> boolean) || *(interpreter -> popStack() -> boolean)));
    }

    void TPP_METHODS::notB(TPPInterpreter* interpreter){
        interpreter -> pushStack(MAKE_TPP_BOOLEAN(!(*(interpreter -> popStack() -> boolean))));
    }

    void TPP_METHODS::pop(TPPInterpreter* interpreter){
        interpreter -> popStack();
    }

    void TPP_METHODS::breakIf(TPPInterpreter* interpreter){
        tpp_type* condition = interpreter -> popStack();
        if (*(condition -> boolean)){
            interpreter -> breakLoop = true;
        }
    }



    void EXTERN_print(TPP_FUNCTION_ARGS){
        std::cout << PRINTABLE_STRING(interpreter -> popStack());
    }

    void EXTERN_printV(TPP_FUNCTION_ARGS){
        std::cout << PRINTABLE_FULL(interpreter -> popStack()) << std::endl;
    }

    void EXTERN_input(TPP_FUNCTION_ARGS){
        std::string in;
        std::getline(std::cin, in);
        interpreter -> pushStack(MAKE_TPP_STRING(in));
    }

    void EXTERN_stringMerge(TPP_FUNCTION_ARGS){
        tpp_type* stringCount = interpreter -> popStack();
        int numStrings = -1;
        if (TPP_GET_TYPE(stringCount) == 2){
            numStrings = *(stringCount -> number);
        }
        else{
            interpreter -> pushStack(stringCount); // Don't be a stack thief.
        }
        std::string result = "";
        if (numStrings == -1){
            tpp_type* crSTR = interpreter -> popStack();
            while (crSTR != nullptr && TPP_GET_TYPE(crSTR) == 1){
                result = *(crSTR -> string) + result;
                crSTR = interpreter -> popStack();
            }
        }
        else{
            for (int i = 0; i < *(stringCount -> number); i ++){
                result = *(interpreter -> popStack() -> string) + result;
            }
        }
        interpreter -> pushStack(MAKE_TPP_STRING(result));
    }
    /*
    int main(int argc, char** argv){
        TPPInterpreter interpreter;
        interpreter.registerFunction(&EXTERN_print, "print");
        interpreter.registerFunction(&EXTERN_printV, "printV");
        interpreter.registerFunction(&EXTERN_input, "input");
        interpreter.registerFunction(&EXTERN_stringMerge, "stringMerge");
        std::ifstream t("test.tpp");
        std::stringstream buffer;
        buffer << t.rdbuf();
        interpreter.interpret(buffer.str());
        return 0;
    }
    */
}
