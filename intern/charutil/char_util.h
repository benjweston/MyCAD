#pragma once

const wchar_t* toWchar(const char *str);                //convert char* to wchar_t*
const wchar_t* toWchar(float number, int precision=-1); //convert float to wchar_t*
const wchar_t* toWchar(double number, int precision=-1);//convert double float to wchar_t*
const wchar_t* toWchar(int number);                     //convert integer to wchar_t*
const wchar_t* toWchar(long number);                    //convert long integer to wchar_t*

const char* toChar(const wchar_t *str);                 //convert wchar_t* to char*
const char* toChar(float number, int precision=-1);     //convert float to char*
const char* toChar(double number, int precision=-1);    //convert double to char*
const char* toChar(int number);                         //convert integer to char*
const char* toChar(long number);                        //convert long integer to char*

bool parseFloat(const char *in, float& res);
bool parseDouble(const char *in, double& res);

const int toInt(const char *str);
const float toFloat(const char *str);
const double toDouble(const char *str);
