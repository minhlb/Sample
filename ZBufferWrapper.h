/*
 * File:   ZBufferWrapper.h
 * Author: tintq
 *
 * Created on September 18, 2012, 6:43 PM
 */

#ifndef ZBUFFERWRAPPER_H
#define	ZBUFFERWRAPPER_H

class ZBufferWrapper {
public:

	ZBufferWrapper(uint8_t* buf, size_t sizeRemain)
	: _buf(buf), _sizeRemain(sizeRemain) {
	}

	virtual ~ZBufferWrapper() {
	}

public:

	size_t sizeRemain() const {
		return _sizeRemain;
	}

	void consume(uint32_t len) {
		if (len > _sizeRemain) len = _sizeRemain;
		if (len == 0) return;
		_buf += len;
		_sizeRemain -= len;
	}

	uint8_t* rawBuf(uint32_t& size) {
		size = _sizeRemain;
		return _buf;
	}

public: // write

	bool writeBool(bool val) {
		if (sizeof (val) > _sizeRemain) {
			return false;
		}
		*(bool*) _buf = val;
		_buf += sizeof (val);
		_sizeRemain -= sizeof (val);
		return true;
	}

	bool writeI8(int8_t val) {
		if (sizeof (val) > _sizeRemain) {
			return false;
		}
		*(int8_t*) _buf = val;
		_buf += sizeof (val);
		_sizeRemain -= sizeof (val);
		return true;
	}

	bool writeI16(int16_t val) {
		if (sizeof (val) > _sizeRemain) {
			return false;
		}
		*(int16_t*) _buf = val;
		_buf += sizeof (val);
		_sizeRemain -= sizeof (val);
		return true;
	}

	bool writeI32(int32_t val) {
		if (sizeof (val) > _sizeRemain) {
			return false;
		}
		*(int32_t*) _buf = val;
		_buf += sizeof (val);
		_sizeRemain -= sizeof (val);
		return true;
	}

	bool writeI64(int64_t val) {
		if (sizeof (val) > _sizeRemain) {
			return false;
		}
		*(int64_t*) _buf = val;
		_buf += sizeof (val);
		_sizeRemain -= sizeof (val);
		return true;
	}

	template <class _TString>
	bool writeStringS2(const _TString& val) {
		if (val.size() + sizeof (uint16_t) > _sizeRemain) {
			return false;
		}
		// write 2 bytes size of chars
		*(uint16_t*) _buf = val.size();
		_buf += sizeof (uint16_t);
		_sizeRemain -= sizeof (uint16_t);

		// write data
		if (val.size() > 0) {
			memcpy(_buf, val.data(), val.size());
			_buf += val.size();
			_sizeRemain -= val.size();
		}
		return true;
	}

	template <class _TString>
	bool writeStringS4(const _TString& val) {
		if (val.size() + sizeof (uint32_t) > _sizeRemain) {
			return false;
		}
		// write 4 bytes size of chars
		*(uint32_t*) _buf = val.size();
		_buf += sizeof (uint32_t);
		_sizeRemain -= sizeof (uint32_t);

		// write data
		if (val.size() > 0) {
			memcpy(_buf, val.data(), val.size());
			_buf += val.size();
			_sizeRemain -= val.size();
		}
		return true;
	}

	template <class _TList>
	bool writeListS2(const _TList& list) {
		if (list.size() + sizeof (uint32_t) > _sizeRemain) {
			return false;
		}
		typedef typename _TList::value_type ValueType;
		// write 2 bytes size of list
		*(uint16_t*) _buf = list.size();
		_buf += sizeof (uint16_t);
		_sizeRemain -= sizeof (uint16_t);

		// write data
		if (list.size() > 0) {
			typename _TList::const_iterator it = list.begin();
			typename _TList::const_iterator itEnd = list.end();
			for (; it != itEnd; ++it) {
				*(ValueType*) _buf = *it;
				_buf += sizeof (ValueType);
			}
			_sizeRemain -= list.size() * sizeof (ValueType);
		}
		return true;
	}

	template <class _TList>
	bool writeListS4(const _TList& list) {
		if (list.size() + sizeof (uint32_t) > _sizeRemain) {
			return false;
		}
		typedef typename _TList::value_type ValueType;
		// write 4 bytes size of list
		*(uint32_t*) _buf = list.size();
		_buf += sizeof (uint32_t);
		_sizeRemain -= sizeof (uint32_t);

		// write data
		if (list.size() > 0) {
			typename _TList::const_iterator it = list.begin();
			typename _TList::const_iterator itEnd = list.end();
			for (; it != itEnd; ++it) {
				*(ValueType*) _buf = *it;
				_buf += sizeof (ValueType);
			}
			_sizeRemain -= list.size() * sizeof (ValueType);
		}
		return true;
	}

