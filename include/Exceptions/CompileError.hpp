/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** InitException.hpp header
*/

#ifndef FOX_COMPILER_EXCEPTION_HPP
#define FOX_COMPILER_EXCEPTION_HPP

#include "AException.hpp"

class CompileError : public AException
{
public:
    CompileError(const std::string &) throw();
    virtual ~CompileError() throw();
};

#endif