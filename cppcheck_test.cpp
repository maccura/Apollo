#include <iostream>
using std::cout;
using std::endl;

#include <string>

void foo(int x)
{
	int buf[10];
	
	buf[0] = 0;		//< -Error
		
	if (1000 == x)
	{
		//< tbd
	}
		
}


void f(int *p)
{
	*p = 3;	//< 
}

int main(void)
{
	int arr[10];
	arr[10] = 0;
	
	int x = 5, y, z;
	y = 3;
	
	z = x/y;
	
	int *p = nullptr;
	f(p);
	
	std::string str = "Hello,world";
	//char *pstr = (char *)str.c_str();
	char *pstr = const_cast<char *>(str.c_str());


#ifdef _WIN32
	system("pause");
#endif	
}






