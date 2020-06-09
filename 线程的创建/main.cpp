#include<iostream>
#include <thread>
#include <string>
using namespace std;


struct Student {
	string _name;
	int _id;

	Student(string name,int id):_name(name),_id(id){}
};
void thread_fun1() {
	while (1) {
		cout << "thread_fun1 created! -1 " << endl;
		cout << "thread_fun1 created! -2" << endl;
		cout << "thread_fun1 created! -3" << endl;
		cout << "thread_fun1 created! -4" << endl;
		cout << "thread_fun1 created! -5" << endl;
		this_thread::sleep_for(chrono::seconds(1));
	}
}
void thread_fun2() {
	while (1) {
		cout << "thread_fun2 created! -1" << endl;
		cout << "thread_fun2 created! -2" << endl;
		cout << "thread_fun2 created! -3" << endl;
		cout << "thread_fun2 created! -4" << endl;
		cout << "thread_fun2 created! -5" << endl;
		this_thread::sleep_for(chrono::seconds(1));
	}
		
}
//带参线程
void thread_fun3(const char* what) {
	while (1) {
		cout << "I'm " << what << endl;
		this_thread::sleep_for(chrono::seconds(1));
	}
}
//带有复合参数的线程
void thread_fun4(const Student* s) {
	while (1) {
		cout << "name: " << s->_name << endl;
		cout << "ID: " << s->_id << endl;
		this_thread::sleep_for(chrono::seconds(1));
	}
}
//通过一个函数对象——即仿函数（functor）——创建线程。
class Hello {
public:
	void operator()(const char* what) {
		while (1) {
			cout << "I'm Hello->" << what << endl;
			this_thread::sleep_for(chrono::seconds(1));
		}
		
	}
};

//通过一个成员函数创建线程。 与前例不同之处在于，需要以 bind 绑定 this 指针作为第一个参数。
class World {
public:
	World() {
		thread t(std::bind(&World::Entry, this, "456"));
		t.join();

	}
private:
	void Entry(const char* what) {
		cout << "Entry created. what->" << what << endl;
	}
};

//创建两个线程，各自倒着计数。 此例顺带演示了 detached 线程，被 detached 的线程，自生自灭，不受控制，无法再 join。
class Counter {
public:
	Counter(){}
	Counter(int value):_value(value),isEnd(false){}
	int operator()() {
		while (_value > 0) {
			cout << "_value = " << _value << endl;
			_value--;
			this_thread::sleep_for(chrono::seconds(1));
		}
		cout << "Counter线程结束了" << endl;
		isEnd = true;
		return 1;
	}
private:
	int _value;		//计数值
	bool isEnd;		//是否结束
};

//测试两个无参线程
void test01() {
	thread t1(&thread_fun1);
	thread t2(&thread_fun2);
	t1.join();
	t2.join();
}
//测试有参线程
void test02() {
	thread t3(&thread_fun3, "jubowen");
	thread t4(&thread_fun4, &Student("jubowen", 161713140));
	t3.join();
	t4.join();
}
//测试通过仿函数 和 成员函数创建的线程
void test03() {
	Hello hello;
	thread t4(hello, "123");
	World world;
}
//测试线程分离
void test04() {
	thread t5(Counter(3));
	t5.join();

	thread t6(Counter(3));
	t6.detach();		//主线程结束了，t6次线程还没结束

}
int main()
{
	test02();
	//test04();
	


	
	
	system("pause");
	return 0;
}