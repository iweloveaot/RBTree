#ifndef IENUMERABLE_H
#define IENUMERABLE_H

template <typename T>
class IEnumerator {
public:
    virtual ~IEnumerator() {}
    virtual bool MoveNext() = 0;
    virtual const T& GetCurrent() const = 0;
    virtual void Reset() = 0;
};

template <typename T>
class IEnumerable {
public:
    virtual ~IEnumerable() {}
    virtual IEnumerator<T>* GetEnumerator() const = 0;
};

#endif
