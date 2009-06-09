#ifndef NIL_MATH_HPP
#define NIL_MATH_HPP

#include <string>
#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <sstream>
#include <iostream>

#include <nil/exception.hpp>
#include <nil/string.hpp>

namespace nil
{
	extern float const pi;

	int round(double input);
	int absolute(int input);

	float positive_sin(float x);
	float positive_cos(float x);

	unsigned gcd(unsigned left, unsigned right);

	class rational_number
	{
	public:
		rational_number();
		rational_number(int integer);
		rational_number(int numerator, int denominator);
		
		rational_number & operator+=(rational_number const & other);
		rational_number & operator-=(rational_number const & other);
		rational_number & operator*=(rational_number const & other);
		rational_number & operator/=(rational_number const & other);
		
		rational_number operator+(rational_number const & other) const;
		rational_number operator-(rational_number const & other) const;
		rational_number operator*(rational_number const & other) const;
		rational_number operator/(rational_number const & other) const;
		rational_number operator-() const;

		bool operator<(rational_number const & other) const;
		bool operator>(rational_number const & other) const;
		bool operator==(rational_number const & other) const;
		bool operator!=(rational_number const & other) const;
		bool operator<=(rational_number const & other) const;
		bool operator>=(rational_number const & other) const;

		bool negative() const;
		rational_number square_root() const;
		double approximation() const;

		std::string string() const;
		
	private:
		int numerator;
		int denominator;

		void euclid();
	};

	template <typename number_type>
		std::string get_number_string(number_type input)
	{
		return nil::string::number_to_string<number_type>(input);
	}

	

	template <typename number_type>
		class base_matrix
	{
	public:
		base_matrix<number_type>():
			rows(0),
			columns(0)
		{
		}

		base_matrix(std::size_t rows, std::size_t columns)
		{
			initialise(rows, columns);
		}

		void initialise(std::size_t new_rows, std::size_t new_columns)
		{
			rows = new_rows;
			columns = new_columns;
			elements = std::vector<number_type>(rows * columns);
		}

		
		void from_array(int * pointer, std::size_t count)
		{
			std::size_t size = static_cast<std::size_t>(std::sqrt(static_cast<float>(count)));
			columns = size;
			rows = size;
			elements.clear();
			for(std::size_t i = 0; i < count; i++)
				elements.push_back(pointer[i]);
		}
		
		void from_array(number_type * pointer)
		{
			for(std::size_t row = 1; row <= rows; row++)
			{
				for(std::size_t column = 1; column <= columns; column++)
				{
					set(row, column, pointer[(row - 1) * columns + (column - 1)]);
				}
			}
		}
		
		void one()
		{
			for(std::size_t i = 1; i <= columns && i <= rows; i++)
				set(i, i, 1);
		}
		
		void multiply(number_type factor)
		{
			for(typename std::vector<number_type>::iterator i = elements.begin(), end = elements.end(); i != end; i++)
				*i *= factor;
		}

		void multiply(std::size_t row, number_type factor)
		{
			if(factor == 0)
				throw exception("Illegal multiplication operation");
			for(std::size_t column = 1; column <= columns; ++column)
				get_reference(row, column) *= factor;
		}

		void add(number_type factor, std::size_t source_row, std::size_t destination_row)
		{
			if(source_row == destination_row)
				throw exception("Illegal operation on a base_matrix");
			for(std::size_t column = 1; column <= columns; column++)
				elements[get_offset(destination_row, column)] += factor * get(source_row, column);
		}

		
		void swap(std::size_t row1, std::size_t row2)
		{
			for(std::size_t column = 1; column <= columns; column++)
				std::swap(get_reference(row1, column), get_reference(row2, column));
		}

		
		number_type get(std::size_t row, std::size_t column) const
		{
			return elements[get_offset(row, column)];
		}

		
		void set(std::size_t row, std::size_t column, number_type value)
		{
			get_reference(row, column) = value;
		}

		
		void lu_decomposition(base_matrix & swap_matrix, int & determinant_swap_factor, base_matrix & lower, base_matrix & upper) const
		{
			if(rows != columns)
				throw exception("Matrix isn't square");

			determinant_swap_factor = 1;

			swap_matrix.initialise(rows, columns);
			swap_matrix.one();
			lower.initialise(rows, columns);
			lower.one();
			upper = *this;
			for(std::size_t row = 1; row <= rows; row++)
			{
				std::size_t non_zero_row;
				if(!find_non_zero_element(row, non_zero_row))
					continue;
				if((row - non_zero_row) % 2)
					determinant_swap_factor = - determinant_swap_factor;
				swap_matrix.swap(row, non_zero_row);
				upper.swap(row, non_zero_row);
				number_type pivot = upper.get(row, row);
				for(std::size_t i = row + 1; i <= rows; i++)
				{
					number_type target = upper.get(i, row);
					number_type factor = - target / pivot;
					upper.add(factor, row, i);
					lower.set(i, row, - factor);
					/*
					std::cout << "Target (" << i << ", " << row << "): " << target.string() << std::endl;
					std::cout << "target / pivot = " << target << " / " << pivot << std::endl;
					std::cout << upper.string() << std::endl;
					std::cout << lower.string() << std::endl;
					std::cin.get();
					*/
				}
			}
		}

