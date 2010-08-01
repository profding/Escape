/**
 * $Id$
 * Copyright (C) 2008 - 2009 Nils Asmussen
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef BASIC_STRING_H_
#define BASIC_STRING_H_

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <impl/string/char_traits.h>
#include <iterator>

namespace std {
	template<class charT,class traits>
	class basic_istream;
	template<class charT,class traits>
	class basic_ostream;
	template<class charT,class traits>
	class basic_stringbuf;

	template<typename T>
	class basic_string {
	public:
		typedef size_t size_type;
		typedef ptrdiff_t difference_type;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T* pointer;
		typedef const T* const_pointer;
		typedef pointer iterator;
		typedef const_pointer const_iterator;
		typedef std::reverse_iterator<iterator> reverse_iterator;
		typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

	private:
		static const size_type INIT_SIZE = 8;

	public:
		/**
		 * npos is a static member constant value with the greatest possible value for an element
		 * of type size_t.
		 * This value, when used as the value for a count parameter n in string's member
		 * functions, roughly indicates "as many as possible".
		 * When used in some pos parameters that allow for out-of-range values, npos indicates the
		 * end of the string.
		 * As a return value it is usually used to indicate failure.
		 */
		static const size_type npos = -1;

		/**
		 * Content is initialized to an empty string.
		 */
		explicit basic_string();
		/**
		 * Content is initialized to a copy of the string object str.
		 */
		basic_string(const basic_string& str);
		/**
		 * Content is initialized to a copy of a substring of str. The substring is the portion of
		 * str that begins at the character position pos and takes up to n characters (it takes
		 * less than n if the end of str is reached before).
		 */
		basic_string(const basic_string& str,size_type pos,size_type n = npos);
		/**
		 * Content is initialized to a copy of the string formed by the first n  characters in
		 * the array of characters pointed by s.
		 */
		basic_string(const T* s,size_type n);
		/**
		 * Content is initialized to a copy of the string formed by the null-terminated character
		 * sequence (C string) pointed by s. The length of the caracter sequence is determined
		 * by the first occurrence of a null character (as determined by traits.length(s)).
		 * This version can be used to initialize a string object using a string literal constant.
		 */
		basic_string(const T* s);
		/**
		 * Content is initialized as a string formed by a repetition of character c, n times.
		 */
		basic_string(size_type n,T c);
		/**
		 * The parameters are taken as iterators, and the content is initialized
		 * with the values of the elements that go from the element referred by iterator begin
		 * to the element right before the one referred by iterator end.
		 */
		template<class InputIterator>
		basic_string(InputIterator begin,InputIterator end);

		/**
		 * Destructor
		 */
		~basic_string();

		/**
		 * Sets a copy of the argument as the new content for the string object. The previous
		 * content is dropped.
		 * The assign member function provides a similar functionality with additional options.
		 */
		basic_string& operator=(const basic_string& str);
		basic_string& operator=(const T* s);
		basic_string& operator=(T c);

		/**
		 * @return the beginning of the string
		 */
		iterator begin();
		/**
		 * @return the beginning of the string, as const-iterator
		 */
		const_iterator begin() const;
		/**
		 * @return the end of the string
		 */
		iterator end();
		/**
		 * @return the end of the string, as const-iterator
		 */
		const_iterator end() const;
		/**
		 * @return the beginning of the string for the reverse-iterator (i.e. the end)
		 */
		reverse_iterator rbegin();
		/**
		 * @return the beginning of the string for the const-reverse-iterator (i.e. the end)
		 */
		const_reverse_iterator rbegin() const;
		/**
		 * @return the end of the string for the reverse-iterator (i.e. the beginning)
		 */
		reverse_iterator rend();
		/**
		 * @return the end of the string for the const-reverse-iterator (i.e. the beginning)
		 */
		const_reverse_iterator rend() const;

		/**
		 * @return a count of the number of characters in the string.
		 */
		size_type size() const;
		size_type length() const;
		/**
		 * @return the maximum number of characters that the string object can hold.
		 */
		size_type max_size() const;
		/**
		 * @return the size of the allocated storage space in the string object.
		 * 	The real limit on the size a string  object can reach is returned by member max_size.
		 */
		size_type capacity() const;

		/**
		 * Resizes the string content to n characters.
		 *
		 * If n is smaller than the current length of the string, the content is reduced to its
		 * first n characters, the rest being dropped.
		 * If n is greater than the current length of the string, the content is expanded by
		 * appending as many instances of the c character as needed to reach a size of n characters.
		 *
		 * The second version, actually calls: resize(n,char()), so when a string is resized
		 * to a greater size without passing a second argument, the new character positions are
		 * filled with the default value of a char, which is the null character.
		 */
		void resize(size_type n,T c);
		void resize(size_type n);

		/**
		 * Requests that the capacity  of the allocated storage space in the string be at least
		 * res_arg.
		 * This can expand or shrink the size of the storage space in the string, although notice
		 * that the resulting capacity after a call to this function is not necessarily equal to
		 * res_arg but can be either equal or greater than res_arg, therefore shrinking requests
		 * may or may not produce an actual reduction of the allocated space in a particular
		 * library implementation. In any case, it never trims the string content (for that
		 * purposes, see resize or clear, which modify the content).
		 */
		void reserve(size_type res_arg = 0);

		/**
		 * The string content is set to an empty string, erasing any previous content and thus
		 * leaving its size at 0 characters.
		 */
		void clear();

		/**
		 * @return whether the string is empty, i.e. whether its size is 0.
		 */
		bool empty() const;

		/**
		 * Returns a reference to the character at position pos in the string.
		 * The function actually returns data()[ pos ].
		 * The at member function has the same behavior as this operator function, except that at
		 * also performs a range check.
		 */
		const_reference operator[](size_type pos) const;
		reference operator[](size_type pos);

		/**
		 * Returns the character at position pos in the string.
		 * This member function behaves as operator[] , except that at also performs a
		 * range check, throwing an exception of type out_of_range in case that pos is not an
		 * actual position in the string.
		 */
		const_reference at(size_type pos) const;
		reference at(size_type pos);

		/**
		 * Appends a copy of the argument to the string.
		 * The new string content is the content existing in the string object before the call
		 * followed by the content of the argument.
		 * The append member function provides a similar functionality with additional options.
		 */
		basic_string& operator+=(const basic_string& str);
		basic_string& operator+=(const T* s);
		basic_string& operator+=(char c);

		/**
		 * Appends a copy of str.
		 */
		basic_string& append(const basic_string& str);
		/**
		 * Appends a copy of a substring of str. The substring is the portion of str that begins
		 * at the character position pos and takes up to n characters (it takes less than n if
		 * the end of string is reached before).
		 * If the position passed is past the end of str, an out_of_range exception is thrown.
		 */
		basic_string& append(const basic_string& str,size_type pos,size_type n);
		/**
		 * Appends a copy of the string formed by the first n characters in the array of
		 * characters pointed by s.
		 */
		basic_string& append(const T* s,size_type n);
		/**
		 * Appends a copy of the string formed by the null-terminated character sequence (C string)
		 * pointed by s. The length of this character sequence is determined by the first ocurrence
		 * of a null character (as determined by traits.length(s)).
		 */
		basic_string& append(const T* s);
		/**
		 * Appends a string formed by the repetition n times of character c.
		 */
		basic_string& append(size_type n,T c);
		/**
		 * The parameters are considered iterators and the content is appended by the
		 * that go from element referred by iterator first to the element right before the
		 * one referred by iterator last.
		 */
		template<class InputIterator>
		basic_string& append(InputIterator first,InputIterator last);

		/**
		 * Appends a single character to the string content, increasing its size by one.
		 */
		void push_back(T c);

		/**
		 * Sets a copy of str as the new content.
		 */
		basic_string& assign(const basic_string& str);
		/**
		 * Sets a copy of a substring of str as the new content. The substring is the portion of
		 * str that begins at the character position pos and takes up to n characters (it
		 * takes less than n if the end of str is reached before).
		 */
		basic_string& assign(const basic_string& str,size_type pos,size_type n);
		/**
		 * Sets as the new content a copy of the string formed by the first n  characters of the
		 * array pointed by s.
		 */
		basic_string& assign(const T* s,size_type n);
		/**
		 * Sets a copy of the string formed by the null-terminated character sequence (C string)
		 * pointed by s as the new content. The length of the caracter sequence is determined by
		 * the first ocurrence of a null character (as determined by traits.length(s)).
		 */
		basic_string& assign(const T* s);
		/**
		 * Sets a string formed by a repetition of character c, n times, as the new content.
		 */
		basic_string& assign(size_type n,T c);
		/**
		 * The content is set to the values of the elements that go from element
		 * referred to by iterator first to the element right before the one referred
		 * to by iterator last.
		 */
		template<class InputIterator>
		basic_string& assign(InputIterator first,InputIterator last);

		/**
		 * Inserts a copy of the entire string  object str at character position pos1.
		 */
		basic_string& insert(size_type pos1,const basic_string& str);
		/**
		 * Inserts a copy of a substring of str at character position pos1. The substring is the
		 * portion of str that begins at the character position pos2 and takes up to n characters
		 * (it takes less than n if the end of str is reached before).
		 */
		basic_string& insert(size_type pos1,const basic_string& str,size_type pos2,size_type n);
		/**
		 * Inserts at the character position pos1, a copy of the string formed by the first n
		 * characters in the array of characters pointed by s.
		 */
		basic_string& insert(size_type pos1,const T* s,size_type n);
		/**
		 * Inserts at character position pos1, a copy of the string formed by the null-terminated
		 * character sequence (C string) pointed by s. The length of this caracter sequence is
		 * determined by the first ocurrence of a null character (as determined by traits.length(s)).
		 */
		basic_string& insert(size_type pos1,const T* s);
		/**
		 * Inserts a string formed by a repetition of character c, n times, at the character
		 * position pos1.
		 */
		basic_string& insert(size_type pos1,size_type n,T c);
		/**
		 * Inserts at the internal position referred by p the content made up of the characters
		 * that go from the element referred by iterator first to the element right before the
		 * one referred by iterator last.
		 */
		template<class InputIterator>
		void insert(iterator p,InputIterator first,InputIterator last);

		/**
		 * Erases a sequence of n characters starting at position pos. Notice that both parameters
		 * are optional: with only one argument, the function deletes everything from position pos
		 * forwards, and with no arguments, the function deletes the entire string, like member
		 * clear.
		 */
		basic_string& erase(size_type pos = 0,size_type n = npos);
		/**
		 * Erases the character referred by the iterator position. Only one character is affected.
		 */
		iterator erase(iterator position);
		/**
		 * Erases all the characters between first and last.
		 */
		iterator erase(iterator first,iterator last);

		/**
		 * The section is replaced by a copy of the entire string object str.
		 */
		basic_string& replace(size_type pos1,size_type n1,const basic_string& str);
		basic_string& replace(iterator i1,iterator i2,const basic_string& str);
		/**
		 * The section is replaced by a copy of a substring of str. The substring is the portion
		 * of str that begins at the character position pos2 and takes up to n2 characters
		 * (it takes less than n2 if the end of the string is reached before).
		 */
		basic_string& replace(size_type pos1,size_type n1,const basic_string& str,size_type pos2,
				size_type n2);
		/**
		 * The section is replaced by a copy of the string formed by the first n2  characters in
		 * the array of characters pointed by s.
		 */
		basic_string& replace(size_type pos1,size_type n1,const T* s,size_type n2);
		basic_string& replace(iterator i1,iterator i2,const T* s,size_type n2);
		/**
		 * The section is replaced by a copy of the string formed by the null-terminated character
		 * sequence (C string) pointed by s. The length of this caracter sequence is determined
		 * by the first ocurrence of a null character (as determined by traits.length(s)).
		 */
		basic_string& replace(size_type pos1,size_type n1,const T* s);
		basic_string& replace(iterator i1,iterator i2,const T* s);
		/**
		 * The section is replaced by a repetition of character c, n2  times.
		 */
		basic_string& replace(size_type pos1,size_type n1,size_type n2,T c);
		basic_string& replace(iterator i1,iterator i2,size_type n2,T c);
		/**
		 * The section is replaced by the content made up of the characters that go from the
		 * element referred by iterator j1 to the element right before the one referred by
		 * iterator j2.
		 */
		template<class InputIterator>
		basic_string& replace(iterator i1,iterator i2,InputIterator j1,InputIterator j2);

		/**
		 * Copies a sequence of characters from the string content to the array pointed by s.
		 * This sequence of characters is made of the characters in the string that start at
		 * character position pos and span n characters from there.
		 *
		 * The function does not append a null character after the content copied. To retrieve
		 * a temporary c-string value from a string object, a specific member function
		 * exists: c_str.
		 */
		size_type copy(T* s,size_type n,size_type pos = 0) const;

		/**
		 * Swaps the contents of the string with those of string object str, such that after the
		 * call to this member function, the contents of this string are those which were in str
		 * before the call, and the contents of str are those which were in this string.
		 *
		 * Notice that a global function with the same name, swap, exists with the same behavior,
		 * and which acts as a specialization for strings of the algorithm function with
		 * the same name.
		 */
		void swap(basic_string& str);

		/**
		 * Generates a null-terminated sequence of characters (c-string) with the same content
		 * as the string object and returns it as a pointer to an array of characters.
		 *
		 * A terminating null character is automatically appended.
		 *
		 * The returned array points to an internal location with the required storage space
		 * for this sequence of characters plus its terminating null-character, but the values
		 * in this array should not be modified in the program and are only granted to remain
		 * unchanged until the next call to a non-constant member function of the string object.
		 */
		const_pointer c_str() const;

		/**
		 * Returns a pointer to an array of characters with the same content as the string.
		 * Notice that no terminating null character is appended (see member c_str for such a
		 * functionality).
		 * The returned array points to an internal location which should not be modified directly
		 * in the program. Its contents are guaranteed to remain unchanged only until the next
		 * call to a non-constant member function of the string object.
		 */
		const_pointer data() const;

		/**
		 * Searches the string for the content specified in either str, s  or c, and returns the
		 * position of the first occurrence in the string.
		 * When pos is specified the search only includes characters on or after position pos,
		 * ignoring any possible occurrences in previous locations.
		 * Notice that unlike member find_first_of, whenever more than one character is being
		 * searched for, it is not enough that only one of these characters match, but the
		 * entire sequence of characters to find must be matched.
		 */
		size_type find(const basic_string& str,size_type pos = 0) const;
		size_type find(const T* s,size_type pos,size_type n) const;
		size_type find(const T* s,size_type pos = 0) const;
		size_type find(T c,size_type pos = 0) const;

		/**
		 * Searches the string for the content specified in either str, s  or c, and returns the
		 * position of the last occurrence in the string.
		 * When pos is specified, the search only includes characters between the beginning of
		 * the string and position pos, ignoring any possible occurrences after pos.
		 */
		size_type rfind(const basic_string& str,size_type pos = npos) const;
		size_type rfind(const T* s,size_type pos,size_type n) const;
		size_type rfind(const T* s,size_type pos = npos) const;
		size_type rfind(T c,size_type pos = npos) const;

		/**
		 * Searches the string for any of the characters that are part of either str, s or c,
		 * and returns the position of the first occurrence in the string.
		 * When pos is specified the search only includes characters on or after position pos,
		 * ignoring any possible occurrences at previous character positions.
		 * Notice that for a match to happen it is enough that one of the characters matches in
		 * the string (any of them). To search for an entire sequence of characters use find
		 * instead.
		 */
		size_type find_first_of(const basic_string& str,size_type pos = 0) const;
		size_type find_first_of(const T* s,size_type pos,size_type n) const;
		size_type find_first_of(const T* s,size_type pos = 0) const;
		size_type find_first_of(T c,size_type pos = 0) const;

		/**
		 * Searches the string from the end for any of the characters that are part of either
		 * str, s or c, and returns the position of the last occurrence in the string.
		 * When pos is specified the search only includes characters on or before position
		 * pos, ignoring any possible occurrences at character positions after it.
		 * Notice that for a match to happen is enough that one of the characters matches in the
		 * string (any of them). To search from the end for an entire sequence of characters use
		 * rfind instead.
		 */
		size_type find_last_of(const basic_string& str,size_type pos = npos) const;
		size_type find_last_of(const T* s,size_type pos,size_type n) const;
		size_type find_last_of(const T* s,size_type pos = npos) const;
		size_type find_last_of(T c,size_type pos = npos) const;

		/**
		 * Searches for the first character in the object which is not part of either str, s or c,
		 * and returns its position.
		 * When pos is specified the search only includes characters on or after position pos,
		 * ignoring any content in the previous character positions.
		 */
		size_type find_first_not_of(const basic_string& str,size_type pos = 0) const;
		size_type find_first_not_of(const T* s,size_type pos,size_type n) const;
		size_type find_first_not_of(const T* s,size_type pos = 0) const;
		size_type find_first_not_of(T c,size_type pos = 0) const;

		/**
		 * Searches for the last character in the object which is not part of either str, s or
		 * c, and returns its position.
		 * When pos is specified the search only includes characters on or before position pos,
		 * ignoring any content in the character positions after it.
		 */
		size_type find_last_not_of(const basic_string& str,size_type pos = npos) const;
		size_type find_last_not_of(const T* s,size_type pos,size_type n) const;
		size_type find_last_not_of(const T* s,size_type pos = npos) const;
		size_type find_last_not_of(T c,size_type pos = npos) const;

		/**
		 * Returns a string object with its contents initialized to a substring of the current
		 * object.
		 * This substring is the character sequence that starts at character position pos and
		 * has a length of n characters.
		 */
		basic_string substr(size_type pos = 0,size_type n = npos) const;

		/**
		 * Compares the content of this object (or a substring of it, known as compared
		 * (sub)string) to the content of a comparing string, which is formed according to the
		 * arguments passed.
		 * The member function returns 0 if all the characters in the compared contents compare
		 * equal, a negative value if the first character that does not match compares to less
		 * in the object than in the comparing string, and a positive value in the opposite case.
		 * Notice that for string objects, the result of a character comparison depends only on
		 * its character code (i.e., its ASCII code), so the result has some limited alphabetical
		 * or numerical ordering meaning.
		 * For other basic_string class instantitations, the comparison depends on the
		 * specific traits::compare function, where traits is one of the class template parameters.
		 */
		int compare(const basic_string& str) const;
		int compare(const T* s) const;
		int compare(size_type pos1,size_type n1,const basic_string& str) const;
		int compare(size_type pos1,size_type n1,const T* s) const;
		int compare(size_type pos1,size_type n1,const basic_string& str,size_type pos2,
					size_type n2) const;
		int compare(size_type pos1,size_type n1,const T* s,size_type n2) const;

	private:
		T* _str;
		size_type _size;
		size_type _length;
	};

	typedef basic_string<char> string;

	/**
	 * Returns a string object whose contents are the combination of the content of lhs followed
	 * by those of rhs.
	 * Because the diversity of left-hand parameter types, this function is implemented as an
	 * overload of the global operator+ function.
	 */
	template<typename T>
	basic_string<T> operator+(const basic_string<T>& lhs,const basic_string<T>& rhs);
	template<typename T>
	basic_string<T> operator+(const T* lhs,const basic_string<T>& rhs);
	template<typename T>
	basic_string<T> operator+(T lhs,const basic_string<T>& rhs);
	template<typename T>
	basic_string<T> operator+(const basic_string<T>& lhs,const T* rhs);
	template<typename T>
	basic_string<T> operator+(const basic_string<T>& lhs,T rhs);

	/**
	 * Inserts the string object str  into the output stream os.
	 * This function overloads the global operator<< to behave as described in ostream::operator<<
	 * but applied to string objects.
	 */
	template<class charT,class traits>
	basic_ostream<charT,traits>& operator <<(basic_ostream<charT,traits>& os,
				const basic_string<charT>& s);
	/**
	 * Extracts a string from the input stream is storing its content in str. Any previous content
	 * of str is cleared.
	 * This function overloads the global operator>> to behave as described in istream::operator>>
	 * but applied to string objects.
	 * Notice that the istream extraction operations use whitespaces as separators, therefore this
	 * operation will only extract what can be considered a word from the stream. To extract entire
	 * lines of text, refer to the string overload of global function getline.
	 */
	template<class charT,class traits>
	basic_istream<charT,traits>& operator >>(basic_istream<charT,traits>& in,basic_string<charT>& s);

	/**
	 * Swaps the contents of the string  objects lhs and rhs, such that after the call to this
	 * function, the contents of rhs are those which were in lhs before the call, and the contents
	 * of lhs are those which were in rhs.
	 * Notice that string objects implement a member function also called swap; In fact, this
	 * global function effectively calls:
	 * lhs.swap(rhs);
	 */
	template<typename T>
	void swap(basic_string<T>& lhs,basic_string<T>& rhs);

	/**
	 * Extracts characters from <is> and stores them into <str> until a delimitation character is
	 * found. The delimiter character is <delim> for the first function version, and '\n'
	 * for the second. The extraction also stops if the end of file is reached in is or
	 * if some other error occurs during the input operation.
	 * If the delimiter is found, it is extracted and discarded, i.e. it is not stored and the
	 * next input operation will begin after it.
	 *
	 * @param is the input-stream
	 * @param str the string to write into
	 * @param delim the delimiter
	 * @return is
	 */
	template<class charT,class traits>
	basic_istream<charT,traits>& getline(basic_istream<charT,traits>& is,basic_string<charT>& str,
			charT delim);
	template<class charT,class traits>
	basic_istream<charT,traits>& getline(basic_istream<charT,traits>& is,basic_string<charT>& str);

	/**
	 * These overloaded global operator functions perform the appropriate comparison operation,
	 * between lhs and rhs.
	 * These function depend on the value returned by string member compare.
	 */
	template<typename T>
	bool operator==(const basic_string<T>& lhs,const basic_string<T>& rhs);
	template<typename T>
	bool operator==(const T* lhs,const basic_string<T>& rhs);
	template<typename T>
	bool operator==(const basic_string<T>& lhs,const T* rhs);
	template<typename T>
	bool operator!=(const basic_string<T>& lhs,const basic_string<T>& rhs);
	template<typename T>
	bool operator!=(const T* lhs,const basic_string<T>& rhs);
	template<typename T>
	bool operator!=(const basic_string<T>& lhs,const T* rhs);
	template<typename T>
	bool operator<(const basic_string<T>& lhs,const basic_string<T>& rhs);
	template<typename T>
	bool operator<(const T* lhs,const basic_string<T>& rhs);
	template<typename T>
	bool operator<(const basic_string<T>& lhs,const T* rhs);
	template<typename T>
	bool operator>(const basic_string<T>& lhs,const basic_string<T>& rhs);
	template<typename T>
	bool operator>(const T* lhs,const basic_string<T>& rhs);
	template<typename T>
	bool operator>(const basic_string<T>& lhs,const T* rhs);
	template<typename T>
	bool operator<=(const basic_string<T>& lhs,const basic_string<T>& rhs);
	template<typename T>
	bool operator<=(const T* lhs,const basic_string<T>& rhs);
	template<typename T>
	bool operator<=(const basic_string<T>& lhs,const T* rhs);
	template<typename T>
	bool operator>=(const basic_string<T>& lhs,const basic_string<T>& rhs);
	template<typename T>
	bool operator>=(const T* lhs,const basic_string<T>& rhs);
	template<typename T>
	bool operator>=(const basic_string<T>& lhs,const T* rhs);
}

#endif /* BASIC_STRING_H_ */