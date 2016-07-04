#ifndef GRAMMAR_H
#define GRAMMAR_H

//        "<program> ::= program ~ var <var_list_1> begin  <~1> <operators_list_1> end !" +
//        "<var_list_1> ::= <var_list> !" +
//        "<var_list> ::= <var> ~ !" +
//        "<var_list> ::= <var_list> <var> ~ !" +
//        "<var> ::= <id_list_1> : <type> !" +
//        "<id_list_1> ::= <id_list> !" +
//        "<id_list> ::= id !" +
//        "<id_list> ::= <id_list> , id !" +
//        "<type> ::= float !" +
//        "<operators_list_1> ::= <operators_list> !" +
//        "<operators_list> ::= <operator> ~ !" +
//        "<operators_list> ::= <operators_list> <operator> ~ !" +
//        "<operator> ::= <unmarked_operator> !" +
//        "<operator> ::= label : <unmarked_operator> !" +
//        "<operator> ::= label : !" +
//        "<~1> ::= ~ !" +
//        "<unmarked_operator> ::= id = <expression_1> !" +
//        "<unmarked_operator> ::= read ( <id_list_1> ) !" +
//        "<unmarked_operator> ::= write ( <id_list_1> ) !" +
//        "<unmarked_operator> ::= if <logical_expression_1> then goto label !" +
//        "<unmarked_operator> ::= do id = <expression_1> to <expression_2> <~1> <operators_list_1> next !" +
//        "<expression_1> ::= <expression> !" +
//        "<expression_2> ::= <expression_1> !" +
//        "<expression> ::= <term_1> !" +
//        "<expression> ::= <expression> + <term_1> !" +
//        "<expression> ::= <expression> - <term_1> !" +
//        "<expression> ::= - <term_1> !" +
//        "<term_1> ::= <term> !" +
//        "<term> ::= <factor_1> !" +
//        "<term> ::= <term> * <factor_1> !" +
//        "<term> ::= <term> / <factor_1> !" +
//        "<factor_1> ::= <factor> !" +
//        "<factor> ::= const !" +
//        "<factor> ::= id !" +
//        "<factor> ::= ( <expression_1> ) !" +
//        "<logical_expression_1> ::= <logical_expression> !" +
//        "<logical_expression> ::= <logical_term_1> !" +
//        "<logical_expression> ::= <logical_expression> or <logical_term_1> !" +
//        "<logical_term_1> ::= <logical_term> !" +
//        "<logical_term> ::= <logical_factor_1> !" +
//        "<logical_term> ::= <logical_term> and <logical_factor_1> !" +
//        "<logical_factor_1> ::= <logical_factor> !" +
//        "<logical_factor> ::= <expression_1> <sign> <expression_1> !" +
//        "<logical_factor> ::= [ <logical_expression_1> ] !" +
//        "<logical_factor> ::= not <logical_factor> !" +
//        "<sign> ::= < !" +
//        "<sign> ::= > !" +
//        "<sign> ::= <= !" +
//        "<sign> ::= >= !" +
//        "<sign> ::= == !" +
//        "<sign> ::= != !";


#endif // GRAMMAR_H
