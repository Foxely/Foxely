
#pragma once

class Value;

Value whichNative(int argCount, Value* args);
Value shellNative(int argCount, Value* args);
Value getEnvNative(int argCount, Value* args);