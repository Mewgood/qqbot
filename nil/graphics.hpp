#ifndef NIL_GRAPHICS_HPP
#define NIL_GRAPHICS_HPP

#include <algorithm>
#include <string>

#include <nil/windows.hpp>
#include <nil/array.hpp>

namespace nil
{
	struct coordinate
	{
		long x;
		long y;

		coordinate();
		coordinate(long x, long y);
		void swap();

		coordinate & operator+=(coordinate & other);
		coordinate & operator-=(coordinate & other);
		coordinate & operator*=(long factor);

		coordinate & operator+=(long summand);
		coordinate & operator-=(long subtrahend);

		coordinate operator+(coordinate & other) const;
		coordinate operator-(coordinate & other) const;
		coordinate operator*(long factor) const;

		coordinate operator+(long summand) const;
		coordinate operator-(long subtrahend) const;

		std::string string() const;
	};

	typedef coordinate dimensions;

	class hit_region
	{
	public:
		hit_region(coordinate offset, dimensions size);
		bool match(coordinate mouse);

	private:
		coordinate offset;
		dimensions size;
	};

#ifdef NIL_WINDOWS
	void draw_line(::HDC buffer_dc, coordinate begin, coordinate end);
	void draw_rectangle(::HDC buffer_dc, coordinate offset, dimensions size);
	void fill_rectangle(::HDC buffer_dc, coordinate offset, dimensions size, ::HBRUSH brush);
	void draw_rectangle(::HDC buffer_dc, coordinate offset, dimensions size, ::HBRUSH brush);
	void draw_string(::HDC buffer_dc, std::string const & text, coordinate offset);
#endif
}

#endif
