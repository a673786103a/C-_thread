#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>


/*
条件变量是利用线程间共享的全局变量进行同步的一种机制，主要包括两个动作：
一个线程等待"条件变量的条件成立"而挂起；另一个线程使"条件成立"（给出条件成立信号）。
为了防止竞争，条件变量的使用总是和一个互斥锁结合在一起。
*/
std::deque<int> q;
std::mutex mutex;
std::mutex mu;
std::condition_variable cv;	//条件变量
std::condition_variable cond;
std::string data;
bool ready = false;  // 条件
bool processed = false;  // 条件

/***************************--begin************************************************/
void Worker() {
	std::unique_lock<std::mutex> lock(mutex);

	// 等待主线程发送数据。
	cv.wait(lock, [] { return ready; });

	// 等待后，继续拥有锁。
	std::cout << "工作线程正在处理数据..." << std::endl;
	// 睡眠一秒以模拟数据处理。
	std::this_thread::sleep_for(std::chrono::seconds(1));
	data += " 已处理";

	// 把数据发回主线程。
	processed = true;
	std::cout << "工作线程通知数据已经处理完毕。" << std::endl;

	// 通知前，手动解锁以防正在等待的线程被唤醒后又立即被阻塞。
	lock.unlock();

	cv.notify_one();
}
//第一个例子
void test01() {
	std::thread worker(Worker);

	// 把数据发送给工作线程。
	{
		std::lock_guard<std::mutex> lock(mutex);
		std::cout << "主线程正在准备数据..." << std::endl;
		// 睡眠一秒以模拟数据准备。
		std::this_thread::sleep_for(std::chrono::seconds(1));
		data = "样本数据";
		ready = true;
		std::cout << "主线程通知数据已经准备完毕。" << std::endl;
	}
	cv.notify_one();

	// 等待工作线程处理数据。
	{
		std::unique_lock<std::mutex> lock(mutex);
		cv.wait(lock, [] { return processed; });
	}
	std::cout << "回到主线程，数据 = " << data << std::endl;

	worker.join();

}
/***************************--end************************************************/







/***************************生产者-消费者begin********************************/
//生产者
void function_1() {
	int count = 10;
	while (count > 0) {
		std::unique_lock<std::mutex> locker(mu);
		q.push_front(count);
		locker.unlock();	//不明白这里为什么要解锁？
		cond.notify_one();  // Notify one waiting thread, if there is one.
		std::this_thread::sleep_for(std::chrono::seconds(1));
		count--;
	}
}
//消费者
void function_2() {
	int data = 0;
	while (data != 1) {
		std::unique_lock<std::mutex> locker(mu);
		while (q.empty())
			cond.wait(locker); // Unlock mu and wait to be notified在判断队列是否为空的时候，使用的是while(q.empty())，而不是if(q.empty())，这是因为wait()从阻塞到返回，不一定就是由于notify_one()函数造成的，还有可能由于系统的不确定原因唤醒（可能和条件变量的实现机制有关），这个的时机和频率都是不确定的，被称作伪唤醒，如果在错误的时候被唤醒了，执行后面的语句就会错误，所以需要再次判断队列是否为空，如果还是为空，就继续wait()阻塞。
								// 这个wait被调用的时候是否会自动调用locker.lock()?  如果会，那么是不是加了两次锁？
		data = q.back();
		q.pop_back();
		locker.unlock();		//这里为什么要解锁？ -- Once notified (explicitly, by some other thread), the function unblocks and calls lck.lock()		
		std::cout << "t2 got a value from t1: " << data << std::endl;
	}
}
void test02() {
	std::thread t1(function_1);
	std::thread t2(function_2);
	t1.join();
	t2.join();
}
/***************************生产者-消费者end**********************************/






/**************************wait函数 -begin**************************************/
int cargo = 0;
std::mutex mtx;
bool shipment_available() { return cargo != 0; }

void consume(int n) {
	for (int i = 0; i < n; ++i) {
		std::unique_lock<std::mutex> lck(mtx);
		cv.wait(lck, shipment_available);
		// consume:
		std::cout << cargo << '\n';
		cargo = 0;			//可能是为了让自己阻塞？
		lck.unlock();		//这句话例子里没有，是我自己加的
	}
}

void test03()
{
	std::thread consumer_thread(consume, 10);

	// produce 10 items when needed:
	for (int i = 0; i < 10; ++i) {
		while (shipment_available()) 
			std::this_thread::yield();
		std::unique_lock<std::mutex> lck(mtx);
		cargo = i + 1;
		cv.notify_one();
	}

	consumer_thread.join();

}
/**************************wait函数 -end**************************************/










/**************************notify_one函数 -begin**************************************/
std::mutex mtx_notify_one;
std::condition_variable produce, cconsume;		//两个条件变量
void consumer() {
	std::unique_lock<std::mutex> lck(mtx_notify_one);
	while (cargo == 0) 
		cconsume.wait(lck);
	std::cout << cargo << '\n';
	cargo = 0;		//下一次阻塞自己
	produce.notify_one();
}

void producer(int id) {
	std::unique_lock<std::mutex> lck(mtx_notify_one);
	while (cargo != 0) produce.wait(lck);
	cargo = id;
	cconsume.notify_one();
}
void test04() {
	std::thread consumers[10], producers[10];
	// spawn 10 consumers and 10 producers:
	for (int i = 0; i < 10; ++i) {
		consumers[i] = std::thread(consumer);
		producers[i] = std::thread(producer, i + 1);
	}

	// join them back:
	for (int i = 0; i < 10; ++i) {
		producers[i].join();
		consumers[i].join();
	}
}
/**************************notify_one函数 -end**************************************/








int main() {
	
	//test02();
	//test03();
	test04();
	system("pause");
	return 0;
}