		number_type determinant() const
		{
			number_type output = 1;
			base_matrix swap_matrix, lower, upper;
			int determinant_swap_factor;
			lu_decomposition(swap_matrix, determinant_swap_factor, lower, upper);
			output *= determinant_swap_factor;
			for(std::size_t i = 1; i <= rows; i++)
				output *= lower.get(i, i) * upper.get(i, i);
			return output;
		}
		
		base_matrix<number_type> solve(base_matrix<number_type> input) const
		{
			base_matrix<number_type> swap_matrix, lower, upper;
			int determinant_swap_factor;
			lu_decomposition(swap_matrix, determinant_swap_factor, lower, upper);

			input = swap_matrix * input;

			//LUx = input
			//y = Ux

			//Solve Ly = input

			base_matrix y(rows, 1);
			for(std::size_t row = 1; row <= rows; row++)
			{
				number_type y_value = input.get(row, 1);
				for(std::size_t column = 1; column < row; column++)
					y_value -= lower.get(row, column) * y.get(column, 1);
				y_value /= lower.get(row, row);
				y.set(row, 1, y_value);
			}

			//Solve Ux = y

			base_matrix x(rows, 1);
			for(std::size_t row = rows; row >= 1; row--)
			{
				number_type x_value = y.get(row, 1);
				for(std::size_t column = columns; column > row; column--)
					x_value -= upper.get(row, column) * x.get(column, 1);
				x_value /= upper.get(row, row);
				x.set(row, 1, x_value);
			}

			return x;
		}

	#define CHOLESKY_DEBUG 1
		
		void cholesky_decomposition(base_matrix & left, base_matrix & right) const
		{
			if(rows != columns)
				throw exception("Matrix isn't square");
			left.initialise(rows, columns);
			for(std::size_t row = 1; row <= rows; row++)
			{
				if(CHOLESKY_DEBUG)
					std::cout << "k = " << row << std::endl;

				number_type sum = get(row, row);
				for(std::size_t column = 1; column < row; column++)
				{
					number_type number = left.get(row, column);
					sum -= number * number;
				}
				left.set(row, row, sum.square_root());
				if(CHOLESKY_DEBUG)
					std::cout << left.string() << std::endl;
				for(std::size_t lower_row = row + 1; lower_row <= rows; lower_row++)
				{
					if(CHOLESKY_DEBUG)
						std::cout << "i = " << lower_row << std::endl;
					sum = get(lower_row, row);
					for(std::size_t left_column = 1; left_column < row; left_column++)
						sum -= left.get(lower_row, left_column) * left.get(row, left_column);
					left.set(lower_row, row, sum / left.get(row, row));
				}
				if(CHOLESKY_DEBUG)
					std::cout << left.string() << std::endl;
			}
			right = left.transpose();
		}
		
		base_matrix<number_type> transpose() const
		{
			base_matrix output(columns, rows);
			for(std::size_t row = 1; row <= rows; row++)
				for(std::size_t column = 1; column <= columns; column++)
					output.set(column, row, get(row, column));
			return output;
		}

		base_matrix<number_type> gradient_descent(base_matrix<number_type> const & target, base_matrix<number_type> const & start, unsigned steps) const
		{
			if(!is_square() || !target.same_size(start) || !target.is_vector() || target.columns == columns)
				throw exception("Invalid base_matrix sizes");

			base_matrix const & A = *this;
			base_matrix u = start;
			base_matrix r(target.rows, target.columns);
			r = target - A * u;
			for(unsigned i = 1; i <= steps; i++)
			{
				base_matrix a = A * r;
				/*
				number_type alpha = 0;
				for(std::size_t row = 1; row <= r.rows; row++)
				{
					number_type value = r.get(row, 1);
					alpha += value * value;
				}
				*/
				number_type alpha = r.square_sum();
				alpha /= a.scalar_product(r);
				u = u + alpha * r;
				r = r - alpha * a;

				std::cout << "alpha_" << i << ":" << std::endl << alpha.string() << std::endl;
				std::cout << "u_" << i << ":" << std::endl << u.string() << std::endl;
				std::cout << "r_" << i << ":" << std::endl << r.string() << std::endl;
				std::cout << "u approximation: ";
				for(std::size_t row = 1; row <= rows; row++)
					std::cout << u.get(row, 1).approximation() << " ";
				std::cout << std::endl;
			}

			return u;
		}

