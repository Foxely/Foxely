/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** InitException.cpp
*/

#include "Exceptions/CompileError.hpp"

CompileError::CompileError(const std::string& strMsg) throw()
	: AException(strMsg) { }

CompileError::~CompileError() throw() { }