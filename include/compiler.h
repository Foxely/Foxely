#ifndef fox_compiler_h
#define fox_compiler_h

// #include "vm.h"

// bool compile(const char* source, Chunk& chunk);

// static inline void
// variable(parser_t *parser, scanner_t *scanne, bool can_assign) {
//     named_variable(parser, scanne, parser->previous, can_assign);
// }

// static inline void string(parser_t *parser, scanner_t *scanne, bool can_assign)
// {
//     (void) can_assign;
//     (void) scanne;
//     emit_constant(parser,
//             OBJ_VAL(copy_string(parser->vm, parser->previous.start + 1,
//                                     parser->previous.length - 2)));
// }

// static inline
// void call_compiler(parser_t *parser, scanner_t *scan, bool can_assign) {
//     (void) can_assign;
//     uint8_t arg_count = argument_list(parser, scan);
//     emit_bytes(parser, OP_CALL, arg_count);
// }

#endif