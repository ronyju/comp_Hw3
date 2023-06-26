//
// Created by Oren on 20/06/2023.
//

#ifndef INC_5_REGISTER_H
#define INC_5_REGISTER_H

#include <iostream>


class Register {
public:
    string registerValue;
    bool isString = false;
    int stringLength = 0;
    string GetRegName(int regNum){
        return "%t"+std::to_string(regNum);
    }
    Register():registerValue("REG_UNDEF"){}
    Register(string value):registerValue(value){};// example: gets %t0 sets %t0 or gets '5' sets '5'
    Register(int regNum){ // example: gets 0 returns %t0
        registerValue = GetRegName(regNum);
    };
    void SetRegister(int regNum) { registerValue = GetRegName(regNum); }
    void SetRegister(string value) { registerValue = value; }
    void SetIsString(bool is) { isString = is; }
    void SetStringLength (int length) { stringLength = length; }
    bool IsString () { return isString; }
    int GetStringLength() { return stringLength; }

    string GetRegister () {return registerValue; }
    ~Register(){};


};
#endif //INC_5_REGISTER_H