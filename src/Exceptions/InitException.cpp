/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** InitException.cpp
*/

#include "Exceptions/InitException.hpp"

InitException::InitException(const std::string& strMsg) throw()
	: AException(strMsg) { }

InitException::~InitException() throw() { }