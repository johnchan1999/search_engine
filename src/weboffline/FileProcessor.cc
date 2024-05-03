#include "../../Include/FileProcessor.h"
#include "../../Include/tinyxml2.h"
#include <iostream>
#include <sys/stat.h>
#include <regex>


using namespace tinyxml2;
using std::cerr;
using std::endl;
using std::regex;


//获取Rss文件中channel节点下item节点的内容，存入结构体vector
void FileProcessor::parseRss(const char *filename, vector<RssItem> &_rss)
{
    XMLDocument xml;                                                              // 1、声明xml类，读取xml文件
    xml.LoadFile(filename);                                                       // 2、读取xml文件  LoadFile传入的要是C风格字符串
    XMLElement *rss = xml.RootElement();                                          // 3、拿到xml文件的根节点rss
    XMLElement *channel = rss->FirstChildElement("channel");                      // 4、拿到channel节点指针
    XMLElement *item = channel->FirstChildElement("item");                        // 5、拿到item节点

    while(item)
    {
        RssItem rss_item;                                                         //数据保存到结构体
        regex reg("<[^>]*>");                                                     //正则表达式去除html标签

                                                
        XMLElement *item_title = item->FirstChildElement("title");                //获取title节点指针，内容保存到结构体变量
        if(item_title)
        {
            rss_item.title = item_title->GetText();                               //保存节点的内容
        }

        //获取link节点指针，内容保存到结构体变量
        XMLElement *item_link = item->FirstChildElement("link");
        if(item_link)
        {
            rss_item.link = filename;
        }

        //获取desc节点指针，内容保存到结构体变量
        XMLElement *item_description = item->FirstChildElement("description");
        if(item_description)
        {
            rss_item.description = item_description->GetText();
        }

        //去除html标签 
        rss_item.description = regex_replace(rss_item.description, reg, ""); 

        XMLElement *item_content = item->FirstChildElement("content");
        if(item_content)
        {
            rss_item.content = item_content->GetText(); 
        } 

        rss_item.content = regex_replace(rss_item.content, reg, "");

        _rss.push_back(rss_item);                                                      //存入结构体
        item = item->NextSiblingElement();                                             //寻找下一个item节点
    }
}


