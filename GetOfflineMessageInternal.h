/*
 * File:   GetOfflineMessageInternal.h
 * Author: tintq
 *
 * Created on April 26, 2013, 2:32 PM
 */

#ifndef GETOFFLINEMESSAGEINTERNAL_H
#define	GETOFFLINEMESSAGEINTERNAL_H

#include "masteragent/messages/ZMessageDef.h"
#include "masteragent/ZBufferWrapper.h"

BEG_NSP_MASTERAGENT();

class GetOfflineMessageInternal {
public:
	int32_t zpwType;
	int32_t zpwVer;
	std::xa_string zpwSek;
	std::xa_string callback;
	std::xa_string encryptKey; // encryptKey != null => connection is verified
	std::xa_string recvOnly;
	std::xa_string params;

public:

	__CLS_XAL_DECL(GetOfflineMessageInternal);

	GetOfflineMessageInternal()
	: zpwType(0)
	, zpwVer(0)
	, zpwSek()
	, callback()
	, encryptKey()
	, recvOnly()
	, params() {
	}

	~GetOfflineMessageInternal() {
	}

public:

	uint32_t headerSize() const {
		return 22 + zpwSek.size() + callback.size() + encryptKey.size() + recvOnly.size() + params.size();
	}

	uint32_t dataSize() const {
		return headerSize();
	}

public:

	uint8_t* serialize(uint32_t& len) {
		// calc size
		len = dataSize();
		uint8_t* ret = (uint8_t*) ZMALLOC(len);
		if (ret == NULL) {
			throw std::bad_alloc();
		}

		if (!_serialize(ret, len)) {
			ZFREE(ret);
			return NULL;
		}
		return ret;
	}

	bool serialize(ZBuffer::BufferPtr& buf, uint32_t& len) {
		uint8_t* ret = serialize(len);
		if (ret == NULL) {
			return false;
		}
		buf.reset(new ZBuffer::Buffer(ret));
		return true;
	}

	bool serialize(uint8_t* pData, uint32_t len) {
		if (len != dataSize()) return false;
		return _serialize(pData, len);
	}

	bool deserialize(const uint8_t* data, uint32_t len) {
		if (data == NULL) return false;
		return _deserialize(data, len);
	}

	std::xa_string toString() const {
		return xstring::Format(
				"GetOfflineMessageInternal: {"
				"zpwType:%d"
				";zpwVer:%d"
				";zpwSek:%s"
				";callback:%s"
				";encryptKey:%s"
				";recvOnly:%s"
				";params:%s"
				"}"
				, zpwType
				, zpwVer
				, zpwSek.c_str()
				, callback.c_str()
				, encryptKey.c_str()
				, recvOnly.c_str()
				, params.c_str()
				);
	}

private: // serialize

	bool _serialize(uint8_t* data, uint32_t len) const {
		ZBufferWrapper buf(data, len);
		// zpwType: 4
		if (!buf.writeI32(zpwType)) {
			return false;
		}
		// zpwVer: 4
		if (!buf.writeI32(zpwVer)) {
			return false;
		}
		// zpwSek: 4 + ...
		if (!buf.writeStringS4(zpwSek)) {
			return false;
		}
		// callback: 2 + ...
		if (!buf.writeStringS2(callback)) {
			return false;
		}
		// encryptKey: 2 + ...
		if (!buf.writeStringS2(encryptKey)) {
			return false;
		}
		// recvOnly: 2 + ...
		if (!buf.writeStringS2(recvOnly)) {
			return false;
		}
		// params: 4 + ...
		if (!buf.writeStringS4(params)) {
			return false;
		}
		return buf.sizeRemain() == 0;
	}

private: // deserialize

	bool _deserialize(const uint8_t* data, uint32_t len) {
		ZBufferWrapper buf(const_cast<uint8_t*> (data), len);
		// zpwType: 4
		if (!buf.readI32(zpwType)) {
			return false;
		}
		// zpwVer: 4
		if (!buf.readI32(zpwVer)) {
			return false;
		}
		// zpwSek: 4 + ...
		if (!buf.readStringS4(zpwSek)) {
			return false;
		}
		// callback: 2 + ...
		if (!buf.readStringS2(callback)) {
			return false;
		}
		// encryptKey: 2 + ...
		if (!buf.readStringS2(encryptKey)) {
			return false;
		}
		// recvOnly: 2 + ...
		if (!buf.readStringS2(recvOnly)) {
			return false;
		}
		// params: 4 + ...
		if (!buf.readStringS4(params)) {
			return false;
		}
		return buf.sizeRemain() == 0;
	}
};

END_NSP_MASTERAGENT();

#endif	/* GETOFFLINEMESSAGEINTERNAL_H */

