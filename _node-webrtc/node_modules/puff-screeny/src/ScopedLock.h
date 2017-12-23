#ifndef _H_SCOPEDLOCK_
#define _H_SCOPEDLOCK_

class ScopedLock
{
public:
	ScopedLock(void) = delete;
	ScopedLock(ScopedLock&) = delete; 
	ScopedLock(uv_mutex_t& mutex, bool ___try = false) :
		mutex(mutex), locked(!___try)
	{
		if (___try == true)
			this->locked = (uv_mutex_trylock(&mutex) == 0);
		else
			uv_mutex_lock(&mutex);
	}
	~ScopedLock(void)
	{
		if (this->locked)
			uv_mutex_unlock(&mutex);
	}

private:
	uv_mutex_t& mutex;
	bool locked;
};

#define SCOPED_LOCK(x) ScopedLock __##x__(x);
#define SCOPED_TRYLOCK(x) ScopedLock __##x__(x, true);

#endif
