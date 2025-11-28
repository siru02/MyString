#include <iostream>
#include <memory>
#include <algorithm>

using namespace std;

class MyString {
private:
	char* data; // 실제 문자 데이터
	size_t size; // 실제 문자열 길이
	size_t capacity; // data에 할당된 메모리 크기

	// 스마트포인터버전은 나중에 구현해보자
	//unique_ptr<char[]> data2; // 객체를 가리키는 포인터가 유일함 -> unique_ptr이 범위를 벗어나면 delete 호출
	//shared_ptr<char[]> data3; // 객체를 가리키는 포인터가 여러개 -> cnt가 0이 되면 delete 호출

public:
	MyString();
	~MyString();
	MyString& operator=(const MyString& ref);
	MyString& operator=(MyString ref); // 일반 인자로 받아서 대입연산을 수행하면 swap호출
	MyString(const MyString& ref);
	MyString(const char ch);
	MyString(const char* str);

	// 문자열 뒤에 다른 문자열 붙이기
	void append(const char ch);
	void append(const char* str);
	void append(const MyString& str);

	// 문자열 내에 포함되어 있는 문자열 구하기
	int find(const MyString& str) const; // 이거 하나만 있으면 아래 둘은 필요없긴함
	int find(const char ch) const;
	int find(const char* str) const;

	// 문자열이 같은지 비교
	int compare(char ch) const;
	int compare(const char* str) const;
	int compare(const MyString& ref) const;
	bool operator==(const MyString& ref) const;

	// 문자열 크기 비교(사전 순)
	bool operator>(const MyString& ref) const;
	bool operator>=(const MyString& ref) const;
	bool operator<(const MyString& ref) const;
	bool operator<=(const MyString& ref) const;

	// 유틸함수들
	int length() const; // 길이 반환
	MyString& insert(int loc, const MyString& str); // loc에 삽입
	//MyString& insert(int loc, const char* str); MyString객체에 대한 함수가 있으므로 묵시적 형변환으로 가능
	//MyString& insert(int loc, char ch); MyString객체에 대한 함수가 있으므로 묵시적 형변환으로 가능
	MyString& assign(const MyString& str); //할당
	MyString& erase(int loc, int num);
	//reserve

	//capacity
};

MyString::MyString() : size(0), capacity(1), data(new char[1]){
	//data = new char[capacity];
	data[0] = '\0';
}

MyString::~MyString() {
	delete[] data;
}

// const ref객체를 인자로 받는 경우 -> 학습용으로 그동안 구현한 버전
MyString& MyString::operator=(const MyString& ref) {
	if (this == &ref) return *this; // 자기 자신을 대입하는 경우

	if (capacity < ref.size + 1) { // 재할당필요한 경우
		delete[] data;
		capacity = ref.capacity;
		data = new char[capacity];
	}
	size = ref.size;
	for (size_t i = 0; i < size; ++i) {
		data[i] = ref.data[i];
	}
	data[size] = '\0';
	return *this;
}

// 일반 객체를 인자로 받도록 하는 경우에는 swap을 이용한다
MyString& MyString::operator=(MyString ref) {
	swap(*this, ref); //ref는 스택변수이므로 여기의 값을 *this에 넣는다
	// -> shallow copy수행
	return *this;
}

MyString::MyString(const MyString& ref):size(ref.size),capacity(ref.capacity),data(new char[ref.capacity]) {
	//*this = ref; //이렇게 하면 operator= 동장에서 생성되지 않은 this의 쓰레기값과 ref가 비교
	for (size_t i = 0; i < size; ++i) {
		data[i] = ref.data[i];
	}
	data[size] = '\0';
}

MyString::MyString(char ch):size(1),capacity(2),data(new char[capacity]) {
	data[0] = ch;
	data[1] = '\0';
}

MyString::MyString(const char* str):size(strlen(str)),capacity(size + 1),data(new char[capacity]) {
	for (size_t i = 0; i < size; ++i) {
		data[i] = str[i];
	}
	data[size] = '\0';
}

// 비교해서 사전순으로 뒤에오면 1반환
int MyString::compare(char ch) const {
	if (size == 0) return -1;
	if (data[0] == ch) {
		if (size == 1) return 0;
		return 1;
	}
	if (data[0] > ch)	return 1; // MyString이 사전순으로 뒤에 오는 경우
	return -1; //MyString이 사전순으로 앞에 오는 경우
}
int MyString::compare(const char* str) const {
	if (str == nullptr) return 1; // 이걸 따로 예외처리해야함
	size_t argStrLen = strlen(str);
	size_t minLen = min(size, argStrLen); // 두 문자열중 작은 길이를 선정

	// 범위에 맞게 순회하며 사전순으로 맞지 않는게 있는지 찾음
	for (size_t i = 0; i < minLen; ++i) {
		if (data[i] > str[i]) return 1; // 현 문자열이 사전순으로 뒤에 오는 경우
		if (data[i] < str[i]) return -1; // 현 문자열이 사전순으로 앞에 오는 경우
	}
	if (size > argStrLen) return 1;
	if (size < argStrLen) return -1;
	return 0; //같은 string임
}
int MyString::compare(const MyString& ref) const {
	size_t minLen = min(size, ref.size);

	// 내용 비교
	for (size_t i = 0; i < minLen; ++i) {
		if (data[i] < ref.data[i]) return -1;
		if (data[i] > ref.data[i]) return 1;
	}

	if (size > ref.size) return 1; // 내 문자가 더 긴 경우
	if (size < ref.size) return -1; // 내 문자가 더 짧은 경우
	return 0;
}

