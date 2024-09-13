#ifndef SINGLEINSTANCE_H_
#define SINGLEINSTANCE_H_
#include<memory>
#include<mutex>

template<typename _Ty>
class SingleInstance
{
public:
	virtual~SingleInstance(){}
	SingleInstance(const SingleInstance<_Ty>&) = delete;
	SingleInstance(SingleInstance<_Ty>&&) = delete;
	SingleInstance& operator=(const SingleInstance<_Ty>&) = delete;


	static std::shared_ptr<_Ty> instance()
	{
		static std::once_flag flag;
		std::call_once(flag, [&]() {
			m_instance = std::shared_ptr<_Ty>(new _Ty);
			});
		return m_instance;
	}


protected:
	SingleInstance(){}
	static std::shared_ptr<_Ty> m_instance;
};

template<typename _Ty>
std::shared_ptr<_Ty> SingleInstance<_Ty>::m_instance = nullptr;

#endif