#include <Windows.h>
#include <cassert>
#include <stdint.h>
#include <iostream>
#include <map>

// Set to 0 in case of issue, tracking allocation works at least on visual studio 2015, no guarantee for any compiler.
#define USE_ALLOCATION_CHECKER 1
int32_t glo_allocatedByteCount = 0;
int32_t glo_allocationCount = 0;

void* my_alloc(size_t size)
{
#if USE_ALLOCATION_CHECKER && defined (_DEBUG)
	glo_allocatedByteCount += (int32_t)size;
#endif
	glo_allocationCount++;

	return malloc(size);
}

void my_free(void* pointer)
{
	if (pointer != nullptr)
	{
#if USE_ALLOCATION_CHECKER && defined (_DEBUG)
		size_t* hack0 = (size_t*)pointer - 1;
		size_t* hack1 = (size_t*)pointer - 2;
		size_t* hack2 = (size_t*)pointer - 3;
		size_t* hack3 = (size_t*)pointer - 4;

#if defined(_WIN64)
		glo_allocatedByteCount -= (int32_t)(*hack1);
#elif defined(_WIN32)
		glo_allocatedByteCount -= (int32_t)(*hack2);
#endif
#endif
		free(pointer);
	}
}

// Forbid a few things for the exercice.
#define new		new_is_forbidden
#define delete  delete_is_forbidden
#define malloc  malloc_is_forbidden
#define free    free_is_forbidden
#define realloc realloc_is_forbidden
#define strcmp  strcmp_is_forbidden_code_it_yourself
#define strlen  strlen_is_forbidden_code_it_yourself

//-------------------------------------------------------------------------------------------------
// exercise: 
//    Implement CString to make the main function to execute and work properly.
//      -> Do not add feature or member function that are not strictly necessary for the main function to work.
//      -> authorized functions only: my_alloc, my_free and that's all.
//      -> do not add any member
//      -> do not use new/delete
//      -> do not use malloc/free/realloc (use my_alloc, my_free instead.)
//      -> do not use strcmp/strcpy/strlen code it yourself.
// aim of the exercise:
//    -> C skills: proper array indexing && consistent memory management.
//    -> C++ class model features.
// note:
//    The CString class to implement is not the state of the art of string classes, and will not be
//    very efficient. Keep it as simple and minimal as possible.
//    Be ready to talk about how to improve the class behavior.
//-------------------------------------------------------------------------------------------------

class CString
{
public:
	CString() 
	{
		m_storage = nullptr; 
	};

	CString(const CString& _str) 
	{
		m_storage = my_strcpy(_str.m_storage); 
	}

	CString(const char* _str)
	{
		if (nullptr == _str || _str[0] == '\0')
		{
			m_storage = nullptr;
			return;
		}
		m_storage = my_strcpy(_str);
	}

	CString(CString&& _str) noexcept
	{
		m_storage = _str.m_storage;
		_str.m_storage = nullptr;
	}

	~CString()
	{
		if (m_storage) my_free(m_storage);
	}

	char& operator[](size_t _index)
	{
		return m_storage[_index];
	}

	char operator[](size_t _index) const
	{
		return m_storage[_index];
	}

	CString& operator=(const CString& _str)
	{
		if (this == &_str) return *this; // Cstring are the same
		char* oldPtr = m_storage;
		m_storage = my_strcpy(_str.c_str());
		if (oldPtr) my_free(oldPtr);
		return *this;
	}

	CString& operator+=(const CString& _str)
	{
		char* oldPtr = m_storage;
		m_storage = my_strcat(this->m_storage, _str.m_storage);
		if (oldPtr) my_free(oldPtr);
		return *this;
	}

	CString operator+(const CString& _str) const
	{
		CString newString;
		newString.m_storage = my_strcat(this->m_storage, _str.m_storage);
		return newString;
	}

	inline bool operator==(const CString& _str) const { return *this == _str.m_storage; }

	bool operator==(const char* _str) const
	{
		if (nullptr == this->m_storage && nullptr == _str) return true; // The strings are both empty
		if (nullptr == this->m_storage || nullptr == _str) return false; // One of the strings is empty
		size_t size = this->size();
		if (size != my_strlen(_str)) return false; // The strings have different sizes
		for (size_t i = 0; i < size; i++)
		{
			if (this->m_storage[i] != _str[i]) return false; // A char is different
		}
		return true;
	}

	inline bool operator<(const CString& _str) const { return this->size() < _str.size(); }

	friend std::ostream& operator<<(std::ostream& _stream, const CString& _str)
	{
		return _stream << _str.c_str();
	}

