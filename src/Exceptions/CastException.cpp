/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** CastException.cpp
*/

#include "Exceptions/CastException.hpp"

CastException::CastException(const std::string& strMsg) throw()
	: AException(strMsg) { }
CastException::~CastException() throw() { }