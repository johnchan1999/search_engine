#ifndef __NONCOPYABLE_H__
#define __NONCOPYABLE_H__

namespace OurPool
{
    class NonCopyable
    {
    protected:
        NonCopyable() {}
        ~NonCopyable() {}

    private:
        NonCopyable(const NonCopyable &rhs);
        NonCopyable &operator=(const NonCopyable &rhs) = delete;
    };

}; // end of OurPool

#endif