	bool writeRawBuf(const uint8_t* rawBuf, uint32_t len) {
		if (len > _sizeRemain) {
			return false;
		}
		if (len > 0) {
			memcpy(_buf, rawBuf, len);
			_buf += len;
			_sizeRemain -= len;
		}
		return true;
	}

public: // read

	bool readBool(bool& val) {
		if (sizeof (val) > _sizeRemain) {
			return false;
		}
		val = *(bool*) _buf;
		_buf += sizeof (val);
		_sizeRemain -= sizeof (val);
		return true;
	}

	bool readI8(int8_t& val) {
		if (sizeof (val) > _sizeRemain) {
			return false;
		}
		val = *(int8_t*) _buf;
		_buf += sizeof (val);
		_sizeRemain -= sizeof (val);
		return true;
	}

	bool readI16(int16_t& val) {
		if (sizeof (val) > _sizeRemain) {
			return false;
		}
		val = *(int16_t*) _buf;
		_buf += sizeof (val);
		_sizeRemain -= sizeof (val);
		return true;
	}

	bool readI32(int32_t& val) {
		if (sizeof (val) > _sizeRemain) {
			return false;
		}
		val = *(int32_t*) _buf;
		_buf += sizeof (val);
		_sizeRemain -= sizeof (val);
		return true;
	}

	bool readI64(int64_t& val) {
		if (sizeof (val) > _sizeRemain) {
			return false;
		}
		val = *(int64_t*) _buf;
		_buf += sizeof (val);
		_sizeRemain -= sizeof (val);
		return true;
	}

	template <class _TString>
	bool readStringS2(_TString& val) {
		if (sizeof (uint16_t) > _sizeRemain) {
			return false;
		}
		// read 2 bytes size of chars
		uint16_t size = *(uint16_t*) _buf;
		_buf += sizeof (uint16_t);
		_sizeRemain -= sizeof (uint16_t);

		// buffer overflow
		if (size > _sizeRemain)
			return false;

		// read data
		if (size > 0) {
			val.assign((char*) _buf, size);
			_buf += size;
			_sizeRemain -= size;
		} else {
			val.clear();
		}
		return true;
	}

	template <class _TString>
	bool readStringS4(_TString& val) {
		if (sizeof (uint32_t) > _sizeRemain) {
			return false;
		}
		// read 4 bytes size of chars
		uint32_t size = *(uint32_t*) _buf;
		_buf += sizeof (uint32_t);
		_sizeRemain -= sizeof (uint32_t);

		// buffer overflow
		if (size > _sizeRemain)
			return false;

		// read data
		if (size > 0) {
			val.assign((char*) _buf, size);
			_buf += size;
			_sizeRemain -= size;
		} else {
			val.clear();
		}
		return true;
	}

	template <class _TList>
	bool readListS2(_TList& list) {
		if (sizeof (uint32_t) > _sizeRemain) {
			return false;
		}
		typedef typename _TList::value_type ValueType;
		// read 2 bytes size of list
		uint16_t size = *(uint16_t*) _buf;
		_buf += sizeof (uint16_t);
		_sizeRemain -= sizeof (uint16_t);

		// buffer overflow
		if (size * sizeof (ValueType) > _sizeRemain)
			return false;

		// read data
		if (size > 0) {
			for (uint32_t i = 0; i < size; ++i) {
				list.push_back(*(ValueType*) _buf);
				_buf += sizeof (ValueType);
			}
			_sizeRemain -= size * sizeof (ValueType);
		} else {
			list.clear();
		}
		return true;
	}

	template <class _TList>
	bool readListS4(_TList& list) {
		if (sizeof (uint32_t) > _sizeRemain) {
			return false;
		}
		typedef typename _TList::value_type ValueType;
		// read 4 bytes size of list
		uint32_t size = *(uint32_t*) _buf;
		_buf += sizeof (uint32_t);
		_sizeRemain -= sizeof (uint32_t);

		// buffer overflow
		if (size * sizeof (ValueType) > _sizeRemain)
			return false;

		// read data
		if (size > 0) {
			for (uint32_t i = 0; i < size; ++i) {
				list.push_back(*(ValueType*) _buf);
				_buf += sizeof (ValueType);
			}
			_sizeRemain -= size * sizeof (ValueType);
		} else {
			list.clear();
		}
		return true;
	}

	bool readRawBuf(uint8_t* rawBuf, uint32_t len) {
		if (len > _sizeRemain) {
			return false;
		}
		if (len > 0) {
			memcpy(rawBuf, _buf, len);
			_buf += len;
			_sizeRemain -= len;
		}
		return true;
	}

	const uint8_t* readRawBuf(uint32_t len) {
		if (len > _sizeRemain || len <= 0 || _sizeRemain <= 0)
			return NULL;
		uint8_t* ret = _buf;
		_buf += len;
		_sizeRemain -= len;
		return ret;
	}

private:
	uint8_t* _buf;
	size_t _sizeRemain;
};

#endif	/* ZBUFFERWRAPPER_H */

