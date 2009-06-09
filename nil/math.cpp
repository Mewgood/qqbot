#include <nil/math.hpp>

#include <sstream>
#include <iostream>
#include <cmath>
#include <functional>
#include <algorithm>
#include <deque>

#include <nil/string.hpp>

namespace nil
{
	float const pi = static_cast<float>(M_PI);

	int round(double input)
	{
		return static_cast<int>(static_cast<double>(static_cast<int>(input + 0.5)));
	}

	int absolute(int input)
	{
		if(input < 0)
			return -input;
		else
			return input;
	}

	float positive_sin(float x)
	{
		return (std::sin(x) + 1.0f) / 2.0f;
	}

	float positive_cos(float x)
	{
		return (std::cos(x) + 1.0f) / 2.0f;
	}

	unsigned gcd(unsigned left, unsigned right)
	{
		if(right == 0)
			return left;
		else
			return gcd(right, left % right);
	}

	rational_number::rational_number():
		numerator(0),
		denominator(1)
	{
	}

	rational_number::rational_number(int integer):
		numerator(integer),
		denominator(1)
	{
	}

	rational_number::rational_number(int numerator, int denominator):
		numerator(numerator),
		denominator(denominator)
	{
		if(denominator == 0)
			throw exception("The denominator must not be zero");
	}
	
	rational_number & rational_number::operator+=(rational_number const & other)
	{
		int other_numerator = other.numerator * denominator;
		numerator *= other.denominator;
		denominator *= other.denominator;
		numerator += other_numerator;
		euclid();
		return *this;
	}

	rational_number & rational_number::operator-=(rational_number const & other)
	{
		
		operator+=(-other);
		return *this;
	}

	rational_number & rational_number::operator*=(rational_number const & other)
	{
		numerator *= other.numerator;
		denominator *= other.denominator;
		euclid();
		return *this;
	}

	rational_number & rational_number::operator/=(rational_number const & other)
	{
		if(other.numerator == 0)
			throw exception("Division by zero");

		numerator *= other.denominator;
		denominator *= other.numerator;
		euclid();
		return *this;
	}
	
	rational_number rational_number::operator+(rational_number const & other) const
	{
		rational_number output = *this;
		output += other;
		return output;
	}

	rational_number rational_number::operator-(rational_number const & other) const
	{
		rational_number output = *this;
		output -= other;
		return output;
	}

	rational_number rational_number::operator*(rational_number const & other) const
	{
		rational_number output = *this;
		output *= other;
		return output;
	}

	rational_number rational_number::operator/(rational_number const & other) const
	{
		rational_number output = *this;
		output /= other;
		return output;
	}

	rational_number rational_number::operator-() const
	{
		return rational_number(-numerator, denominator);
	}

	bool rational_number::operator<(rational_number const & other) const
	{
		rational_number difference = *this - other;
		return difference.negative();
	}

	bool rational_number::operator>(rational_number const & other) const
	{
		rational_number difference = other - *this;
		return difference.negative();
	}

	bool rational_number::operator==(rational_number const & other) const
	{
		return numerator == other.numerator && denominator == other.denominator;
	}

	bool rational_number::operator!=(rational_number const & other) const
	{
		return !operator==(other);
	}

	bool rational_number::operator<=(rational_number const & other) const
	{
		return !operator>(other);
	}

	bool rational_number::operator>=(rational_number const & other) const
	{
		return !operator<(other);
	}

	bool rational_number::negative() const
	{
		return (numerator < 0 && denominator > 0) || (numerator >= 0 && denominator < 0);
	}

	rational_number rational_number::square_root() const
	{
		if(negative())
			throw exception("No complex numbers");
		int new_numerator = static_cast<int>(std::sqrt(static_cast<float>(numerator)));
		int new_denominator = static_cast<int>(std::sqrt(static_cast<float>(denominator)));
		if(new_numerator * new_numerator != numerator || new_denominator * new_denominator != denominator)
			throw exception("Irrational number");
		return rational_number(new_numerator, new_denominator);
	}

	double rational_number::approximation() const
	{
		return static_cast<double>(numerator) / static_cast<double>(denominator);
	}

	std::string rational_number::string() const
	{
		std::stringstream stream;
		if(negative())
			stream << '-';
		stream << std::abs(numerator);
		if(std::abs(denominator) != 1)
			stream << "/" << std::abs(denominator);
		return stream.str();
	}

	void rational_number::euclid()
	{
		int divisor = gcd(absolute(numerator), absolute(denominator));
		numerator /= divisor;
		denominator /= divisor;

		if(numerator == 0)
			denominator = 1;
	}

	struct candidate_entry
	{
		std::size_t row, column;
		nil::rational_number value;

		candidate_entry()
		{
		}

		candidate_entry(nil::rational_number value, std::size_t row, std::size_t column):
			value(value),
			row(row),
			column(column)
		{
		}

		bool operator>(candidate_entry const & other) const
		{
			return value > other.value;
		}
	};

