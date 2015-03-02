#ifndef _OBJECT_CACHED_POOL_H_
#define _OBJECT_CACHED_POOL_H_

#include <map>
#include <Windows.h>

template <class Key, class Obj>
class ObjectPool
{
public:
	ObjectPool()
	{
		m_pContext = nullptr;
	}
	virtual ~ObjectPool()
	{
		//Clear();
	}

	void Initialize(LPVOID pContext)
	{
		m_pContext = pContext;
	}

	Obj GetObject(Key key)
	{
		auto itr = m_objPool.find(key);
		Obj obj = nullptr;
		if (itr == m_objPool.end())
		{
			obj = CreateNewObj(key);
			m_objPool[key] = obj;
		}
		else
		{
			obj = itr->second;
		}

		return obj;
	}

	void Clear()
	{
		for (auto &&obj : m_objPool)
		{
			DestroyObj(obj.second);
		}
	}


protected:
	virtual Obj CreateNewObj(Key key) = 0;
	virtual void DestroyObj(Obj obj) = 0;
	std::map<Key, Obj> m_objPool;
	LPVOID m_pContext;
};

#endif //_OBJECT_CACHED_POOL_H_

