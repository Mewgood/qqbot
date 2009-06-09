#ifndef NIL_RANDOM_HPP
#define NIL_RANDOM_HPP

#include <vector>
#include <algorithm>

#include <nil/exception.hpp>
#include <nil/types.hpp>

namespace nil
{
	namespace random
	{
		ulong uint();
		ulong uint(ulong minimum, ulong maximum);

		void seed(std::string const & data);
		ulong advanced_uint();
		ulong advanced_uint(ulong minimum, ulong maximum);

		float sp_float();
		float sp_float(float minimum, float maximum);
		double dp_float();
		double dp_float(double minimum, double maximum);
	}

	template <class type>
		class random_scale
	{
	public:
		struct element
		{
			type object;

			ulong minimum;
			ulong maximum;

			element()
			{
			}

			element(type object, ulong minimum, ulong maximum):
				object(object),
				minimum(minimum),
				maximum(maximum)
			{
			}

			bool operator==(ulong input) const
			{
				return input >= minimum && input <= maximum;
			}
		};

		random_scale():
			total_weight(0)
		{
		}

		void add(type object, ulong weight)
		{
			if(weight == 0)
				throw exception("Element weight can't be zero");
			elements.push_back(element(object, total_weight, total_weight + weight - 1));
			total_weight += weight;
		}

		type random()
		{
			return make_choice()->object;
		}

	private:
		std::vector<element> elements;
		ulong total_weight;

		typename std::vector<element>::iterator make_choice()
		{
			if(total_weight == 0)
				throw exception("Random scale object doesn't contain any elements yet");
			ulong choice = random::uint(0, total_weight - 1);
			typename std::vector<element>::iterator iterator = std::find(elements.begin(), elements.end(), choice);
			if(iterator == elements.end())
				throw exception("Unable to match weight, code must be broken");
			return iterator;
		}
	};
}

#endif