	void simplex_algorithm(nil::matrix & matrix, bool debug, std::vector<std::string> * column_label_pointer, std::vector<std::string> * row_label_pointer)
	{
		std::size_t rows = matrix.get_rows();
		std::size_t columns = matrix.get_columns();

		std::vector<std::string> row_labels, column_labels;
		std::deque<bool> row_booleans, column_booleans;

		if(column_label_pointer == 0)
			for(std::size_t column = 1; column < columns; column++)
				column_labels.push_back("x" + nil::string::number_to_string<std::size_t>(column));
		else
			column_labels = *column_label_pointer;

		if(row_label_pointer == 0)
			for(std::size_t row = 1; row < rows; row++)
				row_labels.push_back("y" + nil::string::number_to_string<std::size_t>(row));
		else
			row_labels = *row_label_pointer;

		for(std::size_t row = 1; row < rows; row++)
			row_booleans.push_back(false);
		for(std::size_t column = 1; column < columns; column++)
			column_booleans.push_back(true);

		while(true)
		{
			std::cout << std::endl << "Column labels: ";
			for(std::size_t column = 1; column < columns; column++)
				std::cout << column_labels[column - 1] << " ";
			std::cout << std::endl;

			std::cout << "Row labels: ";
			for(std::size_t row = 1; row < rows; row++)
				std::cout << row_labels[row - 1] << " ";
			std::cout << std::endl << std::endl;

			std::cout << matrix.string() << std::endl;

			//choose a pivot column

			//check z entries for negativity
			bool hit = false;
			bool all_negative = true;
			std::size_t pivot_column;
			for(std::size_t column = 1; column < columns; column++)
			{
				nil::rational_number value = matrix.get(rows, column);
				bool negative = value.negative();
				//std::cout << "z_" << column << " = " << value.string() << std::endl;
				if(!negative)
				{
					all_negative = false;
					if(column_booleans[column - 1])
					{
						std::cout << "Found an appropriate column with a non-negative z-entry: " << column << std::endl;
						pivot_column = column;
						hit = true;
						break;
					}
				}
			}

			if(!hit)
			{
				if(all_negative)
					std::cout << "Terminating" << std::endl;
				else
				{
					std::cout << "There are still non-negative z-entries, eliminating x-entries:" << std::endl;
					std::size_t new_rows = rows - (columns - 1);
					nil::matrix new_matrix(new_rows, columns);
					std::size_t new_matrix_row = 1;
					std::vector<std::string> new_row_labels;
					for(std::size_t row = 1; row <= rows; row++)
					{
						if(row < rows)
						{
							if(row_labels[row - 1][0] == 'x')
								continue;
							new_row_labels.push_back(row_labels[row - 1]);
						}

						for(std::size_t column = 1; column <= columns; column++)
							new_matrix.set(new_matrix_row, column, matrix.get(row, column));

						new_matrix_row++;
					}

					//std::cin.get();

					simplex_algorithm(new_matrix, debug, &column_labels, &new_row_labels);
				}
				return;
			}

			std::vector<candidate_entry> candidates;
			for(std::size_t row = 1; row <= rows; row++)
			{
				if(matrix.get(row, pivot_column) == 0)
				{
					std::cout << "b(" << row << ", " << pivot_column << ") == 0, skipping" << std::endl;
					continue;
				}
				nil::rational_number candidate = matrix.get(row, columns) / matrix.get(row, pivot_column);
				if(candidate < 0)
					candidates.push_back(candidate_entry(candidate, row, pivot_column));
			}

			if(candidates.empty())
			{
				std::cout << "Problem: There are no valid pivot candidates, all elements were positive" << std::endl;
				return;
			}

			std::sort(candidates.begin(), candidates.end(), std::greater<candidate_entry>());

			candidate_entry pivot_entry = candidates[0];
			std::size_t pivot_row = pivot_entry.row;
			nil::rational_number pivot = matrix.get(pivot_row, pivot_column);

			std::cout << "Pivot element: b(" << pivot_row << ", " << pivot_column << ") = " << pivot << std::endl;

			nil::matrix output_matrix(rows, columns);

			for(std::size_t row = 1; row <= rows; row++)
			{
				for(std::size_t column = 1; column <= columns; column++)
				{
					nil::rational_number value = matrix.get(row, column), output;
					if(row == pivot_row && column == pivot_column)
						output = nil::rational_number(1) / pivot;
					else if(row != pivot_row && column == pivot_column)
						output = value / pivot;
					else if(row == pivot_row && column != pivot_column)
						output = - value / pivot;
					else
					{
						output = value - matrix.get(pivot_row, column) * matrix.get(row, pivot_column) / pivot;
						if(debug)
						{
							std::cout << "\tb(" << row << ", " << column << ") = b(" << row << ", " << column << ") - b(" << pivot_row << ", " << column << ") * b(" << row << ", " <<  pivot_column << ") / b(" << pivot_row << ", " << pivot_column << ")" << std::endl;
							std::cout << "\tb(" << row << ", " << column << ") = " << value.string() << " - " << matrix.get(pivot_row, column).string() << " * " << matrix.get(row, pivot_column).string() << " / (" << pivot.string() << ")" << std::endl;
						}
					}
					if(debug)
						std::cout << "b(" << row << ", " << column << ") = " << output.string() << std::endl;
					output_matrix.set(row, column, output);
				}
			}

			std::swap(row_labels[pivot_row - 1], column_labels[pivot_column - 1]);
			std::swap(row_booleans[pivot_row - 1], column_booleans[pivot_column - 1]);

			matrix = output_matrix;

			//std::cin.get();
		}
	}

	template <>
		std::string get_number_string<rational_number>(rational_number input)
	{
		return input.string();
	}

	double get_norm(nil::real_matrix & vector)
	{
		double sum = 0.0;
		std::size_t rows = vector.get_rows();
		std::size_t columns = vector.get_columns();
		for(std::size_t row = 1; row <= rows; row++)
		{
			for(std::size_t column = 1; column <= columns; column++)
			{
				double field = vector.get(row, column);
				sum += std::pow(field, 2.0);
			}
		}
		return std::pow(sum, 0.5);
	}
}

std::ostream & operator<<(std::ostream & stream, nil::rational_number & number)
{
	stream << number.string();
	return stream;
}

nil::matrix operator*(nil::rational_number const & left, nil::matrix right)
{
	right.multiply(left);
	return right;
}

nil::real_matrix operator*(double const & left, nil::real_matrix right)
{
	//??
	right.multiply(static_cast<float>(left));
	return right;
}
