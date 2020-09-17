/*
** EPITECH PROJECT, 2019
** print_test
** File description:
** test the print function
*/

#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include "vm.h"
#include "object.h"
#include "my.h"

void redirect_all_std(void)
{
    cr_redirect_stdout ();
    cr_redirect_stderr ();
}

Test(init_vm, clear_free)
{
    vm_t vm;
    init_vm(&vm);
    vm.init_string = copy_string(&vm, "init", 4);
    cr_assert_not_null(vm.init_string);
    free_vm(&vm);
    cr_assert_null(vm.init_string);
}

Test(run_file, simple_print_number, .init = redirect_all_std)
{
    vm_t vm;

    init_vm(&vm);
    run_file(&vm, "tests/print/print_number.eno");
    fflush(stdout);
    cr_assert_stdout_eq_str("1\n");
    free_vm(&vm);
}

Test(run_file, simple_print_string, .init = redirect_all_std)
{
    vm_t vm;

    init_vm(&vm);
    run_file(&vm, "tests/print/print_string.eno");
    fflush(stdout);
    cr_assert_stdout_eq_str("Hello World\n");
    free_vm(&vm);
}

Test(run_file, simple_print_expression, .init = redirect_all_std)
{
    vm_t vm;

    init_vm(&vm);
    run_file(&vm, "tests/print/print_expression.eno");
    fflush(stdout);
    cr_assert_stdout_eq_str("9\n");
    free_vm(&vm);
}

Test(run_file, simple_print_string_concat, .init = redirect_all_std)
{
    vm_t vm;

    init_vm(&vm);
    run_file(&vm, "tests/print/print_string_concat.eno");
    fflush(stdout);
    cr_assert_stdout_eq_str("Hello World\n");
    free_vm(&vm);
}

Test(print, missing_argument, .init = redirect_all_std)
{
    vm_t vm;

    init_vm(&vm);
    interpret(&vm, "print;");
    fflush(stderr);
    cr_assert_stderr_eq_str("[line 1] Error at ';': Expect expression.\n");
    free_vm(&vm);
}