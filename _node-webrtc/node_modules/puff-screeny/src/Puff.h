#ifndef _H_PUFF_
#define _H_PUFF_

class Puff
{
private:
	static Puff* instance;

public:
	static Puff* getInstance(nbind::cbFunction&, nbind::cbFunction&);

public:
	static Puff* getInternalInstance(void);

private:
	Puff(void) = delete;
	Puff(Puff&) = delete;
	explicit Puff(nbind::cbFunction&, nbind::cbFunction&);
	~Puff(void);

public:
	const std::vector<puff::type_helper::window_t*>& getAvailableWindows(void) const;
	const std::vector<puff::type_helper::screen_t*>& getAvailableScreens(void) const;

public:
	std::string encode(const std::vector<short>& data);

private:
	PuffImpl* impl;
	nbind::cbFunction stringConverter, bufferGenerator;
};

#endif