// 같은 객체인지를 비교하는게 아닌, 같은 문자열인지 비교
bool MyString::operator==(const MyString& ref) const {
	return compare(ref) == 0;
}

//util함수들 -> 함수뒤에 const를 붙여서 const함수로 정의해야 const객체와 일반객체 모두에서 사용가능
// 문자열 길이를 구하는 함수
int MyString::length() const {
	return size;
}

// 문자열 뒤에 다른 문자열 붙이기
void MyString::append(const char ch) {
	if (capacity < size + 2) {
		size_t newCapacity = std::max(capacity * 2, size + 2); // 빈 문자열의 경우도 있으므로
		char* newData = new char[newCapacity];
		for (size_t i = 0; i < size; ++i) {
			newData[i] = data[i];
		}
		delete[] data;

		capacity = newCapacity;
		data = newData;
	}

	data[size] = ch;
	size += 1;
	data[size] = '\0';
	return;
}
void MyString::append(const char* str) {
	if (str == nullptr) return;
	size_t len = strlen(str);
	if (capacity < size + len + 1) { // size + len + 1 이상이면 수용가능하므로
		size_t newCapacity = std::max(capacity * 2, size + len + 1);
		char* newData = new char[newCapacity];
		for (size_t i = 0; i < size; ++i) { // 기존 데이터 복사
			newData[i] = data[i];
		}
		delete[] data;

		data = newData; // 새로 할당한 데이터로 교체
		capacity = newCapacity; // 용량 갱신
	}

	for (size_t i = 0; i < len; ++i) {
		data[size + i] = str[i];
	}
	size += len; //size갱신
	data[size] = '\0';
	return;
}
void MyString::append(const MyString& str) {
	if (str.size == 0) return;

	// self append 방지
	if (this == &str) {
		MyString temp(str);
		return append(temp); //double free가 발생할 수 있으므로 임시객체를 생성하여 인자로
	}

	if (size + str.size >= capacity) {
		size_t newCapacity = std::max(capacity * 2, size + str.size + 1);
		char* newData = new char[newCapacity];
		for (size_t i = 0; i < size; ++i) {
			newData[i] = data[i];
		}

		delete[] data;
		data = newData;
		capacity = newCapacity;
	}

	// 이어붙이기
	for (size_t i = 0; i < str.size; ++i) {
		data[size + i] = str.data[i];
	}
	size += str.size;
	data[size] = '\0';
	return;
}

// 문자열 내에 포함되어 있는 문자열의 시작 index를 반환
int MyString::find(const char ch) const {
	for (size_t i = 0; i < size; ++i) {
		if (data[i] == ch)
			return i;
	}
	return string::npos;
}
int MyString::find(const char* str) const {
	if (str == nullptr) return string::npos; // 빈 문자열 넣은경우

	size_t targetLen = strlen(str);
	if (targetLen == 0) return 0;

	if (targetLen > size) return string::npos; // 찾으려는 문자열이 더 큰 경우
	//MyString newStr{ str }; // 생성자로 비교 문자열 생성

	for (size_t i = 0; i < size - targetLen + 1; ++i) {
		size_t j = 0;
		for (; j < targetLen; ++j) {
			if (data[i + j] != str[j])
				break;
		}
		if (j == targetLen)
			return i;
		/*if (data[i] == str[0]) {
			/*
			* operator==을 오버로딩했으므로 MyString객체로 생성해서 비교가 가능
			if (MyString(data + i) == str) // MyString(data + i) == newStr과 같은 동작이 일어난다
			// lvalue는 MyString객체이고, operator==의 인자는 MyString인데 rvalue가 MyString객체가 아니므로 이에 대해 암시적 변환이 일어난다.
				return i;
		}*/
	}
	return string::npos;
}
int MyString::find(const MyString& str) const {
	if (str.data[0] == '\0') return 0;
	if (size < str.size) return string::npos; //못찾음
	
	// index 1부터 비교 -> 너 잘했을 듯
	for (size_t i = 0; i < size - str.size + 1; ++i) { // size - str.size는 문자열 개수 차이, +1을 해야 nullptr까지 구분
		size_t j = 0;
		for (; j < str.size; ++j) {
			if (data[i + j] != str.data[j])
				break;
		}
		if (j == str.size)	return i;
	}
	return string::npos;
}

// 문자열 크기 비교(사전 순)
bool MyString::operator<(const MyString& ref) const {
	return compare(ref) < 0;
}
bool MyString::operator<=(const MyString& ref) const {
	return compare(ref) <= 0;
}
bool MyString::operator>(const MyString& ref) const {
	return compare(ref) > 0;
}
bool MyString::operator>=(const MyString& ref) const {
	return compare(ref) >= 0;
}