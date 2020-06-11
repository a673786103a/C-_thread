#include <iostream>
#include <condition_variable>
#include <vector>
#include <mutex>
#include <thread>
using namespace std;

condition_variable full, _empty;
mutex g_mutex;

class Produe_Consume {
private:
	int _begin;
	int _end;
	int _buffer_size;
	int _current_cnt;
	vector<int> _buffer;
public:
	Produe_Consume(int buffer_size)
		:_begin(0), _end(0), _current_cnt(0), _buffer_size(buffer_size), _buffer(buffer_size) {}
	//生产
	void produce(int n) {
		unique_lock<mutex> lk(g_mutex);
		//如果没有空位置了，等待
		while (_current_cnt > _buffer_size)
			_empty.wait(lk);
		//_empty.wait(lk, [=] {return _current_cnt <_buffer_size; });
		_buffer[_end] = n;
		_end = (_end+1) % _buffer_size;
		_current_cnt++;
		//先通知还是先解锁？
		lk.unlock();
		full.notify_one();
	}
	//消费
	int consume() {
		unique_lock<mutex> lk(g_mutex);
		while (_current_cnt < 0)
			full.wait(lk);
		//full.wait(lk, [=] {return _current_cnt > 0; });
		int n = _buffer[_begin];
		_begin = (_begin + 1) % _buffer_size;
		_current_cnt--;
		lk.unlock();
		_empty.notify_one();
		
		return n;
	}
};
mutex mtx;
Produe_Consume pc(10);	//能装5个数据   
int n = 30;				//生产1-30
void produce() {
	
	for (int i = 1; i <= n; i++) {
		mtx.lock();				//能不能在这里加一个锁？
		pc.produce(i);
		//unique_lock<mutex> lk(mutex);
		cout << "produce a data: " << i << endl;
		mtx.unlock();
	}
}
void consume() {
	for (int i = 1; i <= n; i++) {
		mtx.lock();
		int x = pc.consume();
		//unique_lock<mutex> lk(mutex);
		cout << "consume a data: " << x << endl;
		mtx.unlock();
	}
	
}
int main()
{
	vector<thread> threads;
	threads.reserve(2);
	threads.push_back(thread(&produce));
	threads.push_back(thread(&consume));
	for (auto& t : threads)
		t.join();

	system("pause");
	return 0;
}