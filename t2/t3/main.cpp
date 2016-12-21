#include <iostream>
#include <stdio.h> 

using namespace std;


//定义Point结构体（类） 
typedef struct tagPoint 
{ 
	int x;  
	int y; 
} Point; 
//扔出int异常的函数 
static void f(int n) 
{ 
	throw 1; 
} 
//扔出Point异常的函数 
static void f(Point point) 
{ 
	Point p; 
	p.x = 1; 
	p.y = 2; 
	throw p; 
} 

template <typename T>
T Div(T x, T y)
{
	if(y == 0)
		throw y;//抛出异常
	return x/y;
}

int main()
{
	int x=5,y=0;
	double x1=5.5,y1=0.0;
	Point point; 
	point.x = 0; 
	point.y = 0;
	int * ptr = 0;
	try
	{
		//被检查的语句
		//std::cout<<x<<"/"<<y<<"="<<Div(x,y)<<std::endl;
		//std::cout<<x1<<"/"<<y1<<"="<<Div(x1,y1)<<std::endl;
		//f(point); //抛出Point异常 
		//ptr = new int[256];
	}
	catch(int i)//异常类型
	{
		std::cout<<"除数为0,计算错误！" << i <<std::endl;//异常处理语句
	}
	catch(double i)//异常类型
	{
		std::cout<<"除数为0.0,计算错误！"<< i <<std::endl;//异常处理语句
	}
	catch (Point e) 
	{ 
		printf("捕获到Point异常:(%d,%d)\n", e.x, e.y); 
	}
	catch (bad_alloc &e)
	{
		//cout << "bad_alloc" << endl;
	}

	//01.try {  
	//	02.    //其他代码  
	//		03.    ptr = new int[num_max];  
	//	04.    //其他代码  
	//		05.} catch(bad_alloc &e) {  
	//			06.    //这里常见的处理方式为：先释放已经分配的内存，然后结束程序，或者打印一条错误信息并继续执行  
	//				07.}  


	getchar();
	return 0;
}