	const char* c_str() const
	{
		if (m_storage) return m_storage;
		return "";
	}

	void clear()
	{
		if (m_storage) my_free(m_storage);
		m_storage = nullptr;
	}

	bool empty()
	{
		return nullptr == m_storage || m_storage[0] == '\0';
	}

	inline const size_t size() const { return my_strlen(m_storage); }

private:

	static size_t my_strlen(const char* _str)
	{
		if (nullptr == _str) return 0;
		int count = 0;
		while (*_str++)
		{
			count++;
		}
		return count;
	}

	static char* my_strcpy(const char* _str)
	{
		if (nullptr == _str) return nullptr;
		size_t size = my_strlen(_str) + 1;
		char* newArray = (char*)my_alloc(size);
		if (nullptr == newArray) return nullptr; // Failed to alloc
		for (size_t i = 0; i < size; i++)
		{
			newArray[i] = _str[i];
		}
		return newArray;
	}

	static char* my_strcat(const char* _str1, const char* _str2)
	{
		if (nullptr == _str1 || nullptr == _str2) return nullptr;
		size_t sizeArray1 = my_strlen(_str1);
		size_t sizeArray2 = my_strlen(_str2) + 1;
		size_t sizeNewArray = sizeArray1 + sizeArray2;
		char* newArray = (char*)my_alloc(sizeNewArray);
		if (nullptr == newArray) return nullptr; // Failed to alloc
		size_t offsetNewArray = 0;
		for (size_t i = 0; i < sizeArray1; i++)
		{
			newArray[offsetNewArray] = _str1[i];
			offsetNewArray++;
		}
		for (size_t i = 0; i < sizeArray2; i++)
		{
			newArray[offsetNewArray] = _str2[i];
			offsetNewArray++;
		}
		return newArray;
	}

	char* m_storage;
};


int main()
{
	assert(glo_allocatedByteCount == 0);

	// test case 0
	{
		CString test0;
		CString emptyString = "";
		assert(glo_allocationCount == 0);
	}
	assert(glo_allocatedByteCount == 0);
	// test case 1
	{
		CString test0;
		assert(test0.empty());
		assert(test0.size() == 0);
		test0.clear();
		assert(test0.empty());
		assert(test0.size() == 0);

		CString test1 = "tutu";
		test1.clear();
		assert(test1.empty());
		assert(test1.size() == 0);

		printf("<%s> has a length of <%u>\n", test1.c_str(), (int)test1.size());
	}
	assert(glo_allocatedByteCount == 0);
	// test case 2
	{
		CString test1 = "a string";
		CString test2 = test1;
		test1 = test2;
		test2 = test1.c_str();
		assert(test2 == test1);
		CString test3 = std::move(test1);
		assert(test1.empty());
	}
	assert(glo_allocatedByteCount == 0);
	// test case 3
	{
		CString test0 = "the beginning, ";
		CString test1 = "the end.";
		CString testConcat = test0 + test1;
		printf("<%s> has a length of <%u>\n", testConcat.c_str(), (int)testConcat.size());
	}
	// test case 4
	assert(glo_allocatedByteCount == 0);
	{
		CString emptyString;
		std::cout << emptyString << std::endl;
	}
	assert(glo_allocatedByteCount == 0);
	// test case 5
	{
		CString test0 = "It's wuper";
		CString const test1 = "It's wuper";
		test0[5] = test1[3];
		std::cout << test0 << std::endl;
	}
	assert(glo_allocatedByteCount == 0);
	// test case 6
	{
		CString test0 = "blablablablabla";
		test0 = test0;
		printf("<%s> has a length of <%u>\n", test0.c_str(), (int)test0.size());
	}
	assert(glo_allocatedByteCount == 0);
	// test case 7
	{
		std::map< CString, CString > stringToStringMap;
		stringToStringMap["tutu2"] = "toto2";
		stringToStringMap["tutu1"] = "toto1";
		stringToStringMap["baba54"] = "toto54";

		int keyIndex = 0;
		for (auto keyValue : stringToStringMap)
		{
			if (keyIndex == 0)
			{
				assert(keyValue.first == "baba54");
				assert(keyValue.second == "toto54");
			}
			else if (keyIndex == 1)
			{
				assert(keyValue.first == "tutu1");
				assert(keyValue.second == "toto1");
			}
			else if (keyIndex == 2)
			{
				assert(keyValue.first == "tutu2");
				assert(keyValue.second == "toto2");
			}

			keyIndex++;
		}

	}
	assert(glo_allocatedByteCount == 0);


	system("pause");
	return 0;
};


