/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** InitException.hpp header
*/

#ifndef INIT_EXCEPTION_HPP
#define INIT_EXCEPTION_HPP

#include "AException.hpp"

class InitException : public AException
{
public:
    InitException(const std::string &) throw();
    virtual ~InitException() throw();
};

#endif