/* 
 * File:   ZBuffer.h
 * Author: tiennv4
 *
 * Created on November 14, 2012, 5:56 PM
 */

#ifndef ZBUFFER_H
#define	ZBUFFER_H

#include <zchatserver/common/common.h>
#include <xallocator/xal.h>

#include <string.h>

class ZBuffer {
public:

	class Node {
	private:
		uint8_t* _buf;
		size_t _bufSize;
		Node* _next;

	public:
		__CLS_XAL_DECL(Node);

		Node()
		: _buf(NULL)
		, _bufSize(0)
		, _next(NULL) {
		}

		Node(uint8_t* buf, size_t bufSize, bool ownership)
		: _buf(NULL)
		, _bufSize(bufSize)
		, _next(NULL) {
			if (ownership) {
				_buf = buf;
			} else {
				_buf = (uint8_t*) xa_malloc(bufSize);
				memcpy(_buf, buf, bufSize);
			}
		}

		~Node() {
			if (_buf) {
				xa_free(_buf);
			}
		}

		void setBuf(uint8_t* buf, size_t bufSize) {
			if (_buf) {
				xa_free(_buf);
			}
			_buf = buf;
			_bufSize = bufSize;
		}

		uint8_t* getBuf() {
			return _buf;
		}

		size_t getBufSize() const {
			return _bufSize;
		}

		Node* getNext() {
			return _next;
		}

		const Node* getNext() const {
			return _next;
		}

		void attachAfter(Node *node) {
			_next = node->_next;
			node->_next = this;
		}

	protected:
		Node(const Node&);
	};

public:

	ZBuffer();
	~ZBuffer();

public:

	// write data to fd
	int32_t write(int fd, size_t len);

	// take ownership data if ownership is true
	void add(uint8_t* data, size_t len, bool ownership);

	// remove data
	void remove(size_t len);

	// clear data
	void clear();

	// get data size
	size_t size() const;

	// check data is empty
	bool empty() const;

	// dump information to screen
	void dump() const;

private:

	// write using writev
	int _writeiovec(int fd, size_t len);

	// write using write
	int _writeio(int fd, size_t len);

	// add node to tail
	void _addTail(Node* node);

	// remove head and return next node
	Node* _removeHead();

private:

	Node* _head;
	Node* _tail;
	size_t _containerSize;
	size_t _pos;
	size_t _dataSize;
};

#endif	/* ZBUFFER_H */

