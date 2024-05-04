#ifndef __LOG4CPP_H__
#define __LOG4CPP_H__

#include <log4cpp/Category.hh>
#include <log4cpp/Appender.hh>
#include <log4cpp/FileAppender.hh>
#include <log4cpp/OstreamAppender.hh>
#include <log4cpp/Layout.hh>
#include <log4cpp/BasicLayout.hh>
#include <log4cpp/Priority.hh>
#include <log4cpp/PatternLayout.hh>
#include <string>

#define LOGPATTERN(x, y) (std::string("[FILE:") + (x) + "] [Line:" + std::to_string(y) + "]")

namespace SearchEngine
{
    using std::string;

    class MyLog
    {
    public:
        MyLog()
            : _appender(new log4cpp::FileAppender("default", "../log/programe.log")), _user(log4cpp::Category::getInstance("johnchan"))
        {
            _user.setPriority(log4cpp::Priority::DEBUG);
            log4cpp::PatternLayout *layout = new log4cpp::PatternLayout();
            layout->setConversionPattern("%d{%Y-%m-%d %H:%M:%S} [%p] %c: %m%n");
            _appender->setLayout(layout);
            _user.addAppender(_appender);
        }

        MyLog(const string &logfilename, const string &usrname)
            : _appender(new log4cpp::FileAppender("default", logfilename)), _user(log4cpp::Category::getRoot())
        {
            _appender->setLayout(new log4cpp::BasicLayout());
            _user.addAppender(_appender);
            _user.setPriority(log4cpp::Priority::DEBUG);
        }

        ~MyLog()
        {
            log4cpp::Category::shutdown();
        }

        void error(const string &msg)
        {
            _user.error(msg);
        }

        void warn(const string &msg)
        {
            _user.warn(msg);
        }

        void info(const string &msg)
        {
            _user.info(msg);
        }

        void debug(const string &msg)
        {
            _user.debug(msg);
        }

    private:
        log4cpp::Appender *_appender;
        log4cpp::Category &_user;
    };

}; // end of SearchEngine

#endif