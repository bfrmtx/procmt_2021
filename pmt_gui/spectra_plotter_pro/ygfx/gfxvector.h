#ifndef YGFX_GL_VECTOR_H
#define YGFX_GL_VECTOR_H

// stl-includes
#include <array>
#include <vector>
#include <iostream>
#include <type_traits>
#include <assert.h>

// ygfx-includes
#include "gfxarraybasecontainer.h"

// gl-includes
#include "gfxincludes.h"

#define YGFXVECTOR_TH_UP        0.75
#define YGFXVECTOR_TH_DOWN      0.25
#define YGFXVECTOR_SCALE_UP     2.0
#define YGFXVECTOR_SCALE_DOWN   0.5
#define YGFXVECTOR_DEFAULTCAP   16

using namespace ygfx;

template<typename T,
         BufferTarget TARGET = BufferTarget::ARRAY_BUFFER,
         BufferDataUsage USAGE = BufferDataUsage::DYNAMIC_DRAW>
class gfxVector : public gfxArrayBasedContainer<T, TARGET, USAGE> {
private:
    GLuint vectorsize;

public:
    explicit gfxVector() :
        gfxArrayBasedContainer<T, TARGET>(YGFXVECTOR_DEFAULTCAP),
        vectorsize(0)
    {}

    template<class Iter>
    gfxVector(Iter begin, Iter end) :
        gfxVector()
    {
        while (begin != end) {
            push_back(*begin);
            begin++;
        }
    }

    /**
     * just a lazy method, without type-checking. Don't use it if you don't know what you are doing!
     */
    template<typename S>
    gfxVector(const std::vector<S>& stlvector) :
        gfxVector()
    {
        for (const T& val : stlvector) {
            push_back(T(val));
        }
    }

    gfxVector(const std::vector<T>& stlvector) :
        gfxVector()
    {
        reserve(stlvector.size());
        for (const T& val : stlvector) {
            push_back(val);
        }
    }

    gfxVector(gfxVector<T>& second) :
        gfxArrayBasedContainer<T, TARGET>(second.buffercapacity),
        vectorsize(second.vectorsize)
    {
        second.enableMapping(true);
        for (GLuint i = 0; i < second.vectorsize; ++i) {
            push_back(second.at(i));
        }
        second.enableMapping(false);
    }

    ~gfxVector() { }

    gfxVector& operator=(gfxVector& second) {
        second.enableMapping(true);
        for (GLuint i = 0; i < second.vectorsize; ++i) {
            push_back(second.at(i));
        }
        second.enableMapping(false);
        this->enableMapping(false);
        return *this;
    }

    bool empty() const {
        return vectorsize == 0;
    }

    GLuint size() const {
        return vectorsize;
    }

    void clear() {
        vectorsize = 0;
        this->buffercapacity = 4;
        resizeBuffer(4);
    }

    void pop_back() {
        Q_ASSERT(vectorsize > 0);
        if (vectorsize > 0) {
            --vectorsize;
            decSize();
        }
    }

    void push_back(T data) {
        this->enableMapping(true);
        this->datapointer[vectorsize] = data;
        ++vectorsize;
        incSize();
    }

    void push_back(T const * dataPtr, size_t size) {
        insert(vectorsize, dataPtr, size);
    }

    void push_back(const std::initializer_list<T>& values) {
        insert(vectorsize, values);
    }

    void insert(GLuint position, T const * valPtr, size_t size) {
        this->enableMapping(true);
        for (size_t index = 0; index < size; ++index) {
            if (index + position < vectorsize) this->datapointer[position + index] = valPtr[index];
            else push_back(valPtr[index]);
        }
    }

    void insert(GLuint position, const std::initializer_list<T>& values) {
        this->enableMapping(true);
        int index = 0;
        for (auto& iterator : values) {
            if (index + position < vectorsize) this->datapointer[position + index] = iterator;
            else push_back(iterator);
            ++index;
        }
    }


    void resize(GLuint newsize) {
        if (newsize > vectorsize) {
            vectorsize = newsize;
            incSize();
        }
        else if (newsize < vectorsize) {
            vectorsize = newsize;
            decSize();
        }
    }

    void reserve(GLuint newcap) {
        this->buffercapacity = newcap;
        resizeBuffer(this->buffercapacity);
    }

    void shrink_to_fit() {
        this->buffercapacity = vectorsize + 1;
        resizeBuffer(this->buffercapacity);
    }

private:
    void incSize() {
        if (vectorsize > this->buffercapacity * YGFXVECTOR_TH_UP) {
            this->buffercapacity = static_cast<GLuint>(this->buffercapacity * YGFXVECTOR_SCALE_UP);
            while (vectorsize > this->buffercapacity * YGFXVECTOR_TH_UP) {
                this->buffercapacity = static_cast<GLuint>(this->buffercapacity * YGFXVECTOR_SCALE_UP);
            }
            resizeBuffer(this->buffercapacity);
        }
    }

    void decSize() {
        if (vectorsize < this->buffercapacity * YGFXVECTOR_TH_DOWN && this->buffercapacity > YGFXVECTOR_DEFAULTCAP) {
            this->buffercapacity = static_cast<GLuint>(this->buffercapacity * YGFXVECTOR_SCALE_DOWN);
            while (vectorsize > this->buffercapacity * YGFXVECTOR_TH_DOWN && this->buffercapacity > YGFXVECTOR_DEFAULTCAP) {
                this->buffercapacity = static_cast<GLuint>(this->buffercapacity * YGFXVECTOR_SCALE_DOWN);
            }
            resizeBuffer(this->buffercapacity);
        }
    }

    void resizeBuffer(GLuint newsize) {
        this->enableMapping(true);
        GLuint newbuffer;
        OGLFUNCS glGenBuffers(1, &newbuffer);
        OGLFUNCS glBindBuffer(static_cast<GLenum>(TARGET), newbuffer);
        OGLFUNCS glBufferData(static_cast<GLenum>(TARGET), sizeof(T) * newsize, nullptr, GL_DYNAMIC_DRAW);
        OGLFUNCS glBufferSubData(static_cast<GLenum>(TARGET), 0, vectorsize * sizeof(T), this->datapointer);
        this->enableMapping(false);
        OGLFUNCS glDeleteBuffers(1, &this->vertexbufferID);
        this->vertexbufferID = newbuffer;
    }

};

template<typename T>
std::ostream& operator<< (std::ostream& ostr, const gfxVector<T>& vector)
{
    ostr << "[gfxVector|size:" << vector.size() << "|cap:" << vector.capacity() << "|vbo:" << vector.getVBO() << "]" << Qt::endl;
    ostr << "{";
    for (GLuint i = 0; i < vector.size(); ++i) {
        ostr << vector[i];
        if (i < vector.size() - 1) ostr << ", ";
    }
    ostr << "}";
    return ostr;
}

#undef YGFXVECTOR_TH_UP
#undef YGFXVECTOR_TH_DOWN
#undef YGFXVECTOR_SCALE_UP
#undef YGFXVECTOR_SCALE_DOWN
#undef YGFXVECTOR_DEFAULTCAP
#endif
