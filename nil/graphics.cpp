#include <nil/graphics.hpp>

#include <nil/string.hpp>

#include <nil/thread.hpp>

#include <nil/debug.hpp>

namespace nil
{
	coordinate::coordinate():
		x(0),
		y(0)
	{
	}

	coordinate::coordinate(long x, long y):
		x(x),
		y(y)
	{
	}

	void coordinate::swap()
	{
		std::swap(x, y);
	}

	coordinate & coordinate::operator+=(coordinate & other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	coordinate & coordinate::operator-=(coordinate & other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	coordinate & coordinate::operator*=(long factor)
	{
		x *= factor;
		y *= factor;
		return *this;
	}

	coordinate & coordinate::operator+=(long summand)
	{
		x += summand;
		y += summand;
		return *this;
	}

	coordinate & coordinate::operator-=(long subtrahend)
	{
		x -= subtrahend;
		y -= subtrahend;
		return *this;
	}

	coordinate coordinate::operator+(coordinate & other) const
	{
		coordinate output(*this);
		output += other;
		return output;
	}

	coordinate coordinate::operator-(coordinate & other) const
	{
		coordinate output(*this);
		output -= other;
		return output;
	}

	coordinate coordinate::operator*(long factor) const
	{
		coordinate output(*this);
		output *= factor;
		return output;
	}

	coordinate coordinate::operator+(long summand) const
	{
		coordinate output(*this);
		output += summand;
		return output;
	}

	coordinate coordinate::operator-(long subtrahend) const
	{
		coordinate output(*this);
		output -= subtrahend;
		return output;
	}

	std::string coordinate::string() const
	{
		return "(" + string::number_to_string<long>(x) + ", " + string::number_to_string<long>(y) + ")";
	}

	hit_region::hit_region(coordinate offset, dimensions size):
		offset(offset),
		size(size)
	{
	}

	bool hit_region::match(coordinate mouse)
	{
		return
			(mouse.x >= offset.x) &&
			(mouse.x <= offset.x + size.x) &&
			(mouse.y >= offset.y) &&
			(mouse.y <= offset.y + size.y)
		;
	}

#ifdef NIL_WINDOWS
	void draw_line(::HDC buffer_dc, coordinate begin, coordinate end)
	{
		DEBUG_MARK
		::POINT line_points[2];
		line_points[0].x = static_cast< ::LONG>(begin.x);
		line_points[0].y = static_cast< ::LONG>(begin.y);
		line_points[1].x = static_cast< ::LONG>(end.x);
		line_points[1].y = static_cast< ::LONG>(end.y);
		::Polyline(buffer_dc, line_points, static_cast<int>(nil::countof(line_points)));
		DEBUG_MARK
	}

	void draw_rectangle(::HDC buffer_dc, coordinate offset, dimensions size)
	{
		DEBUG_MARK
		::POINT line_points[5];
		line_points[0].x = static_cast< ::LONG>(offset.x);
		line_points[0].y = static_cast< ::LONG>(offset.y);
		line_points[1].x = static_cast< ::LONG>(offset.x + size.x);
		line_points[1].y = static_cast< ::LONG>(offset.y);
		line_points[2].x = static_cast< ::LONG>(offset.x + size.x);
		line_points[2].y = static_cast< ::LONG>(offset.y + size.y);
		line_points[3].x = static_cast< ::LONG>(offset.x);
		line_points[3].y = static_cast< ::LONG>(offset.y + size.y);
		line_points[4] = line_points[0];
		//::Polyline(buffer_dc, line_points, static_cast<int>(nil::countof(line_points)));
		for ( size_t i = 0; i < 500/15; i++ ) {
			if ( ::Polyline(buffer_dc, line_points, static_cast<int>(nil::countof(line_points))) != 0 ) { SetLastError(0); break; }
			nil::sleep(15);
		}
		DEBUG_MARK
	}

	void fill_rectangle(::HDC buffer_dc, coordinate offset, dimensions size, ::HBRUSH brush)
	{
		DEBUG_MARK
		::SelectObject( buffer_dc, brush );
		DEBUG_MARK
		::RECT rectangle;
		rectangle.left = offset.x;
		rectangle.top = offset.y;
		rectangle.right = offset.x + size.x;
		rectangle.bottom = offset.y + size.y;
		for ( size_t i = 0; i < 500/15; i++ ) {
			if ( ::FillRect(buffer_dc, &rectangle, brush) != 0 ) { SetLastError(0); break; }
			nil::sleep(15);
		}
		DEBUG_MARK
	}

	void draw_rectangle(::HDC buffer_dc, coordinate offset, dimensions size, ::HBRUSH brush)
	{
		DEBUG_MARK
		fill_rectangle(buffer_dc, offset, size, brush);
		draw_rectangle(buffer_dc, offset, size);
	}

	void draw_string(::HDC buffer_dc, std::string const & text, coordinate offset)
	{
		::TextOut(buffer_dc, offset.x, offset.y, text.c_str(), static_cast<int>(text.length()));
		DEBUG_MARK
	}
#endif
}