		number_type get_error(base_matrix const & guess, base_matrix const & target) const
		{
			number_type sum = 0;
			base_matrix result = target - *this * guess;
			for(std::size_t row = 1; row < result.rows; row++)
			{
				number_type value = result.get(row, 1);
				sum += value * value;
			}
			return sum;
		}

		base_matrix<number_type> conjugated_gradient_descent(base_matrix<number_type> const & target, base_matrix<number_type> const & start, unsigned steps, bool error_check) const
		{
			/*
			||Ax - b||_2 / initialError <= 1 / 100
			<=> sqrt(sum(i = 1 to n: ((Ax - b)_i)^2)) / initialError <= 1 / 100
			<=> sum(i = 1 to n: ((Ax - b)_i)^2) / initialError^2 <= 1 / 10000
			<=> 10000 * sum(i = 1 to n: ((Ax - b)_i)^2) <= initialError^2
			*/

			if(!is_square() || !target.same_size(start) || !target.is_vector() || target.columns == columns)
				throw exception("Invalid base_matrix sizes");

			base_matrix const & A = *this;
			number_type initial_error = get_error(start, target);

			std::cout << "Initial error^2: " << initial_error.string() << std::endl;

			base_matrix r = target - A * start;
			base_matrix p = r;
			base_matrix x = start;

			for(unsigned i = 1; i <= steps || steps == 0; i++)
			{
				base_matrix a = A * p;

				if(!error_check)
					std::cout << "a_" << i << ":" << std::endl << a.string() << std::endl;

				number_type alpha = r.square_sum() / p.scalar_product(a);

				if(!error_check)
					std::cout << "alpha_" << i << ":" << std::endl << alpha.string() << std::endl << std::endl;

				x = x + alpha * p;

				if(!error_check)
				{
					std::cout << "x_" << i << ":" << std::endl << x.string() << " (";
					bool first = true;
					for(std::size_t row = 1; row <= rows; row++)
					{
						if(first)
							first = false;
						else
							std::cout << " ";
						std::cout << x.get(row).approximation();
					}
					std::cout << ")" << std::endl;
				}

				if(error_check)
				{
					number_type error = get_error(x, target);
					std::cout << i << ": Error^2: " << error.string() << std::endl;
					if(error * 10000 <= initial_error)
					{
						std::cout << i << ": Error has been reduced by the factor of 100" << std::endl;
						break;
					}
				}

				base_matrix old_r = r;
				r = r - alpha * A * p;

				if(!error_check)
					std::cout << "r_" << i << ":" << std::endl << r.string() << std::endl;

				bool equals_zero = true;
				for(std::size_t row = 1; row <= rows; row++)
				{
					if(r.get(row) != 0)
					{
						equals_zero = false;
						break;
					}
				}

				if(equals_zero)
				{
					std::cout << "r_" << i << " = 0, STOP" << std::endl << std::endl;
					break;
				}

				number_type beta = r.square_sum() / old_r.square_sum();

				if(!error_check)
					std::cout << "beta_" << i << ":" << std::endl << beta.string() << std::endl << std::endl;

				p = r + beta * p;

				if(!error_check)
					std::cout << "p_" << i << ":" << std::endl << p.string() << std::endl;
			}

			return x;
		}

		number_type scalar_product(base_matrix const & other) const
		{
			if(!is_vector() || !same_size(other))
				throw nil::exception("Invalid base_matrix sizes");
			number_type sum = 0;
			for(std::size_t row = 1; row <= rows; row++)
				sum += get(row, 1) * other.get(row, 1);
			return sum;
		}

		number_type square_sum() const
		{
			number_type sum = 0;
			for(std::size_t row = 1; row <= rows; row++)
			{
				number_type value = get(row, 1);
				sum += value * value;
			}
			return sum;
		}

		bool find_non_zero_element(std::size_t column, std::size_t & row) const
		{
			for(std::size_t i = column; i <= rows; i++)
			{
				if(get(i, column) != 0)
				{
					row = i;
					return true;
				}
			}
			return false;
		}


		bool same_size(base_matrix const & other) const
		{
			return rows == other.rows && columns == other.columns;
		}

		bool is_vector() const
		{
			return columns == 1;
		}

		bool is_square() const
		{
			return rows == columns;
		}

