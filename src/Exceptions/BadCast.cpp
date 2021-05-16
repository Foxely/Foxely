/*
** EPITECH PROJECT, 2021
** Arcade
** File description:
** EmptyLibraryList.cpp
*/

#include "Exceptions/BadCast.hpp"

BadCast::BadCast(const std::string& strMsg) throw()
	: RuntimeException(strMsg) { }

BadCast::~BadCast() throw() { }