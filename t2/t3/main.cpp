#include <iostream>
#include <stdio.h> 

using namespace std;


//����Point�ṹ�壨�ࣩ 
typedef struct tagPoint 
{ 
	int x;  
	int y; 
} Point; 
//�ӳ�int�쳣�ĺ��� 
static void f(int n) 
{ 
	throw 1; 
} 
//�ӳ�Point�쳣�ĺ��� 
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
		throw y;//�׳��쳣
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
		//���������
		//std::cout<<x<<"/"<<y<<"="<<Div(x,y)<<std::endl;
		//std::cout<<x1<<"/"<<y1<<"="<<Div(x1,y1)<<std::endl;
		//f(point); //�׳�Point�쳣 
		//ptr = new int[256];
	}
	catch(int i)//�쳣����
	{
		std::cout<<"����Ϊ0,�������" << i <<std::endl;//�쳣�������
	}
	catch(double i)//�쳣����
	{
		std::cout<<"����Ϊ0.0,�������"<< i <<std::endl;//�쳣�������
	}
	catch (Point e) 
	{ 
		printf("����Point�쳣:(%d,%d)\n", e.x, e.y); 
	}
	catch (bad_alloc &e)
	{
		//cout << "bad_alloc" << endl;
	}

	//01.try {  
	//	02.    //��������  
	//		03.    ptr = new int[num_max];  
	//	04.    //��������  
	//		05.} catch(bad_alloc &e) {  
	//			06.    //���ﳣ���Ĵ���ʽΪ�����ͷ��Ѿ�������ڴ棬Ȼ��������򣬻��ߴ�ӡһ��������Ϣ������ִ��  
	//				07.}  


	getchar();
	return 0;
}