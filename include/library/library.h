#ifndef FOX_IOPlugin_H
#define FOX_IOPlugin_H

class VM;

void DefineIOModule(VM* oVM);
void DefineMathModule(VM* oVM);
void DefineModuleModule(VM* oVM);
void DefineOSModule(VM* oVM);


void DefineCoreArray(VM* oVM);

#endif
