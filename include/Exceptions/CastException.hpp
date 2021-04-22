/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** CastException.hpp header
*/

#ifndef NTS_EMPTY_CIRCUIT_EXCEPTION_HPP_
#define NTS_EMPTY_CIRCUIT_EXCEPTION_HPP_

#include "AException.hpp"

class CastException : public AException
{
public:
    CastException(const std::string&) throw();
    virtual ~CastException() throw();
};

#endif