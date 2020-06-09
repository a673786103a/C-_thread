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


/******************ʹ�� lock_guard �Զ�������������ԭ���� RAII��������ָ������ -bgein***************************/
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
//�����в��߳�
void test02() {
	Student s1("jubowen", "p161713140",22);
	Student s2("xbmu", "1234456",70);
	thread t1(&thread_fun4, &s1);
	thread t2(&thread_fun4, &s2);
	//thread t2(&thread_fun4, &Student("xbmu", "1234567",70));		//���������string Ϊʲô��ôд��ӡ��������
	t1.join();
	t2.join();
}
/******************ʹ�� lock_guard �Զ�������������ԭ���� RAII��������ָ������ -end***************************/




/*****************ֱ�Ӳ���mutex ��ֱ�ӵ��� mutex �� lock / unlock ���� -begin*****************************/
void counter() {

	g_mutex.lock();	//�ӻ�����
	cout << "cnt = " << cnt++ << endl;
	g_mutex.unlock();

	/*
	��������������  �ɼ�cnt = 1�е�1��û�����أ����л�����һ���߳���,��˼򵥵�һ�����Ӷ��޷���֤������ȷ��
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
		v.emplace_back(&counter);		//������push_back
	}
	for (thread& i : v)
		i.join();
}
/*****************ֱ�Ӳ���mutex ��ֱ�ӵ��� mutex �� lock / unlock ���� -end*****************************/


int main()
{
	test02();

	system("pause");
	return 0;
}