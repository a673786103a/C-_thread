#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <string>
#include <fstream>
using namespace std;

int cnt = 0;
mutex g_mutex;

class Student {
public:
	const char* _name;
	const char*  _id;
	int _age;

	Student(const char*  name, const char*  id,int age) :_name(name), _id(id),_age(age) {}
};


/******************使用 lock_guard 自动加锁、解锁。原理是 RAII，和智能指针类似 -bgein***************************/
void thread_fun4(const Student* s) {
	ofstream ofs("./file.txt", ios::app);
	while (1) {
		//lock_guard<mutex> lk(g_mutex);	
		unique_lock<mutex> lk(g_mutex);
		ofs << "name: " << s->_name << endl;
		ofs << "ID: " << s->_id << endl;
		ofs << "age: " << s->_age << endl << endl;
		//this_thread::sleep_for(chrono::seconds(1));
	}
}
//测试有参线程
void test02() {
	Student s1("jubowen", "p161713140",22);
	Student s2("xbmu", "1234456",70);
	thread t1(&thread_fun4, &s1);
	thread t2(&thread_fun4, &s2);
	//thread t2(&thread_fun4, &Student("xbmu", "1234567",70));		//如果类型是string 为什么这么写打印不出来？
	t1.join();
	t2.join();
}
/******************使用 lock_guard 自动加锁、解锁。原理是 RAII，和智能指针类似 -end***************************/




/*****************直接操作mutex 即直接调用 mutex 的 lock / unlock 函数 -begin*****************************/
void counter() {

	g_mutex.lock();	//加互斥锁
	cout << "cnt = " << cnt++ << endl;
	g_mutex.unlock();

	/*
	不加锁输出结果：  可见cnt = 1中的1还没出来呢，就切换到另一个线程了,如此简单的一个例子都无法保证数据正确性
	cnt = 0cnt = cnt = 2
	1

	cnt = 3

	*/
}
void test01() {
	const int size = 4;
	vector<thread> v;
	v.reserve(4);

	for (int i = 0; i < size; i++) {
		v.emplace_back(&counter);		//不能用push_back
	}
	for (thread& i : v)
		i.join();
}
/*****************直接操作mutex 即直接调用 mutex 的 lock / unlock 函数 -end*****************************/


int main()
{
	test02();

	system("pause");
	return 0;
}