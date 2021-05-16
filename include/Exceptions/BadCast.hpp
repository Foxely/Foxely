/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** EmptyLibraryList.hpp header
*/

#ifndef FOX_BAD_CAST_EXCEPTION_HPP_
#define FOX_BAD_CAST_EXCEPTION_HPP_

#include "RuntimeException.hpp"

class BadCast : public RuntimeException
{
public:
    BadCast(const std::string &) throw();
    virtual ~BadCast() throw();
};

#endif