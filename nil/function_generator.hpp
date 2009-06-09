#ifndef FUNCTION_GENERATOR_HPP
#define FUNCTION_GENERATOR_HPP

class base_member_function_class
{
public:
	virtual base_member_function_class * clone() = 0;
};

#endif

template <class class_type, class return_type TEMPLATE_ARGUMENTS>
class MEMBER_FUNCTION_CLASS: public base_member_function_class
{
	typedef return_type (class_type::*pointer_type)(ARGUMENT_TYPES);

public:
	MEMBER_FUNCTION_CLASS(class_type & object, pointer_type pointer):
		object(object),
		pointer(pointer)
	{
	}

	MEMBER_FUNCTION_CLASS * clone()
	{
		return new MEMBER_FUNCTION_CLASS(*this);
	}

	return_type operator()(ARGUMENT_TYPES_WITH_NAMES)
	{
		return (object.*pointer)(ARGUMENT_NAMES);
	}

private:
	class_type & object;
	pointer_type pointer;
};

template <class return_type TEMPLATE_ARGUMENTS>
class FUNCTION_CLASS
{
	typedef return_type (*free_function_pointer_type)(ARGUMENT_TYPES);

public:
	FUNCTION_CLASS():
		free_function_pointer(0),
		type_information_pointer(0)
	{
	}

	FUNCTION_CLASS(FUNCTION_CLASS const & other):
		free_function_pointer(other.free_function_pointer),
		store_types_pointer(other.store_types_pointer),
		delete_type_information_pointer(other.delete_type_information_pointer),
		type_information_pointer(other.type_information_pointer)
	{
		if(type_information_pointer != 0)
		{
			type_information_pointer = reinterpret_cast<base_member_function_class *>(type_information_pointer)->clone();
		}
	}

	FUNCTION_CLASS(free_function_pointer_type free_function_pointer):
		free_function_pointer(free_function_pointer),
		type_information_pointer(0)
	{
	}

	template <class class_type>
	FUNCTION_CLASS(class_type & object, return_type (class_type::*pointer)(ARGUMENT_TYPES)):
		free_function_pointer(0),
		store_types_pointer(&store_types<class_type>),
		delete_type_information_pointer(&delete_type_information<class_type>),
		type_information_pointer(new MEMBER_FUNCTION_CLASS<class_type, return_type TEMPLATE_ARGUMENT_TYPES>(object, pointer))
	{
	}

	~FUNCTION_CLASS()
	{
		if(type_information_pointer != 0)
		{
			delete_type_information_pointer(type_information_pointer);
		}
	}

	bool is_initialised()
	{
		return free_function_pointer || type_information_pointer;
	}

	void call(ARGUMENT_TYPES_WITH_NAMES)
	{
		if(free_function_pointer != 0)
		{
			free_function_pointer(ARGUMENT_NAMES);
		}
		else
		{
			if(type_information_pointer != 0)
			{
				store_types_pointer(type_information_pointer ARGUMENT_NAMES2);
			}
		}
	}

	return_type operator()(ARGUMENT_TYPES_WITH_NAMES)
	{
		if(free_function_pointer != 0)
		{
			return free_function_pointer(ARGUMENT_NAMES);
		}
		else
		{
			if(type_information_pointer != 0)
			{
				return store_types_pointer(type_information_pointer ARGUMENT_NAMES2);
			}
		}
		throw exception("Called an unitialised function object");
	}

	FUNCTION_CLASS & operator=(FUNCTION_CLASS const & other)
	{
		free_function_pointer = other.free_function_pointer;
		store_types_pointer = other.store_types_pointer;
		delete_type_information_pointer = other.delete_type_information_pointer;
		type_information_pointer = other.type_information_pointer;
		if(type_information_pointer != 0)
		{
			type_information_pointer = reinterpret_cast<base_member_function_class *>(type_information_pointer)->clone();
		}
		return *this;
	}

private:
	free_function_pointer_type free_function_pointer;

	return_type (*store_types_pointer)(void * ARGUMENT_TYPES_WITH_NAMES2);
	void (*delete_type_information_pointer)(void *);
	void * type_information_pointer;
	
	template <class class_type>
	static return_type store_types(void * type_information ARGUMENT_TYPES_WITH_NAMES2)
	{
		typedef MEMBER_FUNCTION_CLASS<class_type, return_type TEMPLATE_ARGUMENT_TYPES> function_type;
		function_type & function = *reinterpret_cast<function_type *>(type_information);
		return function(ARGUMENT_NAMES);
	}

	template <class class_type>
	static void delete_type_information(void * type_information)
	{
		typedef MEMBER_FUNCTION_CLASS<class_type, return_type TEMPLATE_ARGUMENT_TYPES> function_type;
		function_type * function = reinterpret_cast<function_type *>(type_information);
		delete function;
	}
};

#undef FUNCTION_CLASS
#undef MEMBER_FUNCTION_CLASS
#undef TEMPLATE_ARGUMENTS
#undef TEMPLATE_ARGUMENT_TYPES
#undef ARGUMENT_TYPES
#undef ARGUMENT_TYPES_WITH_NAMES
#undef ARGUMENT_TYPES_WITH_NAMES2
#undef ARGUMENT_NAMES
#undef ARGUMENT_NAMES2
