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
//�����߳�
void thread_fun3(const char* what) {
	while (1) {
		cout << "I'm " << what << endl;
		this_thread::sleep_for(chrono::seconds(1));
	}
}
//���и��ϲ������߳�
void thread_fun4(const Student* s) {
	while (1) {
		cout << "name: " << s->_name << endl;
		cout << "ID: " << s->_id << endl;
		this_thread::sleep_for(chrono::seconds(1));
	}
}
//ͨ��һ���������󡪡����º�����functor�����������̡߳�
class Hello {
public:
	void operator()(const char* what) {
		while (1) {
			cout << "I'm Hello->" << what << endl;
			this_thread::sleep_for(chrono::seconds(1));
		}
		
	}
};

//ͨ��һ����Ա���������̡߳� ��ǰ����֮ͬ�����ڣ���Ҫ�� bind �� this ָ����Ϊ��һ��������
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

//���������̣߳����Ե��ż����� ����˳����ʾ�� detached �̣߳��� detached ���̣߳��������𣬲��ܿ��ƣ��޷��� join��
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
		cout << "Counter�߳̽�����" << endl;
		isEnd = true;
		return 1;
	}
private:
	int _value;		//����ֵ
	bool isEnd;		//�Ƿ����
};

//���������޲��߳�
void test01() {
	thread t1(&thread_fun1);
	thread t2(&thread_fun2);
	t1.join();
	t2.join();
}
//�����в��߳�
void test02() {
	thread t3(&thread_fun3, "jubowen");
	thread t4(&thread_fun4, &Student("jubowen", 161713140));
	t3.join();
	t4.join();
}
//����ͨ���º��� �� ��Ա�����������߳�
void test03() {
	Hello hello;
	thread t4(hello, "123");
	World world;
}
//�����̷߳���
void test04() {
	thread t5(Counter(3));
	t5.join();

	thread t6(Counter(3));
	t6.detach();		//���߳̽����ˣ�t6���̻߳�û����

}
int main()
{
	test02();
	//test04();
	


	
	
	system("pause");
	return 0;
}