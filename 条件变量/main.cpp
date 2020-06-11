#include <iostream>
#include <string>
#include <thread>
#include <mutex>
#include <deque>
#include <condition_variable>


/*
���������������̼߳乲���ȫ�ֱ�������ͬ����һ�ֻ��ƣ���Ҫ��������������
һ���̵߳ȴ�"������������������"��������һ���߳�ʹ"��������"���������������źţ���
Ϊ�˷�ֹ����������������ʹ�����Ǻ�һ�������������һ��
*/
std::deque<int> q;
std::mutex mutex;
std::mutex mu;
std::condition_variable cv;	//��������
std::condition_variable cond;
std::string data;
bool ready = false;  // ����
bool processed = false;  // ����

/***************************--begin************************************************/
void Worker() {
	std::unique_lock<std::mutex> lock(mutex);

	// �ȴ����̷߳������ݡ�
	cv.wait(lock, [] { return ready; });

	// �ȴ��󣬼���ӵ������
	std::cout << "�����߳����ڴ�������..." << std::endl;
	// ˯��һ����ģ�����ݴ���
	std::this_thread::sleep_for(std::chrono::seconds(1));
	data += " �Ѵ���";

	// �����ݷ������̡߳�
	processed = true;
	std::cout << "�����߳�֪ͨ�����Ѿ�������ϡ�" << std::endl;

	// ֪ͨǰ���ֶ������Է����ڵȴ����̱߳����Ѻ���������������
	lock.unlock();

	cv.notify_one();
}
//��һ������
void test01() {
	std::thread worker(Worker);

	// �����ݷ��͸������̡߳�
	{
		std::lock_guard<std::mutex> lock(mutex);
		std::cout << "���߳�����׼������..." << std::endl;
		// ˯��һ����ģ������׼����
		std::this_thread::sleep_for(std::chrono::seconds(1));
		data = "��������";
		ready = true;
		std::cout << "���߳�֪ͨ�����Ѿ�׼����ϡ�" << std::endl;
	}
	cv.notify_one();

	// �ȴ������̴߳������ݡ�
	{
		std::unique_lock<std::mutex> lock(mutex);
		cv.wait(lock, [] { return processed; });
	}
	std::cout << "�ص����̣߳����� = " << data << std::endl;

	worker.join();

}
/***************************--end************************************************/







/***************************������-������begin********************************/
//������
void function_1() {
	int count = 10;
	while (count > 0) {
		std::unique_lock<std::mutex> locker(mu);
		q.push_front(count);
		locker.unlock();	//����������ΪʲôҪ������
		cond.notify_one();  // Notify one waiting thread, if there is one.
		std::this_thread::sleep_for(std::chrono::seconds(1));
		count--;
	}
}
//������
void function_2() {
	int data = 0;
	while (data != 1) {
		std::unique_lock<std::mutex> locker(mu);
		while (q.empty())
			cond.wait(locker); // Unlock mu and wait to be notified���ж϶����Ƿ�Ϊ�յ�ʱ��ʹ�õ���while(q.empty())��������if(q.empty())��������Ϊwait()�����������أ���һ����������notify_one()������ɵģ����п�������ϵͳ�Ĳ�ȷ��ԭ���ѣ����ܺ�����������ʵ�ֻ����йأ��������ʱ����Ƶ�ʶ��ǲ�ȷ���ģ�������α���ѣ�����ڴ����ʱ�򱻻����ˣ�ִ�к�������ͻ����������Ҫ�ٴ��ж϶����Ƿ�Ϊ�գ��������Ϊ�գ��ͼ���wait()������
								// ���wait�����õ�ʱ���Ƿ���Զ�����locker.lock()?  ����ᣬ��ô�ǲ��Ǽ�����������
		data = q.back();
		q.pop_back();
		locker.unlock();		//����ΪʲôҪ������ -- Once notified (explicitly, by some other thread), the function unblocks and calls lck.lock()		
		std::cout << "t2 got a value from t1: " << data << std::endl;
	}
}
void test02() {
	std::thread t1(function_1);
	std::thread t2(function_2);
	t1.join();
	t2.join();
}
/***************************������-������end**********************************/






/**************************wait���� -begin**************************************/
int cargo = 0;
std::mutex mtx;
bool shipment_available() { return cargo != 0; }

void consume(int n) {
	for (int i = 0; i < n; ++i) {
		std::unique_lock<std::mutex> lck(mtx);
		cv.wait(lck, shipment_available);
		// consume:
		std::cout << cargo << '\n';
		cargo = 0;			//������Ϊ�����Լ�������
		lck.unlock();		//��仰������û�У������Լ��ӵ�
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
/**************************wait���� -end**************************************/










/**************************notify_one���� -begin**************************************/
std::mutex mtx_notify_one;
std::condition_variable produce, cconsume;		//������������
void consumer() {
	std::unique_lock<std::mutex> lck(mtx_notify_one);
	while (cargo == 0) 
		cconsume.wait(lck);
	std::cout << cargo << '\n';
	cargo = 0;		//��һ�������Լ�
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
/**************************notify_one���� -end**************************************/








int main() {
	
	//test02();
	//test03();
	test04();
	system("pause");
	return 0;
}