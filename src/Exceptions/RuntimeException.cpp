/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** EmptyLibraryList.cpp
*/

#include "Exceptions/RuntimeException.hpp"

RuntimeException::RuntimeException(const std::string& strMsg) throw()
	: AException(strMsg) { }

RuntimeException::~RuntimeException() throw() { }