/* 
 * File:   ZBuffer.cpp
 * Author: tiennv4
 *
 * Created on November 14, 2012, 5:56 PM
 */

#include "zchatserver/ZBuffer.h"

#include <sys/uio.h>
#include <unistd.h>

#define MAX_SIZE_PER_BLOCK 8192
#define DEFAULT_WRITE_IOVEC 128	

#if defined(UIO_MAXIOV) && UIO_MAXIOV < DEFAULT_WRITE_IOVEC
#define NUM_WRITE_IOVEC UIO_MAXIOV
#elif defined(IOV_MAX) && IOV_MAX < DEFAULT_WRITE_IOVEC
#define NUM_WRITE_IOVEC IOV_MAX
#else
#define NUM_WRITE_IOVEC DEFAULT_WRITE_IOVEC
#endif

#define IOV_TYPE struct iovec
#define IOV_PTR_FIELD iov_base
#define IOV_LEN_FIELD iov_len
#define IOV_LEN_TYPE size_t

ZBuffer::ZBuffer()
: _head(NULL)
, _tail(NULL)
, _containerSize(0)
, _pos(0)
, _dataSize(0) {
}

ZBuffer::~ZBuffer() {
	clear();
}

int ZBuffer::write(int fd, size_t len) {
	if (_dataSize == 0) return 0;
	if (len == 0) return 0;
	if (len > _dataSize)
		len = _dataSize;
	if (_containerSize == 1) {
		// only one buffer then writeio
		return _writeio(fd, len);
	}
	return _writeiovec(fd, len);
}

void ZBuffer::add(uint8_t* data, size_t len, bool ownership) {
	if (len == 0) return;
	size_t curSize = 0;
	size_t newSize = 0;
	if (_tail != NULL) {
		if (_head == _tail) {
			curSize = _tail->getBufSize() - _pos;
			newSize = curSize + len;
			if (newSize <= MAX_SIZE_PER_BLOCK) {
				// realloc head buffer and append data to head (free written buffer)
				uint8_t* newBuf = (uint8_t*) xa_malloc(newSize);
				memcpy(newBuf, _tail->getBuf() + _pos, curSize);
				memcpy(newBuf + curSize, data, len);
				_tail->setBuf(newBuf, newSize);
				_pos = 0;
				_dataSize += len;
				if (ownership)
					xa_free(data);
				return;
			}
		} else {
			curSize = _tail->getBufSize();
			newSize = curSize + len;
			if (newSize <= MAX_SIZE_PER_BLOCK) {
				// realloc tail buffer and append data to tail
				uint8_t* newBuf = (uint8_t*) xa_malloc(newSize);
				memcpy(newBuf, _tail->getBuf(), curSize);
				memcpy(newBuf + curSize, data, len);
				_tail->setBuf(newBuf, newSize);
				_dataSize += len;
				if (ownership)
					xa_free(data);
				return;
			}
		}
	}

	// create new node
	Node* node = new Node(data, len, ownership);
	if (node == NULL)
		throw std::bad_alloc();
	_addTail(node);
	_dataSize += len;
}

void ZBuffer::clear() {
	Node* nTmp = NULL;
	while (_head != NULL) {
		nTmp = _head;
		_head = _head->getNext();
		delete nTmp;
	}
	_tail = NULL;
}

void ZBuffer::remove(size_t len) {
	if (len == 0) return;
	if (_dataSize == 0) return;
	if (len > _dataSize)
		len = _dataSize;
	int i = 0;
	size_t curSize = 0;
	for (; _head != NULL && len > 0; ++i) {
		// data length start at _pos if first block
		curSize = (i == 0 ? _head->getBufSize() - _pos : _head->getBufSize());
		if (curSize <= len) {
			_removeHead();
			len -= curSize;
			_dataSize -= curSize;
			_pos = 0;
		} else {
			_dataSize -= len;
			_pos += len;
			break;
		}
	}
}

size_t ZBuffer::size() const {
	return _dataSize;
}

bool ZBuffer::empty() const {
	return _dataSize == 0;
}

void ZBuffer::dump() const {
	printf("pos: %zu\n", _pos);
	printf("data size: %zu\n", _dataSize);
	printf("container size: %zu\n", _containerSize);
	Node* pNode = _head;
	while (pNode != NULL) {
		printf("+ bufSize: %zu\n", pNode->getBufSize());
		pNode = pNode->getNext();
	}
}

int ZBuffer::_writeiovec(int fd, size_t len) {
	// ensure _dataSize > 0

	int numIOVec = NUM_WRITE_IOVEC > _containerSize ? _containerSize : NUM_WRITE_IOVEC;
	int i = 0;
	size_t curSize = 0;
	Node* pNode = _head;
	IOV_TYPE iov[numIOVec];

	// Behavior for first chunk
	iov[i].IOV_PTR_FIELD = pNode->getBuf() + _pos;
	curSize = pNode->getBufSize() - _pos;

	if (curSize < len) {
		iov[i++].IOV_LEN_FIELD = curSize;
		len -= curSize;
		pNode = pNode->getNext();
	} else {
		iov[i++].IOV_LEN_FIELD = len;
		goto done;
	}

	// Behavior for other chunk
	while (pNode != NULL && i < numIOVec && len > 0) {
		curSize = pNode->getBufSize();
		iov[i].IOV_PTR_FIELD = pNode->getBuf();
		if (curSize <= len) {
			iov[i].IOV_LEN_FIELD = curSize;
			len -= curSize;
		} else {
			iov[i++].IOV_LEN_FIELD = len;
			goto done;
		}
		++i;
		pNode = pNode->getNext();
	}

done:
	if (i <= 0)
		return 0;
	return ::writev(fd, iov, i);
}

int ZBuffer::_writeio(int fd, size_t len) {
	// ensure _dataSize > 0
	size_t size = _head->getBufSize() - _pos;
	return ::write(fd, _head->getBuf() + _pos, (size > len ? len : size));
}

void ZBuffer::_addTail(Node* node) {
	++_containerSize;
	if (_head == NULL) {
		_head = _tail = node;
	} else {
		node->attachAfter(_tail);
		_tail = node;
	}
}

ZBuffer::Node * ZBuffer::_removeHead() {
	// ensure _head != NULL
	--_containerSize;
	Node* nTmp = _head;
	_head = _head->getNext();
	if (_head == NULL)
		_tail = NULL;
	delete nTmp;
	return _head;
}
