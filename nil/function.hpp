#ifndef NIL_FUNCTION_HPP
#define NIL_FUNCTION_HPP

#include <nil/exception.hpp>

namespace nil
{

#define FUNCTION_CLASS nullary_function
#define MEMBER_FUNCTION_CLASS nullary_member_function
#define TEMPLATE_ARGUMENTS
#define TEMPLATE_ARGUMENT_TYPES
#define ARGUMENT_TYPES
#define ARGUMENT_TYPES_WITH_NAMES
#define ARGUMENT_TYPES_WITH_NAMES2
#define ARGUMENT_NAMES
#define ARGUMENT_NAMES2

#include <nil/function_generator.hpp>

#define FUNCTION_CLASS unary_function
#define MEMBER_FUNCTION_CLASS unary_member_function
#define TEMPLATE_ARGUMENTS , class argument1_type
#define TEMPLATE_ARGUMENT_TYPES , argument1_type
#define ARGUMENT_TYPES argument1_type
#define ARGUMENT_TYPES_WITH_NAMES argument1_type argument1
#define ARGUMENT_TYPES_WITH_NAMES2 , argument1_type argument1
#define ARGUMENT_NAMES argument1
#define ARGUMENT_NAMES2 , argument1

#include <nil/function_generator.hpp>

#define FUNCTION_CLASS binary_function
#define MEMBER_FUNCTION_CLASS binary_member_function
#define TEMPLATE_ARGUMENTS , class argument1_type, class argument2_type
#define TEMPLATE_ARGUMENT_TYPES , argument1_type, argument2_type
#define ARGUMENT_TYPES argument1_type, argument2_type
#define ARGUMENT_TYPES_WITH_NAMES argument1_type argument1, argument2_type argument2
#define ARGUMENT_TYPES_WITH_NAMES2 , argument1_type argument1, argument2_type argument2
#define ARGUMENT_NAMES argument1, argument2
#define ARGUMENT_NAMES2 , argument1, argument2

#include <nil/function_generator.hpp>

#define FUNCTION_CLASS ternary_function
#define MEMBER_FUNCTION_CLASS ternary_member_function
#define TEMPLATE_ARGUMENTS , class argument1_type, class argument2_type, class argument3_type
#define TEMPLATE_ARGUMENT_TYPES , argument1_type, argument2_type, argument3_type
#define ARGUMENT_TYPES argument1_type, argument2_type, argument3_type
#define ARGUMENT_TYPES_WITH_NAMES argument1_type argument1, argument2_type argument2, argument3_type argument3
#define ARGUMENT_TYPES_WITH_NAMES2 , argument1_type argument1, argument2_type argument2, argument3_type argument3
#define ARGUMENT_NAMES argument1, argument2, argument3
#define ARGUMENT_NAMES2 , argument1, argument2, argument3

#include <nil/function_generator.hpp>

}

#endif