		std::string string() const
		{
			std::vector<std::string> strings;
			for(typename std::vector<number_type>::const_iterator i = elements.begin(), end = elements.end(); i != end; i++)
				strings.push_back(get_number_string<number_type>(*i));
			std::vector<std::size_t> column_widths;
			for(std::size_t column = 1; column <= columns; column++)
			{
				std::size_t maximum = 0;
				for(std::size_t row = 1; row <= rows; row++)
					maximum = std::max(maximum, strings[get_offset(row, column)].length());
				column_widths.push_back(maximum);
			}
			std::stringstream output;
			for(std::size_t row = 1; row <= rows; row++)
			{
				for(std::size_t column = 1; column <= columns; column++)
				{
					std::size_t length = column_widths[column - 1] + 1;
					std::string string = strings[get_offset(row, column)];
					string = std::string(length - string.length(), ' ') + string;
					output << string;
				}
				output << "\n";
			}
			return output.str();
		}

		std::size_t get_rows() const
		{
			return rows;
		}

		std::size_t get_columns() const
		{
			return columns;
		}

		base_matrix<number_type> operator+(base_matrix<number_type> const & other) const
		{
			base_matrix output(*this);
			output += other;
			return output;
		}

		base_matrix<number_type> operator-(base_matrix<number_type> const & other) const
		{
			base_matrix output(*this);
			output -= other;
			return output;
		}

		base_matrix<number_type> operator*(base_matrix<number_type> const & other) const
		{
			if(columns != other.rows)
				throw nil::exception("Invalid base_matrix multiplication");
			base_matrix output(rows, other.columns);
			for(std::size_t row = 1; row <= rows; row++)
			{
				for(std::size_t column = 1; column <= other.columns; column++)
				{
					number_type sum = 0;
					for(std::size_t i = 1; i <= columns; i++)
						sum += get(row, i) * other.get(i, column);
					output.set(row, column, sum);
				}
			}
			return output;
		}

		base_matrix<number_type> operator*(number_type const & other) const
		{
			base_matrix<number_type> output(*this);
			output *= other;
			return output;
		}

		
			base_matrix<number_type> operator/(number_type const & other) const
		{
			base_matrix<number_type> output(*this);
			output /= other;
			return output;
		}

		base_matrix<number_type> & operator+=(base_matrix<number_type> const & other)
		{
			if(!same_size(other))
				throw nil::exception("Invalid base_matrix sizes");
			
			for(std::size_t row = 1; row <= rows; row++)
				for(std::size_t column = 1; column <= columns; column++)
					get_reference(row, column) += other.get(row, column);
			return *this;
		}

		base_matrix<number_type> & operator-=(base_matrix <number_type>const & other)
		{
			if(!same_size(other))
				throw nil::exception("Invalid base_matrix sizes");
			
			for(std::size_t row = 1; row <= rows; row++)
				for(std::size_t column = 1; column <= columns; column++)
					get_reference(row, column) -= other.get(row, column);
			return *this;
		}
		
		base_matrix<number_type> & operator*=(base_matrix<number_type> const & other)
		{
			*this = *this * other;
			return *this;
		}

		base_matrix<number_type> & operator*=(number_type const & other)
		{
			multiply(other);
			return *this;
		}

		base_matrix<number_type> & operator/=(number_type const & other)
		{
			multiply(number_type(1) / other);
			return *this;
		}

		std::size_t get_offset(std::size_t row, std::size_t column) const
		{
			row--;
			column--;
			if(row >= rows || column >= columns)
				throw nil::exception("Matrix indices out of range");
			return row * columns + column;
		}

		number_type & get_reference(std::size_t row, std::size_t column)
		{
			return elements[get_offset(row, column)];
		}

	private:
		std::vector<number_type> elements;

		std::size_t rows;
		std::size_t columns;
	};

	typedef base_matrix<rational_number> matrix;
	typedef base_matrix<float> real_matrix;

	void simplex_algorithm(nil::matrix & matrix, bool debug = false, std::vector<std::string> * column_label_pointer = 0, std::vector<std::string> * row_label_pointer = 0);
	double get_norm(nil::real_matrix & vector);
};

std::ostream & operator<<(std::ostream & stream, nil::rational_number & number);
nil::matrix operator*(nil::rational_number const & left, nil::matrix right);
nil::real_matrix operator*(double const & left, nil::real_matrix right);

template< class T > inline T limit_min( T v, T mn ) { return (v>mn)?v:mn; }
template< class T > inline T limit_max( T v, T mx ) { return (v<mx)?v:mx; }
template< class T > inline T limit_minmax( T v, T mn, T mx ) { return limit_max<T>(limit_min<T>(v,mn),mx); }
template< class T > inline T limit_maxmin( T v, T mx, T mn ) { return limit_min<T>(limit_max<T>(v,mx),mn); }

#endif
