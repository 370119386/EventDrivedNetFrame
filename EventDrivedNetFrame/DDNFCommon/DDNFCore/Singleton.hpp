#ifndef __SINGLETON__H__
#define __SINGLETON__H__
namespace DDNF
{
	template <class T>
	class Singleton
	{
	protected:
		Singleton() {}

	private:
		Singleton(T& rhs) {}
		T& operator=(const T&) {return *this;}

		static T* m_pInstance;

	public:
		static T* Instance();
		static void Release();

	};

	template <class T>
	T* Singleton<T>::Instance()
	{
		if (0 == m_pInstance)
			m_pInstance = new T();

		return m_pInstance;
	}

	template <class T>
	void Singleton<T>::Release()
	{
		if (0 != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = 0;
		}
	}

	template <class T>
	T* Singleton<T>::m_pInstance = 0;
}
#endif//__SINGLETON__H__