#include "includes.h"

Puff* Puff::instance = nullptr;

Puff* Puff::getInstance(nbind::cbFunction& stringConverter, nbind::cbFunction& bufferGenerator)
{
	if (Puff::instance == nullptr)
		Puff::instance = new Puff(stringConverter, bufferGenerator);

	return Puff::instance;
}

Puff* Puff::getInternalInstance()
{
	return Puff::instance;
}

Puff::Puff(nbind::cbFunction& stringConverter, nbind::cbFunction& bufferGenerator)
	: impl(puff::type_helper::puff_impl_t::getInstance()), stringConverter(stringConverter), bufferGenerator(bufferGenerator) {}
Puff::~Puff(void) {}

const std::vector<puff::type_helper::window_t*>& Puff::getAvailableWindows() const
{
	return this->impl->getAvailableWindows();
}
const std::vector<puff::type_helper::screen_t*>& Puff::getAvailableScreens() const
{
	return this->impl->getAvailableScreens();
}

std::string Puff::encode(const std::vector<short>& data)
{
	return this->stringConverter.call<std::string>(data);
}

#include "nbind/nbind.h"

#if defined(BUILDING_NODE_EXTENSION)
NBIND_CLASS(Puff)
{
	method(getInstance);
	method(getAvailableWindows);
	method(getAvailableScreens);
}
#endif