/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** EmptyLibraryList.hpp header
*/

#ifndef FOX_RUNTIME_EXCEPTION_HPP_
#define FOX_RUNTIME_EXCEPTION_HPP_

#include "AException.hpp"

class RuntimeException : public AException
{
public:
    RuntimeException(const std::string &) throw();
    virtual ~RuntimeException() throw();
};

#endif