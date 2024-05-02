 #ifndef __MMISCHINESE_H__
 #define __MMISCHINESE_H__

#include <string>

 namespace SearchEngine
 {
	// 判断是否为UTF8字符
    static size_t getByteNum_utf8(const char byte)
    {
        int byteNum = 0; 
        for (size_t i = 0; i < 6; ++i)
        {
            if (byte & (1 << (7 - i)))
            {
                ++byteNum;
            }
            else
            {
                break;
            }
        }
        return byteNum == 0 ? 1 : byteNum;
    }
 };

#endif